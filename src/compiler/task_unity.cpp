
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

#include "base_task.hpp"

#include "mega/values/compilation/interface/relation_id.hpp"
#include "mega/common_strings.hpp"

#include "database/UnityStage.hxx"
#include "database/UnityStageView.hxx"

#include "database/component_type.hpp"
#include "database/sources.hpp"

#include "nlohmann/json.hpp"

#include <common/stash.hpp>
#include <common/file.hpp>

namespace UnityStage
{
#include "compiler/interface_printer.hpp"
}

namespace mega::compiler
{
namespace
{
bool writeJSON( const boost::filesystem::path& filePath, const nlohmann::json& data )
{
    std::ostringstream os;
    os << data;
    return boost::filesystem::updateFileIfChanged( filePath, os.str() );
}
} // namespace

class Task_UnityReflection : public BaseTask
{
public:
    Task_UnityReflection( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const boost::filesystem::path unityDataFilePath = m_environment.UnityReflection();

        const auto symbolTableCompilationFile
            = m_environment.SymbolAnalysis_SymbolTable( m_environment.project_manifest() );

        start( taskProgress, "Task_UnityReflection", m_environment.project_manifest().path(),
               unityDataFilePath.filename() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( symbolTableCompilationFile ) } );

        if( m_environment.restorePath( unityDataFilePath, determinant ) )
        {
            m_environment.setBuildHashCodePath( unityDataFilePath );
            cached( taskProgress );
            return;
        }

        {
            using namespace UnityStage;
            Database database( m_environment, m_manifest );
            {
                std::unordered_map< std::string, MegaMangle::Mangle* > mangleMap;
                for( auto pMangle : database.many< MegaMangle::Mangle >( m_environment.project_manifest() ) )
                {
                    mangleMap.insert( { pMangle->get_canon(), pMangle } );
                }

                // generate unity reflection data file
                // clang-format off
                nlohmann::json data(
                {
                    { "objects",    nlohmann::json::array() }, 
                    { "dimensions", nlohmann::json::array() },
                    { "links",      nlohmann::json::array() }, 
                    { "structure",  nlohmann::json::array() }
                });
                // clang-format on

                {
                    Symbols::SymbolTable* pSymbolTable
                        = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

                    for( const auto& [ id, pInterfaceType ] : pSymbolTable->get_interface_type_ids() )
                    {
                        std::ostringstream osFullTypeName;
                        if( pInterfaceType->get_context().has_value() )
                        {
                            Interface::IContext* pContext = pInterfaceType->get_context().value();

                            if( Interface::Object* pObject = db_cast< Interface::Object >( pContext ) )
                            {
                                osFullTypeName << Interface::printIContextFullType( pContext );
                                nlohmann::json typeInfo{
                                    { "symbol", id.getSymbolID() }, { "name", osFullTypeName.str() } };
                                data[ "objects" ].push_back( typeInfo );
                            }
                        }
                        else if( pInterfaceType->get_dimension().has_value() )
                        {
                            Interface::DimensionTrait* pDimension = pInterfaceType->get_dimension().value();
                            printDimensionTraitFullType( pDimension, osFullTypeName );

                            const std::string strCanon = pDimension->get_canonical_type();
                            auto              iFind    = mangleMap.find( strCanon );
                            if( iFind == mangleMap.end() )
                            {
                                std::ostringstream os;
                                os << "Failed to locate mangle for canonical type: " << strCanon
                                   << " for: " << pDimension->get_interface_id();
                                msg( taskProgress, os.str() );
                            }
                            using namespace std::string_literals;
                            nlohmann::json typeInfo{
                                { "symbol", id.getSymbolID() },
                                { "name", osFullTypeName.str() },
                                { "blit", ( iFind != mangleMap.end() ) ? iFind->second->get_blit() : ""s } };
                            data[ "dimensions" ].push_back( typeInfo );
                        }
                        else if( pInterfaceType->get_link().has_value() )
                        {
                            Interface::LinkTrait* pLink = pInterfaceType->get_link().value();
                            printLinkTraitFullType( pLink, osFullTypeName );
                            nlohmann::json typeInfo{ { "symbol", id.getSymbolID() }, { "name", osFullTypeName.str() } };
                            data[ "links" ].push_back( typeInfo );
                        }
                        else
                        {
                            THROW_RTE( "Unknown interface type" );
                        }
                    }
                }

                writeJSON( unityDataFilePath, data );
            }
        }

