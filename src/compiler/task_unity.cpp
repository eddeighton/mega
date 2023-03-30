
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

#include "database/model/UnityStage.hxx"
#include "database/types/component_type.hpp"
#include "database/types/sources.hpp"

#include "nlohmann/json.hpp"

#include <common/stash.hpp>
#include <common/file.hpp>

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

void printIContextFullType( UnityStage::Interface::IContext* pContext, std::ostream& os )
{
    using namespace UnityStage;
    using IContextVector = std::vector< Interface::IContext* >;
    IContextVector path;
    while( pContext )
    {
        path.push_back( pContext );
        pContext = db_cast< Interface::IContext >( pContext->get_parent() );
    }
    std::reverse( path.begin(), path.end() );
    for( auto i = path.begin(), iNext = path.begin(), iEnd = path.end(); i != iEnd; ++i )
    {
        ++iNext;
        if( iNext == iEnd )
        {
            os << ( *i )->get_identifier();
        }
        else
        {
            os << ( *i )->get_identifier() << ".";
        }
    }
}

void printDimensionTraitFullType( UnityStage::Interface::DimensionTrait* pDim, std::ostream& os )
{
    using namespace UnityStage;
    Interface::IContext* pParent = db_cast< Interface::IContext >( pDim->get_parent() );
    VERIFY_RTE( pParent );
    printIContextFullType( pParent, os );
    os << "." << pDim->get_id()->get_str();
}

bool generateUnityReflectionJSON( UnityStage::Database& database, const io::StashEnvironment& m_environment,
                                  const boost::filesystem::path& unityDataFilePath )
{
    using namespace UnityStage;

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
        Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

        for( const auto& [ id, pInterfaceType ] : pSymbolTable->get_interface_type_ids() )
        {
            std::ostringstream osFullTypeName;
            if( pInterfaceType->get_context().has_value() )
            {
                Interface::IContext* pContext = pInterfaceType->get_context().value();

                if( Interface::Object* pObject = db_cast< Interface::Object >( pContext ) )
                {
                    printIContextFullType( pContext, osFullTypeName );
                    nlohmann::json typeInfo{ { "symbol", id.getSymbolID() }, { "name", osFullTypeName.str() } };
                    data[ "objects" ].push_back( typeInfo );
                }
                else if( Interface::Link* pLink = db_cast< Interface::Link >( pContext ) )
                {
                    printIContextFullType( pContext, osFullTypeName );
                    nlohmann::json typeInfo{ { "symbol", id.getSymbolID() }, { "name", osFullTypeName.str() } };
                    data[ "links" ].push_back( typeInfo );
                }
            }
            else if( pInterfaceType->get_dimension().has_value() )
            {
                Interface::DimensionTrait* pDimension = pInterfaceType->get_dimension().value();
                printDimensionTraitFullType( pDimension, osFullTypeName );

                std::string    strBlit = "TODO";
                nlohmann::json typeInfo{
                    { "symbol", id.getSymbolID() }, { "name", osFullTypeName.str() }, { "blit", strBlit } };
                data[ "dimensions" ].push_back( typeInfo );
            }
            else
            {
                THROW_RTE( "Unknown interface type" );
            }
        }
    }

    return writeJSON( unityDataFilePath, data );
}
} // namespace

class Task_UnityReflection : public BaseTask
{
    const mega::io::manifestFilePath& m_manifest;

public:
    Task_UnityReflection( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifest )
        : BaseTask( taskArguments )
        , m_manifest( manifest )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const boost::filesystem::path unityDataFilePath = m_environment.UnityReflection();

        const auto symbolTableCompilationFile
            = m_environment.SymbolAnalysis_SymbolTable( m_environment.project_manifest() );

