#include "service/network/network.hpp"

#include <gtest/gtest.h>


TEST( Network, Basic )
{
    ASSERT_STREQ( mega::network::getVersion(), "0.0.0.0" );

}
