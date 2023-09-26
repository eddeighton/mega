
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

#include "database/model/OperationsStage.hxx"

#include "invocation/invocation.hpp"

#include "mega/invocation_io.hpp"
#include "mega/common_strings.hpp"

static constexpr const char Basic_Name[] = "Basic";

// clang-format off
static constexpr const char Basic_Code[] =
R"TESTCODE(
object Root
{
    dim int m_z;
    dim int m_x;
    dim int m_y;
}
)TESTCODE";
// clang-format on

struct BasicData
{
    std::string                context;
    std::vector< std::string > typePath;
};

std::ostream& operator<<( std::ostream& os, const BasicData& testData )
{
    return os;
}

using BasicFixtureType = InvocationTestFixture< Basic_Name, Basic_Code, BasicData >;
template <>
BasicFixtureType::Impl::Ptr BasicFixtureType::m_pImpl;

TEST_P( BasicFixtureType, BasicParameterizedTest )
{
    const BasicData data = GetParam();

    OperationsStage::Database database( m_pImpl->m_environment, m_pImpl->m_megaSrcPath );

    using namespace OperationsStage;
    // using namespace OperationsStage::Interface;
    Symbols::SymbolTable* pSymbolTable
        = database.one< Symbols::SymbolTable >( m_pImpl->m_environment.project_manifest() );
    ASSERT_TRUE( pSymbolTable );
    mega::invocation::SymbolTables symbolTables( pSymbolTable );

    auto symbolNames = pSymbolTable->get_symbol_names();

    std::vector< mega::TypeID > contextSymbolIDs = { symbolNames[ data.context ]->get_id() };
    std::vector< mega::TypeID > typePathSymbolIDs;
    for( const auto& t : data.typePath )
    {
        typePathSymbolIDs.push_back( symbolNames[ t ]->get_id() );
    }
    mega::OperationID        operationTypeID = mega::id_Imp_NoParams;
    const mega::InvocationID id{ contextSymbolIDs, typePathSymbolIDs, operationTypeID };

    Operations::Invocation* pInvocation = mega::invocation::compileInvocation( database, symbolTables, id );
    ASSERT_TRUE( pInvocation );
}

using namespace std::string_literals;

// clang-format off
INSTANTIATE_TEST_SUITE_P( Basic, BasicFixtureType,
        ::testing::Values
        ( 
            BasicData{ "Root"s, { "m_x"s } },
            BasicData{ "Root"s, { "m_y"s } }
        ));
// clang-format on
