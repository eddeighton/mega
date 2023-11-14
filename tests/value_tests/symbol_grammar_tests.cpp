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

#include "mega/values/compilation/symbol_grammar.hpp"

#include "common/grammar.hpp"

#include <gtest/gtest.h>
#include <gtest/internal/gtest-param-util.h>

using namespace std::string_literals;

struct PrintTestParam
{
    std::string operator()( const ::testing::TestParamInfo< std::string >& info ) const
    {
        return info.param;
    }
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Symbol
class SymbolGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( SymbolGrammarTest, Symbol )
{
    using namespace mega;
    const std::string data = GetParam();
    std::string strDataStripped;
    boost::spirit_ext::strip( data, strDataStripped );
    mega::Symbol s = mega::Symbol::parse( data );
    ASSERT_EQ( s.str(), strDataStripped );
    std::ostringstream os;
    os << s;
    ASSERT_EQ( os.str(), strDataStripped );
}

// clang-format off
static PrintTestParam printer1;
INSTANTIATE_TEST_SUITE_P( Symbol, SymbolGrammarTest,
        ::testing::Values
        (
            // default
            "a"s,
            "A"s,
            "1"s,
            "_"s,
            "Hello_World_01234"s,
            
            "  Hello_World_01234 "s,
            "\n\t\r Hello_World_01234\n\t\r "s,
            "/* comment */Hello_World_01234/* comment */"s,
            "// comment\nHello_World_01234// comment\n"s
        )
        );
// clang-format on


class SymbolRejectGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( SymbolRejectGrammarTest, Symbol )
{
    using namespace mega;
    const std::string data = GetParam();
    ASSERT_THROW( mega::Symbol::parse( data ), mega::SymbolParseException );
}

// clang-format off
// static PrintTestParam printer2; // cannot use printer with ASSERT_THROW???
INSTANTIATE_TEST_SUITE_P( Symbol, SymbolRejectGrammarTest,
        ::testing::Values
        (
            // default
            ""s,
            "!"s,
            "a a"s,
            "a\na"s,
            "a\ta"s,
            "a/*comment*/a"s,
            "\\"s,
            "/"s,
            "@HelloWorld_01234"s,
            "HelloWorld_01234@"s,
            "HelloWorld@_01234"s
        )
        );
// clang-format on


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// SymbolVariant
class SymbolVariantGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( SymbolVariantGrammarTest, SymbolVariant )
{
    using namespace mega;
    const std::string data = GetParam();
    mega::SymbolVariant s = mega::SymbolVariant::parse( data );
    std::string strDataStripped;
    boost::spirit_ext::strip( data, strDataStripped );
    ASSERT_EQ( s.str(), strDataStripped );
    std::ostringstream os;
    os << s;
    ASSERT_EQ( os.str(), strDataStripped );
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( SymbolVariant, SymbolVariantGrammarTest,
        ::testing::Values
        (
            // default
            "a"s,
            "A"s,
            "1"s,
            "_"s,
            "Hello_World_01234"s,

            "<a.a>"s,
            "<A.A.A>"s,
            "<1.2.3.4.5.6.7.8.9.0>"s,
            "<_._._._._>"s,
            "<Hello_World_01234.Hello_World_01234.Hello_World_01234>"s,

            "  Hello_World_01234"s,
            "\n\t\r Hello_World_01234"s,
            "// comment\nHello_World_01234"s
        )
        );
// clang-format on

class SymbolVariantRejectGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( SymbolVariantRejectGrammarTest, SymbolVariant )
{
    using namespace mega;
    const std::string data = GetParam();
    ASSERT_THROW( mega::SymbolVariant::parse( data ), mega::SymbolParseException );
}

// clang-format off
// static PrintTestParam printer2; // cannot use printer with ASSERT_THROW???
INSTANTIATE_TEST_SUITE_P( SymbolVariant, SymbolVariantRejectGrammarTest,
        ::testing::Values
        (
            // default
            ""s,
            "!"s,
            "\\"s,
            "/"s,
            "@HelloWorld_01234"s,
            "HelloWorld_01234@"s,
            "HelloWorld@_01234"s,

            "<.>"s,
            "<...>"s,
            "<.a>"s,
            "<a.>"s,
            "<a.a.a.>"s,
            "<a..a.a@>"s,
            "<a.@.a>"s,

            ">"s,
            "<"s,
            "<a>"s,
            "<a.a>>"s,
            "<<a.a>"s
        )
        );
// clang-format on


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// SymbolVariantPath
class SymbolVariantPathGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( SymbolVariantPathGrammarTest, SymbolVariantPath )
{
    using namespace mega;
    const std::string data = GetParam();
    mega::SymbolVariantPath s = mega::SymbolVariantPath::parse( data );
    std::string strDataStripped;
    boost::spirit_ext::strip( data, strDataStripped );
    ASSERT_EQ( s.str(), strDataStripped );
    std::ostringstream os;
    os << s;
    ASSERT_EQ( os.str(), strDataStripped );
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( SymbolVariantPath, SymbolVariantPathGrammarTest,
        ::testing::Values
        (
            // default
            "a"s,
            "A"s,
            "1"s,
            "_"s,
            "Hello_World_01234"s,

            "<a.a>"s,
            "<A.A.A>"s,
            "<1.2.3.4.5.6.7.8.9.0>"s,
            "<_._._._._>"s,
            "<Hello_World_01234.Hello_World_01234.Hello_World_01234>"s,

            "a . a"s,
            "A.A"s,
            "1.1.<A.A.A>"s,
            "_._"s,
            "Hello_World_01234  . Hello_World_01234.Hello_World_01234"s,

            "<a.a>.a.<a.a>"s,
            "<A.A.A>.<A.A.A>"s,
            "<1.2.3.4.5.6.7.8.9.0>.<1.2.3.4.5.6.7.8.9.0>"s,
            "<_._._._._>.1.2.3"s,
            "<Hello_World_01234.Hello_World_01234.Hello_World_01234>"s
        )
        );
// clang-format on

class SymbolVariantPathRejectGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( SymbolVariantPathRejectGrammarTest, SymbolVariantPath )
{
    using namespace mega;
    const std::string data = GetParam();
    ASSERT_THROW( mega::SymbolVariantPath::parse( data ), mega::SymbolParseException );
}

// clang-format off
// static PrintTestParam printer2; // cannot use printer with ASSERT_THROW???
INSTANTIATE_TEST_SUITE_P( SymbolVariantPath, SymbolVariantPathRejectGrammarTest,
        ::testing::Values
        (
            // default
            ""s,
            "!"s,
            "\\"s,
            "/"s,
            "@HelloWorld_01234"s,
            "HelloWorld_01234@"s,
            "HelloWorld@_01234"s,

            "<.>"s,
            "<...>"s,
            "<.a>"s,
            "<a.>"s,
            "<a.a.a.>"s,
            "<a..a.a@>"s,
            "<a.@.a>"s,

            ">"s,
            "<"s,
            "<a>"s,
            "<a.a>>"s,
            "<<a.a>"s,

            "a..a"s,
            "a.<.a"s,
            "a.<a>.a"s,
            "<<a.a>.a>"s
        )
        );
// clang-format on


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// SymbolVariantPathSequence
class SymbolVariantPathSequenceGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( SymbolVariantPathSequenceGrammarTest, SymbolVariantPathSequence )
{
    using namespace mega;
    const std::string data = GetParam();
    mega::SymbolVariantPathSequence s = mega::SymbolVariantPathSequence::parse( data );
    std::string strDataStripped;
    boost::spirit_ext::strip( data, strDataStripped );
    ASSERT_EQ( s.str(), strDataStripped );
    std::ostringstream os;
    os << s;
    ASSERT_EQ( os.str(), strDataStripped );
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( SymbolVariantPathSequence, SymbolVariantPathSequenceGrammarTest,
        ::testing::Values
        (
            // default
            "a"s,
            "A"s,
            "1"s,
            "_"s,
            "Hello_World_01234"s,

            "<a.a>"s,
            "<A.A.A>"s,
            "<1.2.3.4. 5.6.7. 8.9.0>"s,
            "<_._._._._>"s,
            "<Hello_World_01234. Hello_World_01234.Hello_World_01234>"s,

            "a.a"s,
            "A.A"s,
            "1.1.<A.A.A>"s,
            "_. _"s,
            "Hello_World_01234.Hello_World_01234.Hello_World_01234"s,

            "<a.a >.a.<a.a>"s,
            "< A.A.A >.<A.A.A>"s,
            "<1.2.3.4.5.6. 7 .8.9.0> . <1.2.3.4.5.6.7.8.9.0>"s,
            "<_._._._._>.1.2.3"s,
            "<Hello_World_01234.Hello_World_01234.Hello_World_01234>"s,

            "a,a"s,
            "<a.a>,a"s,
            "a.a,a"s,
            "<a.a>.a,a"s,
            "<a.a>.<a.a>.a,a, a , a, <a.a>.a,a"s
        )
        );
// clang-format on

class SymbolVariantPathSequenceRejectGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( SymbolVariantPathSequenceRejectGrammarTest, SymbolVariantPathSequence )
{
    using namespace mega;
    const std::string data = GetParam();
    ASSERT_THROW( mega::SymbolVariantPathSequence::parse( data ), mega::SymbolParseException );
}

// clang-format off
// static PrintTestParam printer2; // cannot use printer with ASSERT_THROW???
INSTANTIATE_TEST_SUITE_P( SymbolVariantPathSequence, SymbolVariantPathSequenceRejectGrammarTest,
        ::testing::Values
        (
            // default
            ""s,
            "!"s,
            "\\"s,
            "/"s,
            "@HelloWorld_01234"s,
            "HelloWorld_01234@"s,
            "HelloWorld@_01234"s,

            "<.>"s,
            "<...>"s,
            "<.a>"s,
            "<a.>"s,
            "<a.a.a.>"s,
            "<a..a.a@>"s,
            "<a.@.a>"s,

            ">"s,
            "<"s,
            "<a>"s,
            "<a.a>>"s,
            "<<a.a>"s,

            "a..a"s,
            "a.<.a"s,
            "a.<a>.a"s,
            "<<a.a>.a>"s,

            ","s,
            "a,"s,
            ",a"s,
            "<a.a,>"s,
            "<a.a>.a,,a"s
        )
        );
// clang-format on


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// NamedSymbolVariantPath
class NamedSymbolVariantPathGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( NamedSymbolVariantPathGrammarTest, NamedSymbolVariantPath )
{
    using namespace mega;
    const std::string data = GetParam();
    mega::NamedSymbolVariantPath s = mega::NamedSymbolVariantPath::parse( data );
    ASSERT_EQ( s.str(), data );
    std::ostringstream os;
    os << s;
    ASSERT_EQ( os.str(), data );
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( NamedSymbolVariantPath, NamedSymbolVariantPathGrammarTest,
        ::testing::Values
        (
            // default
            "a"s,
            "A"s,
            "1"s,
            "_"s,
            "Hello_World_01234"s,

            "<a.a>"s,
            "<A.A.A>"s,
            "<1.2.3.4.5.6.7.8.9.0>"s,
            "<_._._._._>"s,
            "<Hello_World_01234.Hello_World_01234.Hello_World_01234>"s,

            "A.A"s,
            "1.1.<A.A.A>"s,
            "_._"s,

            "<a.a>.a.<a.a>"s,
            "<A.A.A>.<A.A.A>"s,
            "<1.2.3.4.5.6.7.8.9.0>.<1.2.3.4.5.6.7.8.9.0>"s,
            "<_._._._._>.1.2.3"s,
            "<Hello_World_01234.Hello_World_01234.Hello_World_01234>"s,

            "a m_123Name"s,
            "A m_123Name"s,
            "1 m_123Name"s,
            "_ m_123Name"s,
            "Hello_World_01234 m_123Name"s,

            "<a.a> m_123Name"s,
            "<A.A.A> m_123Name"s,
            "<1.2.3.4.5.6.7.8.9.0> m_123Name"s,
            "<_._._._._> m_123Name"s,
            "<Hello_World_01234.Hello_World_01234.Hello_World_01234> m_123Name"s,

            "a.a m_123Name"s,
            "A.A m_123Name"s,
            "1.1.<A.A.A> m_123Name"s,
            "_._ m_123Name"s,

            "<a.a>.a.<a.a> m_123Name"s,
            "<A.A.A>.<A.A.A> m_123Name"s,
            "<1.2.3.4.5.6.7.8.9.0>.<1.2.3.4.5.6.7.8.9.0> m_123Name"s,
            "<_._._._._>.1.2.3 m_123Name"s,
            "<Hello_World_01234.Hello_World_01234.Hello_World_01234> m_123Name"s
        )
        );
// clang-format on

class NamedSymbolVariantPathRejectGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( NamedSymbolVariantPathRejectGrammarTest, NamedSymbolVariantPath )
{
    using namespace mega;
    const std::string data = GetParam();
    ASSERT_THROW( mega::NamedSymbolVariantPath::parse( data ), mega::SymbolParseException );
}

// clang-format off
// static PrintTestParam printer2; // cannot use printer with ASSERT_THROW???
INSTANTIATE_TEST_SUITE_P( NamedSymbolVariantPath, NamedSymbolVariantPathRejectGrammarTest,
        ::testing::Values
        (
            // default
            ""s,
            "!"s,
            "\\"s,
            "/"s,
            "@HelloWorld_01234 asd"s,
            "HelloWorld_01234@ asd"s,
            "HelloWorld@_01234 asd"s,

            "<.> asd"s,
            "<...> asd"s,
            "<.a> asd"s,
            "<a.> asd"s,
            "<a.a.a.> asd"s,
            "<a..a.a@> asd"s,
            "<a.@.a> asd"s,

            "> asd"s,
            "< asd"s,
            "<a> asd"s,
            "<a.a>> asd"s,
            "<<a.a> asd"s,

            "a..a asd"s,
            "a.<.a asd"s,
            "a.<a>.a asd"s,
            "<<a.a>.a> asd"s,

            "a !asd",
            "a a a"
        )
        );
// clang-format on


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// NamedSymbolVariantPathSequence
class NamedSymbolVariantPathSequenceGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( NamedSymbolVariantPathSequenceGrammarTest, NamedSymbolVariantPathSequence )
{
    using namespace mega;
    const std::string data = GetParam();
    mega::NamedSymbolVariantPathSequence s = mega::NamedSymbolVariantPathSequence::parse( data );
    ASSERT_EQ( s.str(), data );
    std::ostringstream os;
    os << s;
    ASSERT_EQ( os.str(), data );
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( NamedSymbolVariantPathSequence, NamedSymbolVariantPathSequenceGrammarTest,
        ::testing::Values
        (
            // default
            "a"s,
            "<a.a>"s,
            "a name"s,
            "<a.a> name"s,

            "a,a,a,a,a"s,
            "<a.a>,a,<a.a>,a,a"s,
            "<a.a> name,<a.a> name,<a.a> name"s,
            "a name,<a.a> name,<a.a.a> name"s,
            "a name,<a.a>,<a.a.a> name,a,a a"s
        )
        );
// clang-format on

class NamedSymbolVariantPathSequenceRejectGrammarTest : public ::testing::TestWithParam< std::string >
{
};

TEST_P( NamedSymbolVariantPathSequenceRejectGrammarTest, NamedSymbolVariantPathSequence )
{
    using namespace mega;
    const std::string data = GetParam();
    ASSERT_THROW( mega::NamedSymbolVariantPathSequence::parse( data ), mega::SymbolParseException );
}

// clang-format off
// static PrintTestParam printer2; // cannot use printer with ASSERT_THROW???
INSTANTIATE_TEST_SUITE_P( NamedSymbolVariantPathSequence, NamedSymbolVariantPathSequenceRejectGrammarTest,
        ::testing::Values
        (
            // default
            ""s,
            "!"s,
            "\\"s,
            "/"s,
            "@HelloWorld_01234 asd"s,
            "HelloWorld_01234@ asd"s,
            "HelloWorld@_01234 asd"s,

            "<.> asd"s,
            "<...> asd"s,
            "<.a> asd"s,
            "<a.> asd"s,
            "<a.a.a.> asd"s,
            "<a..a.a@> asd"s,
            "<a.@.a> asd"s,

            "> asd"s,
            "< asd"s,
            "<a> asd"s,
            "<a.a>> asd"s,
            "<<a.a> asd"s,

            "a..a asd"s,
            "a.<.a asd"s,
            "a.<a>.a asd"s,
            "<<a.a>.a> asd"s,

            "a !asd",
            "a a a",

            "a,",
            ",a",
            "a,,a"
        )
        );
// clang-format on
