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

#include "database/types/clang_compilation.hpp"

#include "database/model/ConcreteStage.hxx"
#include "database/model/OperationsStage.hxx"
#include "database/model/FinalStage.hxx"

#include "database/types/component_type.hpp"
#include "database/types/sources.hpp"

#include "utilities/clang_format.hpp"

#include "common/file.hpp"
#include <common/stash.hpp>

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <boost/filesystem/operations.hpp>

namespace mega::compiler
{

class Task_Initialiser : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

    class TemplateEngine
    {
        const mega::io::StashEnvironment& m_environment;
        ::inja::Environment&              m_injaEnvironment;
        ::inja::Template                  m_initialiserTemplate;

    public:
        TemplateEngine( const mega::io::StashEnvironment& buildEnvironment, ::inja::Environment& injaEnv )
            : m_environment( buildEnvironment )
            , m_injaEnvironment( injaEnv )
            , m_initialiserTemplate( m_injaEnvironment.parse_template( m_environment.InitialiserTemplate().string() ) )
        {
        }

        void renderOperations( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_initialiserTemplate, data );
        }
    };

public:
    Task_Initialiser( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::GeneratedHPPSourceFilePath includeFilePath    = m_environment.Include( m_sourceFilePath );
        const mega::io::GeneratedCPPSourceFilePath initialiserCPPFile = m_environment.Initialiser( m_sourceFilePath );
        start( taskProgress, "Task_Initialiser", m_sourceFilePath.path(), initialiserCPPFile.path() );

        task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.InitialiserTemplate(),
              m_environment.getBuildHashCode( includeFilePath ),
              m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) ),
              m_environment.getBuildHashCode(
                  m_environment.ConcreteTypeRollout_PerSourceConcreteTable( m_sourceFilePath ) ) } );

        if( m_environment.restore( initialiserCPPFile, determinant ) )
        {
            m_environment.setBuildHashCode( initialiserCPPFile );
            cached( taskProgress );
            return;
        }

        {
            using namespace ConcreteStage;
            using namespace ConcreteStage::Interface;

            Database database( m_environment, m_sourceFilePath );

            std::ostringstream os;
            {
                ::inja::Environment injaEnvironment;
                {
                    injaEnvironment.set_trim_blocks( true );
                }
                TemplateEngine templateEngine( m_environment, injaEnvironment );

                std::string strFileID = m_sourceFilePath.path().string();
                {
                    boost::replace_all( strFileID, "/", "_" );
                    boost::replace_all( strFileID, ".", "_" );
                }

                nlohmann::json data( { { "initialisers", nlohmann::json::array() },
                                       { "name", strFileID },
                                       { "include", m_environment.FilePath( includeFilePath ).string() } } );

                // Interface::Root*      pRoot = database.one< Interface::Root >( m_sourceFilePath );
                for( Interface::DimensionTrait* pDim : database.many< Interface::DimensionTrait >( m_sourceFilePath ) )
                {
                    if( auto pUserDimensionTrait = db_cast< Interface::UserDimensionTrait >( pDim ) )
                    {
                        if( auto pInitOpt = pUserDimensionTrait->get_parser_dimension()->get_initialiser();
                            pInitOpt.has_value() )
                        {
                            auto pInit = pInitOpt.value();

                            nlohmann::json initialiser(
                                { { "expression", pInit->get_initialiser() },
                                  { "type", pDim->get_canonical_type() },
                                  { "objectID", pDim->get_interface_id().getObjectID() },
                                  { "subObjectID", pDim->get_interface_id().getSubObjectID() } } );
                            data[ "initialisers" ].push_back( initialiser );
                        }
                    }
                }

                templateEngine.renderOperations( data, os );
            }
            std::string strOperations = os.str();
            mega::utilities::clang_format( strOperations, std::optional< boost::filesystem::path >() );
            boost::filesystem::updateFileIfChanged( m_environment.FilePath( initialiserCPPFile ), strOperations );
        }

        m_environment.setBuildHashCode( initialiserCPPFile );
        m_environment.stash( initialiserCPPFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Initialiser( const TaskArguments&          taskArguments,
                                       const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Initialiser >( taskArguments, sourceFilePath );
}

class Task_InitialiserObject : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_InitialiserObject( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;
        using namespace FinalStage::Interface;

        const mega::io::GeneratedCPPSourceFilePath implementationFile = m_environment.Initialiser( m_sourceFilePath );

        const mega::io::ObjectFilePath initialiserObjFile = m_environment.InitialiserObj( m_sourceFilePath );

        start( taskProgress, "Task_InitialiserObject", m_sourceFilePath.path(), initialiserObjFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( implementationFile ) } );

        if( m_environment.restore( initialiserObjFile, determinant ) )
        {
            m_environment.setBuildHashCode( initialiserObjFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_environment.project_manifest() );

        const mega::Compilation compilationCMD = mega::Compilation::make_initialiser_obj_compilation(
            m_environment,
            m_toolChain,
            getComponent< Components::Component >( database, m_sourceFilePath ),
            m_sourceFilePath );

        if( run_cmd( taskProgress, compilationCMD.generateCompilationCMD() ) )
        {
            failed( taskProgress );
            return;
        }

        if( m_environment.exists( initialiserObjFile ) )
        {
            m_environment.setBuildHashCode( initialiserObjFile );
            m_environment.stash( initialiserObjFile, determinant );
            succeeded( taskProgress );
        }
        else
        {
            failed( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_InitialiserObject( const TaskArguments&          taskArguments,
                                             const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_InitialiserObject >( taskArguments, sourceFilePath );
}
} // namespace mega::compiler
