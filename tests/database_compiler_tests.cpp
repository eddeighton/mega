
#include "database_compiler/grammar.hpp"

#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include <utility>
#include <sstream>

// IdentifierAccept
class IdentifierAccept : public ::testing::TestWithParam< std::string >
{
protected:
    std::string str;
};

TEST_P( IdentifierAccept, Basic )
{
    std::string        str = GetParam();
    dbcomp::Identifier result = dbcomp::parse< dbcomp::Identifier >( str );
    ASSERT_EQ( result, str );
}

INSTANTIATE_TEST_SUITE_P( Identifier_Accept,
                          IdentifierAccept,
                          ::testing::Values( "a", "B", "a1", "m_1",
                                             "TheQuickBrownFoxJumpedOverTheLazyDoc1234567890" ) );

// IdentifierReject
class IdentifierReject : public ::testing::TestWithParam< std::string >
{
protected:
    std::string str;
};

TEST_P( IdentifierReject, Basic )
{
    dbcomp::Identifier  id;
    std::ostringstream  osError;
    dbcomp::ParseResult result = dbcomp::parse( GetParam(), id, osError );
    ASSERT_FALSE( result.bSuccess );
}

INSTANTIATE_TEST_SUITE_P( Identifier_Reject,
                          IdentifierReject,
                          ::testing::Values( "1", "_", "&", "!", "£", "$", "%", "^", "&", ",", "(",
                                             ")", "123_", "=123=" ) );

// IdentifierListAccept
class IdentifierListAccept : public ::testing::TestWithParam< std::string >
{
protected:
    std::string str;
};

TEST_P( IdentifierListAccept, Basic )
{
    std::string            str = GetParam();
    dbcomp::IdentifierList result = dbcomp::parse< dbcomp::IdentifierList >( str );
    std::ostringstream     os;
    os << result;
    ASSERT_EQ( os.str(), str );
}

INSTANTIATE_TEST_SUITE_P( IdentifierList_Accept,
                          IdentifierListAccept,
                          ::testing::Values( "a", "a::b", "abc::def::efg" ) );

// TypeAccept
class TypeAccept : public ::testing::TestWithParam< std::string >
{
protected:
    std::string str;
};

TEST_P( TypeAccept, Basic )
{
    dbcomp::Type        type;
    std::ostringstream  osError;
    dbcomp::ParseResult result = dbcomp::parse( GetParam(), type, osError );
    std::cout << type << std::endl;
    ASSERT_TRUE( result.bSuccess );
}

INSTANTIATE_TEST_SUITE_P( Type_Accept,
                          TypeAccept,
                          ::testing::Values( "a", "a< b::c >", "a::b< b, c, d >",
                                             "a< b< e >, c< f, g >, d >" ) );

// TypeReject
class TypeReject : public ::testing::TestWithParam< std::string >
{
protected:
    std::string str;
};

TEST_P( TypeReject, Basic )
{
    dbcomp::Type        type;
    std::ostringstream  osError;
    dbcomp::ParseResult result = dbcomp::parse( GetParam(), type, osError );
    ASSERT_FALSE( result.bSuccess );
}

INSTANTIATE_TEST_SUITE_P( Type_Reject,
                          TypeReject,
                          ::testing::Values( "a<", "a<<>", "<a>", "a<>", ">a<", "a<<b>>" ) );

// StageAccept
class StageAccept : public ::testing::TestWithParam< std::string >
{
protected:
    std::string str;
};

TEST_P( StageAccept, Basic )
{
    dbcomp::Stage       stage;
    std::ostringstream  osError;
    dbcomp::ParseResult result = dbcomp::parse( GetParam(), stage, osError );
    std::cout << stage << std::endl;
    ASSERT_TRUE( result.bSuccess );
}

INSTANTIATE_TEST_SUITE_P(
    Stage_Accept,
    StageAccept,
    ::testing::Values( "stage test{}", "stage test{ object one program two object three }" ) );

// PropertyAccept
class PropertyAccept : public ::testing::TestWithParam< std::string >
{
protected:
    std::string str;
};

TEST_P( PropertyAccept, Basic )
{
    dbcomp::Property    property;
    std::ostringstream  osError;
    dbcomp::ParseResult result = dbcomp::parse( GetParam(), property, osError );
    std::cout << property << std::endl;
    ASSERT_TRUE( result.bSuccess );
}

INSTANTIATE_TEST_SUITE_P( Property_Accept,
                          PropertyAccept,
                          ::testing::Values( "test map< ref< a::b >, value< std::string >, pred< "
                                             "compare > > -> somefile" ) );

// ObjectAccept
class ObjectAccept : public ::testing::TestWithParam< std::string >
{
protected:
    std::string str;
};

TEST_P( ObjectAccept, Basic )
{
    dbcomp::Object      object;
    std::ostringstream  osError;
    dbcomp::ParseResult result = dbcomp::parse( GetParam(), object, osError );
    std::cout << object << std::endl;
    ASSERT_TRUE( result.bSuccess );
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( Property_Accept,
ObjectAccept,

::testing::Values(

"object test -> file{}",

"object test -> file"
"{"
"   value name"
"}",

"object test -> somefile"
"{"
"    ref< a > context -> otherfile"
"    value< std::string > name"
"    map< ref< a >, value< bool >, pref< compare > > mapping"
"}"

) );
// clang-format on

// NamespaceAccept
class NamespaceAccept : public ::testing::TestWithParam< std::string >
{
protected:
    std::string str;
};

TEST_P( NamespaceAccept, Basic )
{
    dbcomp::Namespace   namespace_;
    std::ostringstream  osError;
    dbcomp::ParseResult result = dbcomp::parse( GetParam(), namespace_, osError );
    std::cout << namespace_ << std::endl;
    ASSERT_TRUE( result.bSuccess );
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Namespace_Accept,
    NamespaceAccept,
::testing::Values( 
    
"namespace test{}", 

"namespace test"
"{"
"    namespace test{}"
"    namespace test{}"
"}",

"namespace test"
"{"
"   object test -> somefile"
"   {"
"       value< std::string > name"
"   }"
"   namespace test{}"
"}"

) );
// clang-format on

// SchemaAccept
class SchemaAccept : public ::testing::TestWithParam< std::string >
{
protected:
    std::string str;
};

TEST_P( SchemaAccept, Basic )
{
    dbcomp::Schema      schema;
    std::ostringstream  osError;
    dbcomp::ParseResult result = dbcomp::parse( GetParam(), schema, osError );
    std::cout << schema << std::endl;
    ASSERT_TRUE( result.bSuccess );
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Schema_Accept,
    SchemaAccept,
::testing::Values( 
    
"namespace test{}", 

"stage test"
"{"
"  object fileone"
"  program fileone"
"}",

"stage test"
"{"
"  object fileone"
"  program fileone"
"  object fileone"
"  program fileone"
"}"
"namespace test"
"{"
"   object test -> somefile"
"   {"
"       value< std::string > name"
"   }"
"}"
"stage test2"
"{"
"  object fileone"
"  program fileone"
"  object fileone"
"  program fileone"
"}"
"namespace test2"
"{"
"   object test -> somefile"
"   {"
"       value< std::string > name"
"   }"
"}"

) );
// clang-format on
