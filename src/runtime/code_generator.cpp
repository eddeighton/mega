
#include "code_generator.hpp"
#include "database/common/environment_archive.hpp"

#include "common/file.hpp"
#include "common/string.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include "boost/process.hpp"

#include <string>
#include <istream>
#include <sstream>

namespace mega
{
namespace runtime
{
class CodeGenerator::Pimpl
{
    const mega::network::MegastructureInstallation m_megastructureInstallation;
    ::inja::Environment                            m_injaEnvironment;
    ::inja::Template                               m_allocateTemplate;
    ::inja::Template                               m_readTemplate;

public:
    Pimpl( const mega::network::MegastructureInstallation& megastructureInstallation )
        : m_megastructureInstallation( megastructureInstallation )
    {
        m_injaEnvironment.set_trim_blocks( true );

        m_allocateTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateAllocate().native() );
        m_readTemplate
            = m_injaEnvironment.parse_template( m_megastructureInstallation.getRuntimeTemplateRead().native() );
    }

    void render_allocate( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_allocateTemplate, data );
    }
    void render_read( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_readTemplate, data );
    }
};

CodeGenerator::CodeGenerator( const mega::network::MegastructureInstallation& megastructureInstallation,
                              const mega::network::Project&                   project )
    : m_pPimpl( std::make_shared< Pimpl >( megastructureInstallation ) )
    , m_tempDir( project.getProjectTempDir() )
    , m_clangPath( megastructureInstallation.getClangPath() )
{
    if ( !boost::filesystem::exists( m_tempDir ) )
    {
        boost::filesystem::create_directories( m_tempDir );
    }
}

void CodeGenerator::compileToIR( const boost::filesystem::path& sourcePath,
                                 const boost::filesystem::path& irPath ) const
{
    std::ostringstream osCmd, osOutput, osError;
    osCmd << m_clangPath << " -S -emit-llvm -o " << irPath.native() << " -c " << sourcePath.native();

    int iCompilationResult = 0;
    {
        namespace bp = boost::process;

        bp::ipstream errStream, outStream; // reading pipe-stream
        bp::child    c( osCmd.str(), bp::std_out > outStream, bp::std_err > errStream );

        std::string strOutputLine;
        while ( c.running() && std::getline( outStream, strOutputLine ) )
        {
            if ( !strOutputLine.empty() )
            {
                osOutput << "\nOUT    : " << strOutputLine;
            }
        }

        c.wait();
        iCompilationResult = c.exit_code();
        if ( iCompilationResult )
        {
            osError << common::COLOUR_RED_BEGIN << "FAILED : " << sourcePath.string();
            while ( errStream && std::getline( errStream, strOutputLine ) )
            {
                if ( !strOutputLine.empty() )
                {
                    osError << "\nERROR  : " << strOutputLine;
                }
            }
            osError << common::COLOUR_END;
        }
    }
    VERIFY_RTE_MSG( iCompilationResult == 0, "Error compilation invocation: " << osError.str() );
}

void CodeGenerator::generate_allocate( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                       std::ostream& os )
{
    // const FinalStage::Operations::Invocation* pInvocation = database.getInvocation( invocation );
    // using namespace FinalStage::Invocations;
    // Variables::Context* pRootVariable = pInvocation->get_root_variable();
    // pInvocation->get_root_instruction()

    std::ostringstream osName;
    osName << invocationID;

    nlohmann::json data( { { "name", osName.str() } } );

    const std::string uuid = common::uuid();

    using namespace std;

    const boost::filesystem::path sourcePath = m_tempDir / ( uuid + ".cpp"s );
    {
        auto outStream = boost::filesystem::createNewFileStream( sourcePath );
        m_pPimpl->render_allocate( data, *outStream );
    }

    const boost::filesystem::path irPath = m_tempDir / ( uuid + ".ir"s );
    compileToIR( sourcePath, irPath );

    boost::filesystem::loadAsciiFile( irPath, os );
}

void CodeGenerator::generate_read( const DatabaseInstance& database, const mega::InvocationID& invocationID,
                                   std::ostream& os )
{
    // const FinalStage::Operations::Invocation* pInvocation = database.getInvocation( invocation );
    // using namespace FinalStage::Invocations;
    // Variables::Context* pRootVariable = pInvocation->get_root_variable();
    // pInvocation->get_root_instruction()

    std::ostringstream osName;
    osName << invocationID;

    nlohmann::json data( { { "name", osName.str() } } );

    const std::string uuid = common::uuid();

    using namespace std;

    const boost::filesystem::path sourcePath = m_tempDir / ( uuid + ".cpp"s );
    {
        auto outStream = boost::filesystem::createNewFileStream( sourcePath );
        m_pPimpl->render_read( data, *outStream );
    }

    const boost::filesystem::path irPath = m_tempDir / ( uuid + ".ir"s );
    compileToIR( sourcePath, irPath );

    boost::filesystem::loadAsciiFile( irPath, os );
}
} // namespace runtime
} // namespace mega
