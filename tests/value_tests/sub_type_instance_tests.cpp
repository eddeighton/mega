
#include "mega/values/compilation/sub_type_instance.hpp"
/*
#include "mega/bin_archive.hpp"
#include "mega/record_archive.hpp"
*/
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/interprocess/streams/vectorstream.hpp>

#include <limits>
#include <sstream>

#include <gtest/gtest.h>

TEST( SubTypeInstance, Basic )
{
    using namespace mega;
    {
        SubTypeInstance s;
        ASSERT_EQ( s.getSubType(), 0 );
        ASSERT_EQ( s.getInstance(), 0 );
    }
    {
        SubTypeInstance s( 1, 2 );
        ASSERT_EQ( s.getSubType(), 1 );
        ASSERT_EQ( s.getInstance(), 2 );
    }
    {
        const auto      maxSubType  = std::numeric_limits< SubType >::max();
        const auto      maxInstance = std::numeric_limits< Instance >::max();
        SubTypeInstance s{ maxSubType, maxInstance };
        ASSERT_EQ( s.getSubType(), maxSubType );
        ASSERT_EQ( s.getInstance(), maxInstance );
    }
}

TEST( SubTypeInstance, Value )
{
    using namespace mega;
    {
        SubTypeInstance s( 0U );
        ASSERT_EQ( s.getSubType(), 0 );
        ASSERT_EQ( s.getInstance(), 0 );
    }
    {
        SubTypeInstance s( 0x00010002 );
        ASSERT_EQ( s.getSubType(), 1 );
        ASSERT_EQ( s.getInstance(), 2 );
    }
    {
        SubTypeInstance s( 0x00020001 );
        ASSERT_EQ( s.getSubType(), 2 );
        ASSERT_EQ( s.getInstance(), 1 );
    }
    {
        SubTypeInstance s( 0xFFFF0000 );
        ASSERT_EQ( s.getSubType(), 0xFFFF );
        ASSERT_EQ( s.getInstance(), 0 );
    }
    {
        SubTypeInstance s( 0x0000FFFF );
        ASSERT_EQ( s.getSubType(), 0x0 );
        ASSERT_EQ( s.getInstance(), 0xFFFF );
    }
}

struct SubTypeInstanceTestData
{
    mega::SubTypeInstance expected;
};

class SubTypeInstanceBinArchiveTest : public ::testing::TestWithParam< SubTypeInstanceTestData >
{
protected:
};

TEST_P( SubTypeInstanceBinArchiveTest, SubTypeInstanceBinArchive )
{
    const SubTypeInstanceTestData data = GetParam();

    /*{
        mega::BinSaveArchive saveArchive;
        saveArchive.save( data.expected );

        mega::BinLoadArchive  loadArchive( saveArchive.makeSnapshot( 0 ) );
        mega::SubTypeInstance result;
        loadArchive.load( result );

        std::ostringstream osError;
        osError << " expected: " << data.expected << " actual: " << result;
        ASSERT_TRUE( result == data.expected ) << osError.str();
    }*/

    {
        boost::interprocess::basic_vectorbuf< std::vector< char > > buffer;
        boost::archive::binary_oarchive                             saveArchive( buffer );
        saveArchive&                                                data.expected;

        boost::archive::binary_iarchive loadArchive( buffer );
        mega::SubTypeInstance           result;
        loadArchive&                    result;

        std::ostringstream osError;
        osError << " expected: " << data.expected << " actual: " << result;
        ASSERT_TRUE( result == data.expected ) << osError.str();
    }

    {
        std::string str;
        {
            std::ostringstream os;
            using ::           operator<<;
            os << data.expected;
            str = os.str();
        }

        mega::SubTypeInstance result;
        {
            std::istringstream is( str );
            using ::           operator>>;
            is >> result;
        }

        std::ostringstream osError;
        osError << " expected: " << data.expected << " actual: " << result;
        ASSERT_TRUE( result == data.expected ) << osError.str();
    }
}

// clang-format off
INSTANTIATE_TEST_SUITE_P( SubTypeInstanceBinArchive, SubTypeInstanceBinArchiveTest,
    ::testing::Values
    (
        SubTypeInstanceTestData{ mega::SubTypeInstance{} },
        SubTypeInstanceTestData{ mega::SubTypeInstance( 1, 2 ) },
        SubTypeInstanceTestData{ mega::SubTypeInstance( 0, std::numeric_limits< mega::Instance >::max() ) },
        SubTypeInstanceTestData{ mega::SubTypeInstance( std::numeric_limits< mega::SubType >::max(), 0 ) },
        SubTypeInstanceTestData{ mega::SubTypeInstance( std::numeric_limits< mega::SubType >::max(), std::numeric_limits< mega::Instance >::max() ) }
    ));
// clang-format on
