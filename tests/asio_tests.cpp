

#include <gtest/gtest.h>
/*
#include "service/network/end_point.hpp"
#include "service/protocol/common/serialisation.hpp"

struct TestMsg
{
    std::string                                 str;
    int                                         i;
    std::vector< std::map< int, std::string > > data;
};
*/
TEST( Asio, EncodeDecode )
{
   // using namespace mega::network;

    // clang-format off
    /*TestMsg msg
    {
        "testing",
        123,
        { 
            { 
                { 1, { std::string{ "Ed was here" }  } }, 
                { 2, { std::string{ "Ed was here" }  } }, 
                { 3, { std::string{ "Ed was here" }  } }, 
                { 4, { std::string{ "Ed was not here" } } }  
            }, 
            { 
                { 1, { std::string{ "Ed was somwehere" } } }, 
                { 2, { std::string{ "Ed was here" }} } 
            } 
        } 
    };
    // clang-format on

    SendBuffer sentBuffer;
    {
        {
            SendBuffer                                         buffer( sizeof( MessageSize ) );
            boost::interprocess::basic_vectorbuf< SendBuffer > os( buffer );
            os.pubseekpos( sizeof( MessageSize ) );

            {
                boost::archive::binary_oarchive oa( os );

                oa& msg.str;
                oa& msg.i;
                oa& msg.data;
            }
            sentBuffer = os.vector();
        }
        encodeSize( sentBuffer, sentBuffer.size() - sizeof( MessageSize ) );
    }

    const MessageSize size = *reinterpret_cast< const MessageSize* >( sentBuffer.data() );
    ASSERT_EQ( size, sentBuffer.size() - sizeof( MessageSize ) );

    TestMsg receivedMsg;
    {
        SendBuffer::const_iterator i = sentBuffer.begin() + sizeof( MessageSize );
        SendBuffer                 receiveBuffer( i, i + size );

        boost::interprocess::basic_vectorbuf< SendBuffer > is( receiveBuffer );

        boost::archive::binary_iarchive ia( is );

        ia& receivedMsg.str;
        ia& receivedMsg.i;
        ia& receivedMsg.data;
    }

    ASSERT_EQ( msg.str, receivedMsg.str );
    ASSERT_EQ( msg.i, receivedMsg.i );
    ASSERT_EQ( msg.data, receivedMsg.data );*/
}