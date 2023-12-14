
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

#include "compiler_fixture.hpp"

#include "database_reporters/factory.hpp"
#include "reports/renderer_html.hpp"

#include <iostream>

struct ParserData
{
    std::string filename;
};

std::ostream& operator<<( std::ostream& os, const ParserData& testData )
{
    return os;
}

using ParserFixtureType = CompilerTestFixture< ParserData >;

TEST_P( ParserFixtureType, ParserParameterizedTest )
{
    const ParserData data = GetParam();

    try
    {
        std::ostringstream osFileName;
        osFileName << data.filename << ".mega";
        auto pCompilation = createBuildAndRun( osFileName.str(), data.filename.c_str(), "Task_ParseAST" );

        mega::pipeline::PipelineResult result = pCompilation->runPipeline( osFileName.str() );
        ASSERT_TRUE( result.m_bSuccess );

        mega::io::Manifest manifest( pCompilation->m_environment, pCompilation->m_environment.project_manifest() );

        std::ostringstream osOutFile;
        osOutFile << data.filename << ".html";
        const boost::filesystem::path resultFile = g_resultDir / osOutFile.str();
        {
            using namespace mega::reports;
            using namespace mega::reporters;

            const URL       url    = boost::urls::parse_origin_form( "/?report=Parser" ).value();
            const Container result = mega::reporters::generateCompilationReport(
                url, CompilationReportArgs{ manifest, pCompilation->m_environment } );
            VERIFY_RTE_MSG( !result.empty(), "Failed to generate any report for: " << url.c_str() );

            HTMLRenderer::JavascriptShortcuts shortcuts;
            HTMLRenderer                      renderer( g_report_templatesDir, shortcuts, true );

            std::ostringstream os;
            renderer.render( result, os );

            try
            {
                boost::filesystem::updateFileIfChanged( resultFile, os.str() );
            }
            catch( std::exception& ex )
            {
                THROW_RTE( "Error generating report: " << resultFile.string() << " exception: " << ex.what() );
            }
        }
    }
    catch( std::exception& ex )
    {
        FAIL() << ex.what();
    }
}

using namespace std::string_literals;

// clang-format off
INSTANTIATE_TEST_SUITE_P( Parser, ParserFixtureType,
        ::testing::Values
        ( 
            ParserData{ "parser_basic" }
        ));
// clang-format on
