

#include <gtest/gtest.h>

TEST( Protocol, Basic )
{
    struct Test
    {
        Test( int i )
            : m_value( i )
        {
        }

        int m_value;
    };

    std::array< Test, 4 > array{ { 123, 123, 123, 123 } };


}
