
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

#include <iostream>

namespace
{
struct Data
{
    std::string filename;
    std::string symbolReport, interfaceReport;
};

std::ostream& operator<<( std::ostream& os, const Data&  )
{
    return os;
}

using SymbolsFixtureType = CompilerTestFixture< Data >;
} // namespace

TEST_P( SymbolsFixtureType, SymbolsTest )
{
    const Data data = GetParam();

    try
    {
        std::ostringstream osFileName;
        osFileName << data.filename << ".mega";
        auto pCompilation = createBuildAndRun( osFileName.str(), data.filename.c_str(), "Task_SymbolRollout" );

        mega::pipeline::PipelineResult result = pCompilation->runPipeline( osFileName.str() );
        ASSERT_TRUE( result.m_bSuccess );

        pCompilation->generateReport( "/?report=Symbols", data.symbolReport );
        pCompilation->generateReport( "/?report=InterfaceTypeID", data.interfaceReport );
    }
    catch( std::exception& ex )
    {
        FAIL() << ex.what();
    }
}

using namespace std::string_literals;

// clang-format off
INSTANTIATE_TEST_SUITE_P( Symbols, SymbolsFixtureType,
        ::testing::Values
        ( 
            Data{ "symbols_basic"s, "symbols"s, "interface"s }
        ));
// clang-format on