        start( taskProgress, "Task_UnityReflection", symbolTableCompilationFile.path(), unityDataFilePath );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( symbolTableCompilationFile ) } );

        if( m_environment.restorePath( unityDataFilePath, determinant ) )
        {
            m_environment.setBuildHashCodePath( unityDataFilePath );
            cached( taskProgress );
            return;
        }

        bool bModified = false;
        {
            using namespace UnityStage;
            Database database( m_environment, m_manifest );
            bModified = generateUnityReflectionJSON( database, m_environment, unityDataFilePath );
        }

        m_environment.setBuildHashCodePath( unityDataFilePath );
        m_environment.stashPath( unityDataFilePath, determinant );

        if( bModified )
        {
            succeeded( taskProgress );
        }
        else
        {
            cached( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_UnityReflection( const TaskArguments&              taskArguments,
                                           const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_UnityReflection >( taskArguments, manifestFilePath );
}

class Task_UnityAnalysis : public BaseTask
{
    const mega::io::manifestFilePath& m_manifest;

public:
    Task_UnityAnalysis( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifest )
        : BaseTask( taskArguments )
        , m_manifest( manifest )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const boost::filesystem::path unityDataFilePath     = m_environment.UnityReflection();
        const boost::filesystem::path unityAnalysisFilePath = m_environment.UnityAnalysis();

        start( taskProgress, "Task_UnityAnalysis", unityDataFilePath, unityAnalysisFilePath );

        // the unityAnalysisFilePath file can have changed externally due to editting IN unity.
        // so DO NOT want to replace it via the stash.
        std::optional< task::FileHash > optFileHash;
        if( boost::filesystem::exists( unityAnalysisFilePath ) )
        {
            optFileHash = task::FileHash{ unityAnalysisFilePath };
        }

        // so IF the analysis file EXISTS then INCLUDE it in the determinant
        // so that way if it has changed it will rerun.
        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCodePath( unityDataFilePath ),
              ( optFileHash.has_value() ? optFileHash.value().get() : std::size_t{} ) } );

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
            osCmd << "-noUpm -batchmode  -quit -nographics -disable-gpu-skinning -disable-assembly-updater "
                     "-disableManagedDebugger ";
            osCmd << "-executeMethod UnityProtocol.RunFromCmdLine";
            osCmd << "-reflectionData " << unityDataFilePath.string();
            osCmd << "-unityData " << unityAnalysisFilePath.string();

            this->run_cmd( taskProgress, osCmd.str() );
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

BaseTask::Ptr create_Task_UnityAnalysis( const TaskArguments&              taskArguments,
                                         const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_UnityAnalysis >( taskArguments, manifestFilePath );
}

class Task_Unity : public BaseTask
{
    const mega::io::manifestFilePath& m_manifest;

public:
    Task_Unity( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifest )
        : BaseTask( taskArguments )
        , m_manifest( manifest )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const boost::filesystem::path unityAnalysisFilePath = m_environment.UnityAnalysis();
        const auto                    symbolTableCompilationFile
            = m_environment.SymbolAnalysis_SymbolTable( m_environment.project_manifest() );

        const mega::io::CompilationFilePath unityAnalysisCompilationFile
            = m_environment.UnityStage_UnityAnalysis( m_manifest );

        start( taskProgress, "Task_Unity", unityAnalysisFilePath, unityAnalysisCompilationFile.path() );

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

        // load the unityAnalysisFilePath
        nlohmann::json data;
        {
            auto pFile = boost::filesystem::loadFileStream( unityAnalysisFilePath );
            data       = nlohmann::json::parse( *pFile );
        }

        for( const auto& prefab : data[ "prefabs" ] )
        {
            const std::string  guid     = prefab[ "guid" ];
            const std::string  typeName = prefab[ "typeName" ];
            const mega::TypeID interfaceTypeID{ TypeID::ValueType{ prefab[ "interfaceTypeID" ] } };

            for( const auto& dataBinding : prefab[ "dataBindings" ] )
            {
                const std::string  typeName = dataBinding[ "typeName" ];
                const mega::TypeID interfaceTypeID{ TypeID::ValueType{ dataBinding[ "interfaceTypeID" ] } };
            }
            for( const auto& linkBinding : prefab[ "linkBindings" ] )
            {
                const std::string  typeName = linkBinding[ "typeName" ];
                const mega::TypeID interfaceTypeID{ TypeID::ValueType{ linkBinding[ "interfaceTypeID" ] } };
            }
        }

        Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

        for( const auto& [ id, pInterfaceType ] : pSymbolTable->get_interface_type_ids() )
        {
            if( pInterfaceType->get_context().has_value() )
            {
                Interface::IContext* pContext = pInterfaceType->get_context().value();

                if( Interface::Object* pObject = db_cast< Interface::Object >( pContext ) )
                {
                }
                else if( Interface::Link* pLink = db_cast< Interface::Link >( pContext ) )
                {
                }
            }
            else if( pInterfaceType->get_dimension().has_value() )
            {
            }
            else
            {
                THROW_RTE( "Unknown interface type" );
            }
        }

        const task::FileHash fileHashCode = database.save_UnityAnalysis_to_temp();
        m_environment.setBuildHashCode( unityAnalysisCompilationFile, fileHashCode );
        m_environment.temp_to_real( unityAnalysisCompilationFile );
        m_environment.stash( unityAnalysisCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Unity( const TaskArguments&              taskArguments,
                                 const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_Unity >( taskArguments, manifestFilePath );
}

} // namespace mega::compiler
