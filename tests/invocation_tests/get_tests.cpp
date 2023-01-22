
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

#include "invoke_fixture.hpp"

static constexpr const char Get_Name[] = "Get";

// clang-format off
static constexpr const char Get_Code[] =
R"TESTCODE(
object Root
{
    dim int m_x;
}
)TESTCODE";
// clang-format on

struct GetData
{
};

std::ostream& operator<<( std::ostream& os, const GetData& testData )
{
    return os;
}

using GetFixtureType = InvocationTestFixture< Get_Name, Get_Code, GetData >;
template <>
GetFixtureType::Impl::Ptr GetFixtureType::m_pImpl;

TEST_P( GetFixtureType, GetParameterizedTest )
{
    const GetData data = GetParam();

    // m_pImpl->m_environment.
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( Get, GetFixtureType,
        ::testing::Values
        ( 
            GetData{  }
        ));
// clang-format on