        m_environment.setBuildHashCodePath( unityDataFilePath );
        m_environment.stashPath( unityDataFilePath, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_UnityReflection( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_UnityReflection >( taskArguments );
}

class Task_UnityAnalysis : public BaseTask
{
public:
    Task_UnityAnalysis( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const boost::filesystem::path unityDataFilePath     = m_environment.UnityReflection();
        const boost::filesystem::path unityAnalysisFilePath = m_environment.UnityAnalysis();

        start( taskProgress, "Task_UnityAnalysis", unityDataFilePath.filename(), unityAnalysisFilePath.filename() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCodePath( unityDataFilePath ) } );

        if( boost::filesystem::exists( unityAnalysisFilePath ) )
        {
            m_environment.setBuildHashCodePath( unityAnalysisFilePath );
            m_environment.stashPath( unityAnalysisFilePath, determinant );
            cached( taskProgress );
            return;
        }

        if( m_environment.restorePath( unityAnalysisFilePath, determinant ) )
        {
            m_environment.setBuildHashCodePath( unityAnalysisFilePath );
            cached( taskProgress );
            return;
        }

        VERIFY_RTE_MSG( boost::filesystem::exists( m_unityProjectDir ),
                        "Could not locate unity project directory at: " << m_unityProjectDir.string() );

        VERIFY_RTE_MSG( boost::filesystem::exists( m_unityEditor ),
                        "Could not locate unity editor at: " << m_unityEditor.string() );

        {
            const boost::filesystem::path unityLog = m_environment.buildDir() / "unity_log.txt";

            std::ostringstream osCmd;

            osCmd << m_unityEditor.string() << " ";
            osCmd << "-projectPath " << m_unityProjectDir.string() << " ";
            osCmd << "-logFile " << unityLog.string() << " ";
            osCmd << "-noUpm -batchmode -quit -nographics -disable-gpu-skinning -disable-assembly-updater "
                     "-disableManagedDebugger ";
            osCmd << "-executeMethod analysis.UnityAnalysis.RunFromCmdLine ";
            osCmd << "-reflectionData " << unityDataFilePath.string() << " ";
            osCmd << "-unityData " << unityAnalysisFilePath.string() << " ";

            this->run_cmd( taskProgress, common::Command{ osCmd.str() } );
        }

        if( !boost::filesystem::exists( unityAnalysisFilePath ) )
        {
            failed( taskProgress );
        }
        else
        {
            m_environment.setBuildHashCodePath( unityAnalysisFilePath );
            m_environment.stashPath( unityAnalysisFilePath, determinant );
            succeeded( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_UnityAnalysis( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_UnityAnalysis >( taskArguments );
}

class Task_Unity : public BaseTask
{
    anifest;

public:
    Task_Unity( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const boost::filesystem::path unityAnalysisFilePath = m_environment.UnityAnalysis();
        const auto                    symbolTableCompilationFile
            = m_environment.SymbolAnalysis_SymbolTable( m_environment.project_manifest() );

        const mega::io::CompilationFilePath unityAnalysisCompilationFile
            = m_environment.UnityStage_UnityAnalysis( m_manifest );

        start( taskProgress, "Task_Unity", unityAnalysisFilePath.filename(), unityAnalysisCompilationFile.path() );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash,
                                                   m_environment.getBuildHashCodePath( unityAnalysisFilePath ),
                                                   m_environment.getBuildHashCode( symbolTableCompilationFile ) } );

        if( m_environment.restore( unityAnalysisCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( unityAnalysisCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace UnityStage;
        Database database( m_environment, m_manifest );

        Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

        // load the unityAnalysisFilePath
        nlohmann::json data;
        {
            auto pFile = boost::filesystem::loadFileStream( unityAnalysisFilePath );
            data       = nlohmann::json::parse( *pFile );
        }

        using BindingMap = std::map< TypeID, UnityAnalysis::ObjectBinding* >;
        BindingMap bindings;

        // generate Root prefab
        /*{
            Concrete::Object* pRootObject = nullptr;
            {
                for( const auto& [ _, pConcreteType ] : pSymbolTable->get_concrete_type_ids() )
                {
                    if( pConcreteType->get_id() == ROOT_TYPE_ID )
                    {
                        VERIFY_RTE( pConcreteType->get_context().has_value() );
                        pRootObject = db_cast< Concrete::Object >( pConcreteType->get_context().value() );
                        break;
                    }
                }
                VERIFY_RTE( pRootObject );
            }

            std::vector< UnityAnalysis::DataBinding* > dataBindings;
            std::vector< UnityAnalysis::LinkBinding* > linkBindings;
            {
                for( Concrete::Data::Link* pLink : pRootObject->get_all_links() )
                {
                    std::ostringstream osTypeName;
                    osTypeName << Interface::printIContextFullType( pLink->get_link() );
                    UnityAnalysis::LinkBinding* pLinkBinding
                        = database.construct< UnityAnalysis::LinkBinding >( UnityAnalysis::LinkBinding::Args{
                            osTypeName.str(), pLink->get_link()->get_interface_id() } );
                    linkBindings.push_back( pLinkBinding );
                }
            }

            UnityAnalysis::Prefab* pRootPrefab = database.construct< UnityAnalysis::Prefab >(
                UnityAnalysis::Prefab::Args{ "", "Root", ROOT_TYPE_ID, dataBindings, linkBindings } );
            bindings.insert( { ROOT_TYPE_ID, pRootPrefab } );
        }*/

        for( const auto& manual : data[ "manuals" ] )
        {
            std::vector< UnityAnalysis::DataBinding* > dataBindings;
            {
                for( const auto& dataBinding : manual[ "dataBindings" ] )
                {
                    UnityAnalysis::DataBinding* pDataBinding
                        = database.construct< UnityAnalysis::DataBinding >( UnityAnalysis::DataBinding::Args{
                            dataBinding[ "typeName" ],
                            mega::TypeID{ TypeID::ValueType{ dataBinding[ "interfaceTypeID" ] } } } );
                    dataBindings.push_back( pDataBinding );
                }
            }
            // THROW_TODO;
            /*std::vector< UnityAnalysis::LinkBinding* > linkBindings;
            {
                for( const auto& linkBinding : manual[ "linkBindings" ] )
                {
                    UnityAnalysis::LinkBinding* pLinkBinding
                        = database.construct< UnityAnalysis::LinkBinding >( UnityAnalysis::LinkBinding::Args{
                            linkBinding[ "typeName" ],
                            mega::TypeID{ TypeID::ValueType{ linkBinding[ "interfaceTypeID" ] } } } );
                    linkBindings.push_back( pLinkBinding );
                }
            }

            const TypeID interfaceTypeID{ TypeID::ValueType{ manual[ "interfaceTypeID" ] } };
            VERIFY_RTE_MSG( interfaceTypeID != TypeID{}, "Invalid type ID from unity for: " << manual[ "manualName" ] );
            UnityAnalysis::Manual* pManual = database.construct< UnityAnalysis::Manual >( UnityAnalysis::Manual::Args{
                UnityAnalysis::ObjectBinding::Args{ manual[ "typeName" ], interfaceTypeID, dataBindings, linkBindings },
                manual[ "manualName" ] } );
            bindings.insert( { interfaceTypeID, pManual } );*/
        }

        for( const auto& prefab : data[ "prefabs" ] )
        {
            std::vector< UnityAnalysis::DataBinding* > dataBindings;
            {
                for( const auto& dataBinding : prefab[ "dataBindings" ] )
                {
                    UnityAnalysis::DataBinding* pDataBinding
                        = database.construct< UnityAnalysis::DataBinding >( UnityAnalysis::DataBinding::Args{
                            dataBinding[ "typeName" ],
                            mega::TypeID{ TypeID::ValueType{ dataBinding[ "interfaceTypeID" ] } } } );
                    dataBindings.push_back( pDataBinding );
                }
            }
            // THROW_TODO;
            /*std::vector< UnityAnalysis::LinkBinding* > linkBindings;
            {
                for( const auto& linkBinding : prefab[ "linkBindings" ] )
                {
                    UnityAnalysis::LinkBinding* pLinkBinding
                        = database.construct< UnityAnalysis::LinkBinding >( UnityAnalysis::LinkBinding::Args{
                            linkBinding[ "typeName" ],
                            mega::TypeID{ TypeID::ValueType{ linkBinding[ "interfaceTypeID" ] } } } );
                    linkBindings.push_back( pLinkBinding );
                }
            }

            const TypeID interfaceTypeID{ TypeID::ValueType{ prefab[ "interfaceTypeID" ] } };
            VERIFY_RTE_MSG( interfaceTypeID != TypeID{}, "Invalid type ID from unity for: " << prefab[ "guid" ] );
            UnityAnalysis::Prefab* pPrefab = database.construct< UnityAnalysis::Prefab >( UnityAnalysis::Prefab::Args{
                UnityAnalysis::ObjectBinding::Args{ prefab[ "typeName" ], interfaceTypeID, dataBindings, linkBindings },
                prefab[ "guid" ] } );
            bindings.insert( { interfaceTypeID, pPrefab } );*/
        }

        using ObjectVector = std::vector< Concrete::Object* >;
        using DimMap       = std::multimap< Concrete::Object*, Concrete::Dimensions::User* >;

        ObjectVector objectVector;
        DimMap       dimMap;

        // THROW_TODO;
        /*using LinkMap      = std::multimap< Concrete::Object*, Concrete::Data::Link* >;
        LinkMap      linkMap;
        {
            for( const auto& [ _, pConcreteType ] : pSymbolTable->get_concrete_type_ids() )
            {
                if( pConcreteType->get_context().has_value() )
                {
                    Concrete::Context* pContext = pConcreteType->get_context().value();
                    if( Concrete::Object* pObject = db_cast< Concrete::Object >( pContext ) )
                    {
                        objectVector.push_back( pObject );
                    }
                    else if( Concrete::Data::Link* pLink = db_cast< Concrete::Data::Link >( pContext ) )
                    {
                        auto obj = pLink->get_concrete_object();
                        if( obj.has_value() )
                        {
                            linkMap.insert( { obj.value(), pLink } );
                        }
                    }
                }
                else if( pConcreteType->get_dim_user().has_value() )
                {
                    Concrete::Dimensions::User* pDim = pConcreteType->get_dim_user().value();
                    auto                        obj  = pDim->get_parent()->get_concrete_object();
                    if( obj.has_value() )
                    {
                        dimMap.insert( { obj.value(), pDim } );
                    }
                }
            }
        }*/

        // actually determine bindings...
        for( Concrete::Object* pObject : objectVector )
        {
            UnityAnalysis::ObjectBinding* pObjectBinding = nullptr;
            for( Interface::IContext* pInherited : pObject->get_inheritance() )
            {
                auto iFind = bindings.find( pInherited->get_interface_id() );
                if( iFind != bindings.end() )
                {
                    pObjectBinding = iFind->second;
                    break;
                }
            }
            if( pObjectBinding )
            {
                std::string strName;
                {
                    UnityAnalysis::Prefab* pPrefab = db_cast< UnityAnalysis::Prefab >( pObjectBinding );
                    UnityAnalysis::Manual* pManual = db_cast< UnityAnalysis::Manual >( pObjectBinding );
                    VERIFY_RTE( pPrefab || pManual );
                    strName = pPrefab ? pPrefab->get_guid() : pManual->get_name();
                }

                std::map< Concrete::Dimensions::User*, UnityAnalysis::DataBinding* > dataBindings;
                {
                    for( UnityAnalysis::DataBinding* pDataBinding : pObjectBinding->get_dataBindings() )
                    {
                        const TypeID interfaceTypeID = pDataBinding->get_interfaceTypeID();

                        Concrete::Dimensions::User* pFoundDimension = nullptr;
                        for( auto i = dimMap.lower_bound( pObject ), iEnd = dimMap.upper_bound( pObject ); i != iEnd;
                             ++i )
                        {
                            Concrete::Dimensions::User* pDim = i->second;
                            if( interfaceTypeID == pDim->get_interface_dimension()->get_interface_id() )
                            {
                                VERIFY_RTE_MSG( pFoundDimension == nullptr,
                                                "Found duplicate binding to dimension in object binding: "
                                                    << strName << " concrete type: " << pObject->get_concrete_id()
                                                    << " dimension interface type: " << interfaceTypeID );
                                pFoundDimension = pDim;
                            }
                        }
                        VERIFY_RTE_MSG( pFoundDimension,
                                        "Failed to locate concrete dimension in object binding: "
                                            << strName << " concrete type: " << pObject->get_concrete_id()
                                            << " dimension interface type: " << interfaceTypeID );
                        dataBindings.insert( { pFoundDimension, pDataBinding } );
                    }
                }
                // THROW_TODO;
                /*std::map< Concrete::Data::Link*, UnityAnalysis::LinkBinding* > linkBindings;
                {
                    for( UnityAnalysis::LinkBinding* pLinkBinding : pObjectBinding->get_linkBindings() )
                    {
                        const TypeID interfaceTypeID = pLinkBinding->get_interfaceTypeID();

                        Concrete::Data::Link* pFoundLink = nullptr;
                        for( auto i = linkMap.lower_bound( pObject ), iEnd = linkMap.upper_bound( pObject ); i != iEnd;
                             ++i )
                        {
                            Concrete::Data::Link* pLink = i->second;

                            bool bFound = false;
                            for( Interface::IContext* pInherited : pLink->get_inheritance() )
                            {
                                if( interfaceTypeID == pInherited->get_interface_id() )
                                {
                                    bFound = true;
                                    break;
                                }
                            }

                            if( bFound )
                            {
                                VERIFY_RTE_MSG( pFoundLink == nullptr,
                                                "Found duplicate binding to link in object binding: "
                                                    << strName << " concrete type: " << pObject->get_concrete_id()
                                                    << " link interface type: " << interfaceTypeID );
                                pFoundLink = pLink;
                            }
                        }
                        VERIFY_RTE_MSG( pFoundLink,
                                        "Failed to locate concrete link in object binding: "
                                            << strName << " concrete type: " << pObject->get_concrete_id()
                                            << " link interface type: " << interfaceTypeID );
                        linkBindings.insert( { pFoundLink, pLinkBinding } );
                    }
                }

                // have found highest priority binding!
                database.construct< UnityAnalysis::Binding >(
                    UnityAnalysis::Binding::Args{ pObjectBinding, pObject, dataBindings, linkBindings } );*/
            }
        }

        const task::FileHash fileHashCode = database.save_UnityAnalysis_to_temp();
        m_environment.setBuildHashCode( unityAnalysisCompilationFile, fileHashCode );
        m_environment.temp_to_real( unityAnalysisCompilationFile );
        m_environment.stash( unityAnalysisCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Unity( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_Unity >( taskArguments );
}

class Task_UnityDatabase : public BaseTask
{
public:
    Task_UnityDatabase( const TaskArguments& taskArguments )
        : BaseTask( taskArguments )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const boost::filesystem::path unityDatabaseFilePath = m_environment.UnityDatabase();
        const auto                    symbolTableCompilationFile
            = m_environment.SymbolAnalysis_SymbolTable( m_environment.project_manifest() );

        const mega::io::CompilationFilePath unityAnalysisCompilationFile
            = m_environment.UnityStage_UnityAnalysis( m_manifest );

        start( taskProgress, "Task_UnityDatabase", m_environment.project_manifest().path(),
               unityDatabaseFilePath.filename() );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash,
                                                   m_environment.getBuildHashCode( symbolTableCompilationFile ),
                                                   m_environment.getBuildHashCode( unityAnalysisCompilationFile ) } );

        if( m_environment.restorePath( unityDatabaseFilePath, determinant ) )
        {
            m_environment.setBuildHashCodePath( unityDatabaseFilePath );
            cached( taskProgress );
            return;
        }

        using namespace UnityStageView;
        Database database( m_environment, m_manifest );

        nlohmann::json data( { { "prefabBindings", nlohmann::json::array() },
                               { "manualBindings", nlohmann::json::array() },
                               { "relations", nlohmann::json::array() },
                               { "memoryMaps", nlohmann::json::array() } } );

        std::unordered_map< std::string, MegaMangle::Mangle* > mangleMap;
        for( auto pMangle : database.many< MegaMangle::Mangle >( m_environment.project_manifest() ) )
        {
            mangleMap.insert( { pMangle->get_canon(), pMangle } );
        }

        for( MemoryLayout::MemoryMap* pMemoryMap :
             database.many< MemoryLayout::MemoryMap >( m_environment.project_manifest() ) )
        {
            nlohmann::json memoryMap(
                { { "blockSize", pMemoryMap->get_block_size() },
                  { "allocation", pMemoryMap->get_fixed_allocation() },
                  { "alignment", pMemoryMap->get_block_alignment() },
                  { "interfaceTypeID", pMemoryMap->get_interface()->get_interface_id().getSymbolID() },
                  { "concrete", nlohmann::json::array() } } );

            for( Concrete::Object* pObject : pMemoryMap->get_concrete() )
            {
                nlohmann::json concrete( { { "concrete_type_id", pObject->get_concrete_id().getSymbolID() },
                                           { "dimensions", nlohmann::json::array() } } );

                for( auto pBuffer : pObject->get_buffers() )
                {
                    for( auto pPart : pBuffer->get_parts() )
                    {
                        for( auto pDim : pPart->get_user_dimensions() )
                        {
                            const std::string strCanonical = pDim->get_interface_dimension()->get_canonical_type();
                            auto              iFind        = mangleMap.find( strCanonical );
                            VERIFY_RTE_MSG( iFind != mangleMap.end(),
                                            "Failed to locate mega mangle for canonical type: "
                                                << strCanonical << " of dimension: " << pDim->get_concrete_id() );

                            nlohmann::json dimension( { { "concrete_type_id", pDim->get_concrete_id().getSymbolID() },
                                                        { "offset", pDim->get_offset() },
                                                        { "blit", iFind->second->get_blit() } } );
                            concrete[ "dimensions" ].push_back( dimension );
                        }
                    }
                }

                memoryMap[ "concrete" ].push_back( concrete );
            }
            data[ "memoryMaps" ].push_back( memoryMap );
        }

        for( UnityAnalysis::Binding* pBinding :
             database.many< UnityAnalysis::Binding >( m_environment.project_manifest() ) )
        {
            UnityAnalysis::ObjectBinding* pObjectBinding = pBinding->get_binding();

            if( UnityAnalysis::Prefab* pPrefab = db_cast< UnityAnalysis::Prefab >( pObjectBinding ) )
            {
                nlohmann::json binding( { { "guid", pPrefab->get_guid() },
                                          { "concreteTypeID", pBinding->get_object()->get_concrete_id().getSymbolID() },
                                          { "interfaceTypeID", pPrefab->get_interfaceTypeID().getSymbolID() },
                                          { "interfaceTypeName", pPrefab->get_typeName() },
                                          { "data", nlohmann::json::array() },
                                          { "links", nlohmann::json::array() } } );

                for( const auto& [ pDim, pBinding ] : pBinding->get_dataBindings() )
                {
                    nlohmann::json dataBinding( { { "concreteTypeID", pDim->get_concrete_id().getSymbolID() },
                                                  { "interfaceTypeID", pBinding->get_interfaceTypeID().getSymbolID() },
                                                  { "interfaceTypeName", pBinding->get_typeName() } } );
                    binding[ "data" ].push_back( dataBinding );
                }
                // THROW_TODO;
                /*for( const auto& [ pLink, pBinding ] : pBinding->get_linkBindings() )
                {
                    nlohmann::json dataBinding( { { "concreteTypeID", pLink->get_concrete_id().getSymbolID() },
                                                  { "interfaceTypeID", pBinding->get_interfaceTypeID().getSymbolID() },
                                                  { "interfaceTypeName", pBinding->get_typeName() } } );
                    binding[ "links" ].push_back( dataBinding );
                }*/

                data[ "prefabBindings" ].push_back( binding );
            }
            else if( UnityAnalysis::Manual* pManual = db_cast< UnityAnalysis::Manual >( pObjectBinding ) )
            {
                nlohmann::json binding( { { "name", pManual->get_name() },
                                          { "concreteTypeID", pBinding->get_object()->get_concrete_id().getSymbolID() },
                                          { "interfaceTypeID", pManual->get_interfaceTypeID().getSymbolID() },
                                          { "interfaceTypeName", pManual->get_typeName() },
                                          { "data", nlohmann::json::array() },
                                          { "links", nlohmann::json::array() } } );

                for( const auto& [ pDim, pBinding ] : pBinding->get_dataBindings() )
                {
                    nlohmann::json dataBinding( { { "concreteTypeID", pDim->get_concrete_id().getSymbolID() },
                                                  { "interfaceTypeID", pBinding->get_interfaceTypeID().getSymbolID() },
                                                  { "interfaceTypeName", pBinding->get_typeName() } } );
                    binding[ "data" ].push_back( dataBinding );
                }

                // THROW_TODO;
                /*for( const auto& [ pLink, pBinding ] : pBinding->get_linkBindings() )
                {
                    nlohmann::json dataBinding( { { "concreteTypeID", pLink->get_concrete_id().getSymbolID() },
                                                  { "interfaceTypeID", pBinding->get_interfaceTypeID().getSymbolID() },
                                                  { "interfaceTypeName", pBinding->get_typeName() } } );
                    binding[ "links" ].push_back( dataBinding );
                }*/

                data[ "manualBindings" ].push_back( binding );
            }
            else
            {
                THROW_RTE( "Unknown binding type" );
            }
        }

        // THROW_TODO;
        /*for( HyperGraph::Relation* pRelation :
             database.many< HyperGraph::Relation >( m_environment.project_manifest() ) )
        {
            bool bSourceIsRoot = false;
            bool bTargetIsRoot = false;
            {
                auto lambda = []( std::vector< Interface::Link* > links, RelationID id ) -> bool
                {
                    int iFoundRoot  = 0;
                    int iFoundOther = 0;
                    for( auto pLink : links )
                    {
                        for( auto pConcreteLink : pLink->get_concrete() )
                        {
                            auto pObjectOpt = pConcreteLink->get_concrete_object();
                            if( pObjectOpt.has_value() )
                            {
                                if( pObjectOpt.value()->get_concrete_id() == ROOT_TYPE_ID )
                                {
                                    ++iFoundRoot;
                                    continue;
                                }
                            }
                            ++iFoundOther;
                        }
                    }
                    VERIFY_RTE_MSG( ( iFoundOther == 0 ) || ( iFoundRoot == 0 ),
                                    "Relation to Root also used in other concrete types: " << id );
                    return iFoundRoot != 0;
                };
                bSourceIsRoot = lambda( pRelation->get_sources(), pRelation->get_id() );
                bTargetIsRoot = lambda( pRelation->get_targets(), pRelation->get_id() );
            }

            VERIFY_RTE_MSG(
                !( bSourceIsRoot && bTargetIsRoot ), "Relation is from Root to Root: " << pRelation->get_id() );

            bool bOwning = false;
            bool bSource = false;
            {
                switch( pRelation->get_ownership().get() )
                {
                    case Ownership::eOwnNothing:
                    {
                        if( bSourceIsRoot )
                        {
                            bSource = true;
                        }
                        else if( bTargetIsRoot )
                        {
                            bSource = false;
                        }
                        bOwning = false;
                    }
                    break;
                    case Ownership::eOwnSource:
                    {
                        VERIFY_RTE_MSG( !bSourceIsRoot, "Relation owns Root: " << pRelation->get_id() );
                        bSource = false;
                        bOwning = true;
                    }
                    break;
                    case Ownership::eOwnTarget:
                    {
                        VERIFY_RTE_MSG( !bTargetIsRoot, "Relation owns Root: " << pRelation->get_id() );
                        bSource = true;
                        bOwning = true;
                    }
                    break;
                    case Ownership::TOTAL_OWNERSHIP_MODES:
                        break;
                }
            }

            nlohmann::json relation( { { "relationID", pRelation->get_id().getID() },
                                       { "source_is_root", bSourceIsRoot },
                                       { "target_is_root", bTargetIsRoot },
                                       { "ownership", pRelation->get_ownership().str() } } );
            data[ "relations" ].push_back( relation );
        }*/

        writeJSON( unityDatabaseFilePath, data );
        m_environment.setBuildHashCodePath( unityDatabaseFilePath );
        m_environment.stash( unityAnalysisCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_UnityDatabase( const TaskArguments& taskArguments )
{
    return std::make_unique< Task_UnityDatabase >( taskArguments );
}
} // namespace mega::compiler
