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

#include <gtest/gtest.h>

#include "preprocessor/preprocessor.hpp"

#include "common/file.hpp"

#include <boost/filesystem.hpp>

#include "fixture.hpp"


namespace
{
struct Data
{
    std::string                testName;
    std::string                sourceFile;
    std::vector< std::string > includeFiles;
};

std::ostream& operator<<( std::ostream& os, const Data& testData )
{
    return os;
}

using BasicFixtureType = PreprocessorTestFixture< Data >;
} // namespace

TEST_P( BasicFixtureType, BasicParameterizedTest )
{
    const Data data = GetParam();

    try
    {
        auto inputFilePath = g_inputDir / data.sourceFile;
        ASSERT_TRUE( boost::filesystem::exists( inputFilePath ) )
            << "Failed to locate input file: " << inputFilePath.string();

        mega::preprocessor::Preprocess settings{ inputFilePath, {}, "" };

        std::ostringstream os;
        mega::preprocessor::preprocess( settings, os );

        std::cout << os.str() << std::endl;
    }
    catch( std::exception& ex )
    {
        FAIL() << ex.what();
    }
}

using namespace std::string_literals;

// clang-format off
INSTANTIATE_TEST_SUITE_P( Basic, BasicFixtureType,
        ::testing::Values
        ( 
            Data{ "basic"s, "test.cpp"s, {} }
        ));
// clang-format on
