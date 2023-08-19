
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

#include "compiler/automata.hpp"
/*
TEST( Automata, Basic )
{
    using namespace mega::automata;
    TokenVector tokens;

    const std::string str = "a maybe{ b }or{ c }";

    tokenise( str, tokens );

    ASSERT_FALSE( tokens.empty() );

    const TokenVector expected =
    {
        std::string_view{ str.cbegin(), str.cbegin() + 2 },
        eMaybe,
        std::string_view{ str.cbegin() + 8, str.cbegin() + 11 },
        eBraceClose,
        eOr,
        std::string_view{ str.cbegin() + 15, str.cbegin() + 18 },
        eBraceClose
    };

    ASSERT_EQ( expected, tokens );

}

TEST( Automata, PartialMatch )
{
    using namespace mega::automata;
    TokenVector tokens;

    const std::string str = "seq1 seq{ seq2 }";

    tokenise( str, tokens );

    ASSERT_FALSE( tokens.empty() );

    const TokenVector expected =
    {
        std::string_view{ str.cbegin(), str.cbegin() + 5 },
        eSeq,
        std::string_view{ str.cbegin() + 9, str.cbegin() + 15 },
        eBraceClose
    };

    ASSERT_EQ( expected, tokens );
}*/