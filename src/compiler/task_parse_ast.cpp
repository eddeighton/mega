
#include "base_task.hpp"

#include "database/model/ParserStage.hxx"

#include "parser/parser.hpp"

namespace mega
{
namespace compiler
{

class Task_ParseAST : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;
    using FileRootMap = std::map< boost::filesystem::path, ParserStage::Parser::SourceRoot* >;
    FileRootMap m_rootFiles;

public:
    Task_ParseAST( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    ParserStage::Parser::ContextDef* parseInputFile( ParserStage::Database&                    database,
                                                     const ParserStage::Components::Component* pComponent,
                                                     const mega::io::megaFilePath&             sourceFilePath,
                                                     std::ostream&                             osError,
                                                     std::ostream&                             osWarn )
    {
        
        ParserStage::Parser::ContextDef* pContextDef
            = m_parser->parseEGSourceFile( database, m_environment.FilePath( sourceFilePath ),
                                                   pComponent->get_include_directories(), osError, osWarn );
        return pContextDef;
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        // ParserStage::Parser::Root
        const mega::io::CompilationFilePath astFile  = m_environment.ParserStage_AST( m_sourceFilePath );
        const mega::io::CompilationFilePath bodyFile = m_environment.ParserStage_Body( m_sourceFilePath );

        start( taskProgress, "Task_ParseAST", m_sourceFilePath.path(), astFile.path() );

        using namespace ParserStage;
        Database database( m_environment, m_sourceFilePath );

        std::ostringstream osError, osWarn;

        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        using namespace ParserStage::Parser;

        ContextDef* pContextDef = parseInputFile( database, pComponent, m_sourceFilePath, osError, osWarn );

        ObjectSourceRoot* pObjectSrcRoot = database.construct< ObjectSourceRoot >(
            ObjectSourceRoot::Args{ SourceRoot::Args{ m_sourceFilePath.path(), pComponent, pContextDef } } );

        m_rootFiles.insert( std::make_pair( m_sourceFilePath.path(), pObjectSrcRoot ) );

        // greedy algorithm to parse transitive closure of include files
        {
            bool bExhaustedAll = false;
            while ( !bExhaustedAll )
            {
                bExhaustedAll = true;

                for ( MegaInclude* pInclude : database.many< MegaInclude >( m_sourceFilePath ) )
                {
                    FileRootMap::const_iterator iFind = m_rootFiles.find( pInclude->get_megaSourceFilePath() );
                    if ( iFind == m_rootFiles.end() )
                    {
                        ContextDef* pIncludeContextDef
                            = parseInputFile( database, pComponent,
                                              m_environment.megaFilePath_fromPath( pInclude->get_megaSourceFilePath() ),
                                              osError, osWarn );

                        IncludeRoot* pIncludeRoot
                            = database.construct< IncludeRoot >( { IncludeRoot::Args{ SourceRoot::Args{
                                pInclude->get_megaSourceFilePath(), pComponent, pIncludeContextDef } } } );

                        pInclude->set_root( pIncludeRoot );

                        m_rootFiles.insert( std::make_pair( pInclude->get_megaSourceFilePath(), pIncludeRoot ) );
                        bExhaustedAll = false;
                        break;
                    }
                    else
                    {
                        IncludeRoot* pIncludeRoot = dynamic_database_cast< IncludeRoot >( iFind->second );
                        VERIFY_RTE( pIncludeRoot );
                        pInclude->set_root( pIncludeRoot );
                    }
                }
            }
        }
        if ( !osError.str().empty() )
        {
            // Error
            msg( taskProgress, osError.str() );
            failed( taskProgress );
        }
        else
        {
            if ( !osWarn.str().empty() )
            {
                // Warning
                msg( taskProgress, osWarn.str() );
            }

            bool bRestored = false;
            {
                const task::FileHash astHashCode = database.save_AST_to_temp();
                m_environment.setBuildHashCode( astFile, astHashCode );

                /*{
                    std::ostringstream os;
                    os << "PARSER: " << astHashCode.toHexString();
                    msg( taskProgress, os.str() );
                }*/

                const task::DeterminantHash determinant( { m_toolChain.toolChainHash, astHashCode } );

                if ( !m_environment.restore( astFile, determinant ) )
                {
                    m_environment.temp_to_real( astFile );
                    m_environment.stash( astFile, determinant );
                    bRestored = false;
                }
            }
            {
                const task::FileHash bodyHashCode = database.save_Body_to_temp();
                m_environment.setBuildHashCode( bodyFile, bodyHashCode );

                const task::DeterminantHash determinant( { m_toolChain.toolChainHash, bodyHashCode } );

                if ( !m_environment.restore( bodyFile, determinant ) )
                {
                    m_environment.temp_to_real( bodyFile );
                    m_environment.stash( bodyFile, determinant );
                    bRestored = false;
                }
            }

            if ( bRestored )
            {
                cached( taskProgress );
            }
            else
            {
                succeeded( taskProgress );
            }
        }
    }
};

BaseTask::Ptr create_Task_ParseAST( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ParseAST >( taskArguments, sourceFilePath );
}

} // namespace compiler
} // namespace mega
