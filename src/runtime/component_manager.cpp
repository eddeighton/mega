//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#include "component_manager.hpp"
#include "database/FinalStage.hxx"

#include "database/sources.hpp"
#include "log/log.hpp"

#include "common/string.hpp"
#include "common/file.hpp"

#include <boost/dll/import.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/filesystem/operations.hpp>

#include <boost/function.hpp>

#include <cstddef>

namespace mega::runtime
{

ComponentManager::ComponentPath ComponentManager::InterfaceComponent::makeTempComponent( const ComponentPath& path )
{
    SPDLOG_TRACE( "RUNTIME: InterfaceComponent::makeTempComponent : {}", path.string() );

    VERIFY_RTE_MSG( boost::filesystem::exists( path ), "Failed to locate component: " << path.string() );

    boost::filesystem::path tempDir = boost::filesystem::temp_directory_path() / "mega_registry";
    boost::filesystem::ensureFoldersExist( tempDir / "test" );
    VERIFY_RTE_MSG( boost::filesystem::exists( tempDir ), "Failed to create temporary folder: " << tempDir.string() );

    std::ostringstream osTempFileName;
    {
        osTempFileName << common::uuid() << "_" << path.filename().string();
    }

    const boost::filesystem::path tempDllPath = tempDir / osTempFileName.str();
    boost::filesystem::copy_file( path, tempDllPath, boost::filesystem::copy_options::synchronize );

    SPDLOG_TRACE( "RUNTIME: ComponentManager created : {}", tempDllPath.string() );

    return tempDllPath;
}

ComponentManager::InterfaceComponent::InterfaceComponent( const ComponentPath&              path,
                                                          ComponentManager::FunctionPtrMap& functions )
    : m_path( path )
    , m_tempPath( makeTempComponent( m_path ) )
    , m_libraryInfo( m_tempPath )
    , m_library( m_tempPath )
{
    SPDLOG_TRACE( "RUNTIME: InterfaceComponent loaded : {}", m_path.string() );

    // load all functions from mega alias section
    for( const std::string& strSymbol : m_libraryInfo.symbols( "mega" ) )
    {
        SPDLOG_TRACE( "RUNTIME: InterfaceComponent symbol : {}", strSymbol );
        mega::TypeID interfaceTypeID;
        {
            // c alias symbol:        '_ca0x8efefd52e1eb2505_65562'
            // python alias symbol:   '_pa0x1077e5fca12a0068_65559'
            VERIFY_RTE_MSG( strSymbol.size() > 22, "Invalid symbol in mega alias section: " << strSymbol );
            const std::string  strID( strSymbol.begin() + 22, strSymbol.end() );
            std::istringstream is( strID );
            is >> interfaceTypeID;
            VERIFY_RTE_MSG( interfaceTypeID != mega::TypeID{}, "Invalid type ID decoded from symbol: " << strSymbol );
        }
        ComponentManager::FunctionPtr pFunctionPtr
            = boost::dll::import_alias< TypeErasedFunction* >( m_library, strSymbol );
        VERIFY_RTE_MSG( functions.insert( { interfaceTypeID, pFunctionPtr } ).second,
                        "Duplicate interface symbol found for: " << strSymbol );
    }
}

void ComponentManager::InterfaceComponent::setPythonCaster( const std::string& strComponentName,
                                                            void*              pInterface ) const
{
    std::ostringstream osCasterSymbol;
    osCasterSymbol << '_' << strComponentName << "_setPythonCaster";

    typedef void( CasterFunctionPtr )( void* );
    boost::function< CasterFunctionPtr > pFunction = m_library.get_alias< CasterFunctionPtr >( osCasterSymbol.str() );

    VERIFY_RTE_MSG( pFunction, "Failed to resolve component python caster" );
    pFunction( pInterface );
}

ComponentManager::ComponentManager( const mega::Project& project, JITDatabase& database )
    : m_project( project )
    , m_database( database )
{
}

network::ComponentMgrStatus ComponentManager::getStatus() const
{
    network::ComponentMgrStatus status;

    for( const auto& [ path, pInterfaceComponent ] : m_interfaceComponents )
    {
        ++status.m_interfaceComponents;
    }
    for( const auto& [ path, pInterfaceComponent ] : m_pythonComponents )
    {
        ++status.m_pythonComponents;
    }

    return status;
}

TypeErasedFunction ComponentManager::getOperationFunctionPtr( mega::TypeID interfaceTypeID )
{
    SPDLOG_TRACE( "RUNTIME: ComponentManager getOperationFunctionPtr : {}", interfaceTypeID );

    {
        auto iFind = m_functions.find( interfaceTypeID );
        if( iFind != m_functions.end() )
        {
            return iFind->second.get();
        }
    }

    {
        const FinalStage::Components::Component* pDBComponent = m_database.getOperationComponent( interfaceTypeID );
        const mega::io::ComponentFilePath&       componentBuildPath = pDBComponent->get_file_path();
        const boost::filesystem::path componentPath = m_project.getProjectBin() / componentBuildPath.path().filename();

        auto iFind = m_interfaceComponents.find( componentPath );
        if( iFind == m_interfaceComponents.end() )
        {
            InterfaceComponent::Ptr pComponent = std::make_shared< InterfaceComponent >( componentPath, m_functions );
            m_interfaceComponents.insert( { componentPath, pComponent } );
        }
    }

    {
        auto iFind = m_functions.find( interfaceTypeID );
        if( iFind != m_functions.end() )
        {
            return iFind->second.get();
        }
    }

    SPDLOG_ERROR( "ComponentManager failed to locate symbol : {}", interfaceTypeID );
    THROW_RTE( "Failed to locate symbol: " << interfaceTypeID );
}

TypeErasedFunction ComponentManager::getPythonFunctionPtr( mega::TypeID interfaceTypeID, void* pPythonCaster )
{
    SPDLOG_TRACE( "RUNTIME: ComponentManager getPythonFunctionPtr : {}", interfaceTypeID );

    {
        auto iFind = m_pythonFunctions.find( interfaceTypeID );
        if( iFind != m_pythonFunctions.end() )
        {
            return iFind->second.get();
        }
    }

    {
        const FinalStage::Components::Component* pDBComponent = m_database.getOperationComponent( interfaceTypeID );
        const mega::io::ComponentFilePath&       componentBuildPath = pDBComponent->get_python_file_path();
        const boost::filesystem::path componentPath = m_project.getProjectBin() / componentBuildPath.path().filename();

        SPDLOG_TRACE( "Attempting to load python component: {}", componentPath.string() );

        auto iFind = m_pythonComponents.find( componentPath );
        if( iFind == m_pythonComponents.end() )
        {
            InterfaceComponent::Ptr pComponent
                = std::make_shared< InterfaceComponent >( componentPath, m_pythonFunctions );
            m_pythonComponents.insert( { componentPath, pComponent } );

            // configure the python caster interface
            pComponent->setPythonCaster( pDBComponent->get_name(), pPythonCaster );
        }
    }

    {
        auto iFind = m_pythonFunctions.find( interfaceTypeID );
        if( iFind != m_pythonFunctions.end() )
        {
            return iFind->second.get();
        }
    }

    SPDLOG_ERROR( "ComponentManager failed to locate symbol : {}", interfaceTypeID );
    THROW_RTE( "Failed to locate symbol: " << interfaceTypeID );
}

} // namespace mega::runtime
