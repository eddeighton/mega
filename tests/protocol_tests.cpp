#include "service/protocol/schema.pb.h"

#include <gtest/gtest.h>


TEST( Protocol, Basic )
{
    mega::Message msg;
    msg.set_success( true );

    ASSERT_TRUE( msg.success() );


}
