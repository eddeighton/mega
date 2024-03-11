
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

#include <iostream>

namespace
{
struct Data
{
    std::string                testName;
    std::vector< std::string > sourceFiles;
    std::vector< std::string > includeFiles;
};

std::ostream& operator<<( std::ostream& os, const Data& testData )
{
    return os;
}

using ASTFixtureType = CompilerTestFixture< Data >;
} // namespace

TEST_P( ASTFixtureType, ASTParameterizedTest )
{
    const Data data = GetParam();

    try
    {
        auto pCompilation = createBuildAndRun( data.sourceFiles, data.includeFiles, data.testName.c_str(), "Task_AST" );

        mega::pipeline::PipelineResult result = pCompilation->runPipeline( "" );
        ASSERT_TRUE( result.m_bSuccess );

        pCompilation->generateReport( "/?report=AST", data.testName );
    }
    catch( std::exception& ex )
    {
        FAIL() << ex.what();
    }
}

using namespace std::string_literals;

// clang-format off
INSTANTIATE_TEST_SUITE_P( AST, ASTFixtureType,
        ::testing::Values
        ( 
            Data{ "ast_basic", { "ast_basic.mega" }, { "ast_include.mega" } }
        ));
// clang-format on
