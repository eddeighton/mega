
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
} // namespace

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
        const mega::io::CompilationFilePath unityAnalysisCompilationFile
            = m_environment.UnityStage_UnityAnalysis( m_manifest );

        start( taskProgress, "Task_Unity", m_manifest.path(), unityAnalysisCompilationFile.path() );

        // const task::FileHash previousStageHash = m_environment.getBuildHashCode(
        //    m_environment.OperationsStage_Operations() ( m_schematicFilePath ) );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash } );

        if( m_environment.restore( unityAnalysisCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( unityAnalysisCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace UnityStage;
        Database database( m_environment, m_manifest );

        // generate unity reflection data file
        const boost::filesystem::path unityDataFilePath = m_environment.UnityReflection();
        {
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
                        nlohmann::json typeInfo{ { "symbol", id.getSymbolID() }, { "name", osFullTypeName.str() } };
                        data[ "dimensions" ].push_back( typeInfo );
                    }
                    else
                    {
                        THROW_RTE( "Unknown interface type" );
                    }
                }
            }

            if( writeJSON( unityDataFilePath, data ) )
            {
                std::stringstream os;
                os << "Updated unity data file: " << unityDataFilePath.string();
                msg( taskProgress, os.str() );
            }
        }

        VERIFY_RTE_MSG( boost::filesystem::exists( m_unityProjectDir ),
                        "Could not locate unity project directory at: " << m_unityProjectDir.string() );

        VERIFY_RTE_MSG( boost::filesystem::exists( m_unityEditor ),
                        "Could not locate unity editor at: " << m_unityEditor.string() );

        const boost::filesystem::path unityLog = m_environment.buildDir() / "unity_log.txt";

        {
            std::ostringstream osCmd;

            osCmd << m_unityEditor.string() << " ";
            osCmd << "-projectPath " << m_unityProjectDir.string() << " ";
            osCmd << "-logFile " << unityLog.string() << " ";
            osCmd << "-noUpm -batchmode  -quit -nographics -disable-gpu-skinning -disable-assembly-updater "
                     "-disableManagedDebugger ";
            osCmd << "-executeMethod UnityProtocol.RunFromCmdLine";
            osCmd << "-reflectionData " << unityDataFilePath.string();

            this->run_cmd( taskProgress, osCmd.str() );
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
