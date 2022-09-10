
#include "component_manager.hpp"
#include "database/model/FinalStage.hxx"

#include "service/network/log.hpp"

#include "common/string.hpp"
#include "common/file.hpp"

#include <boost/dll/import.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/filesystem/operations.hpp>

#include <cstddef>

namespace mega
{
namespace runtime
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
    boost::filesystem::copy( path, tempDllPath );

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
    for ( const std::string& strSymbol : m_libraryInfo.symbols( "mega" ) )
    {
        SPDLOG_TRACE( "RUNTIME: InterfaceComponent symbol : {}", strSymbol );
        mega::TypeID interfaceTypeID = 0U;
        {
            // symbold is always '_ma123'
            VERIFY_RTE_MSG( strSymbol.size() > 3, "Invalid symbol in mega alias section: " << strSymbol );
            const std::string  strID( strSymbol.begin() + 3, strSymbol.end() );
            std::istringstream is( strID );
            is >> interfaceTypeID;
            VERIFY_RTE_MSG( interfaceTypeID != 0U, "Invalid type ID decoded from symbol: " << strSymbol );
        }
        ComponentManager::FunctionPtr pFunctionPtr = boost::dll::import_alias< TypeErasedFunction* >( m_library, strSymbol );
        VERIFY_RTE_MSG( functions.insert( { interfaceTypeID, pFunctionPtr } ).second,
                        "Duplicate interface symbol found for: " << strSymbol );
    }
}

ComponentManager::ComponentManager( const mega::network::Project& project, DatabaseInstance& database )
    : m_project( project )
    , m_database( database )
{
}

TypeErasedFunction ComponentManager::getOperationFunctionPtr( mega::TypeID concreteTypeID )
{
    SPDLOG_TRACE( "RUNTIME: ComponentManager getOperation : {}", concreteTypeID );

    const mega::TypeID interfaceTypeID = m_database.getInterfaceTypeID( concreteTypeID );

    {
        auto iFind = m_functions.find( interfaceTypeID );
        if ( iFind != m_functions.end() )
        {
            return iFind->second.get();
        }
    }

    {
        const FinalStage::Components::Component* pDBComponent = m_database.getOperationComponent( concreteTypeID );
        const std::string&                       strComponentFileName = pDBComponent->get_file_name();
        const boost::filesystem::path            componentPath = m_project.getProjectBin() / strComponentFileName;

        auto iFind = m_interfaceComponents.find( componentPath );
        if ( iFind == m_interfaceComponents.end() )
        {
            InterfaceComponent::Ptr pComponent = std::make_shared< InterfaceComponent >( componentPath, m_functions );
            m_interfaceComponents.insert( { componentPath, pComponent } );
        }
    }

    {
        auto iFind = m_functions.find( interfaceTypeID );
        if ( iFind != m_functions.end() )
        {
            return iFind->second.get();
        }
    }

    SPDLOG_ERROR( "ComponentManager failed to locate symbol : {}", concreteTypeID );
    THROW_RTE( "Failed to locate symbol: " << concreteTypeID );
}

} // namespace runtime
} // namespace mega