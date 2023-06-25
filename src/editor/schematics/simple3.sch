Schematic(base;,0)
{
    Space(space_0000;,0)
    {
        transform(1,0,0,0,1,0)
        Contour(contour;,0)
        {
            path(0,0,88,0,88,88,0,88)
        }
        Property(width;,1)
        {
            value(0.5f;)
        }
        Space(spac_0009;,2)
        {
            transform(-1,0,28,0,1,30)
            Contour(contour;,0)
            {
                path(-34,-16,16,-16,16,40,-34,40)
            }
            Property(width;,1)
            {
                value(0.5f;)
            }
            Wall(wall_0002;,2)
            {
                transform(-1,0,16,0,1,30)
                Contour(contour;,0)
                {
                    path(0,-4,16,-4,16,4,0,4)
                }
                Property(width;,1)
                {
                    value(0.5f;)
                }
            }
            Wall(wall_0003;,3)
            {
                transform(-1,0,-9,0,1,9)
                Contour(contour;,0)
                {
                    path(0,-4,16,-4,16,4,0,4)
                }
                Property(width;,1)
                {
                    value(0.5f;)
                }
            }
            Space(space_0004;,4)
            {
                transform(-1,0,-14,0,1,-16)
                Contour(contour;,0)
                {
                    path(0,0,20,0,20,24,0,24)
                }
                Property(width;,1)
                {
                    value(0.5f;)
                }
            }
            Connection(conn_0005;,5)
            {
                transform(2.66454e-15,-1,-14,-1,-2.66454e-15,-4)
                Point(width;,0)
                {
                    position(6.5,2)
                }
            }
            Connection(conn_0006;,6)
            {
                transform(-1,0,-22,0,1,40)
                Point(width;,0)
                {
                    position(5.5,3)
                }
            }
            Connection(conn_0007;,7)
            {
                transform(2.66454e-15,-1,-34,-1,-2.66454e-15,-5)
                Point(width;,0)
                {
                    position(5.5,3)
                }
            }
            Pin(pin_0008;,8)
            {
                position(-13,37)
                Property(path;,0)
                {
                    value(True;)
                }
            }
            Pin(pin_0009;,9)
            {
                position(-13,43)
                Property(path;,0)
                {
                    value(True;)
                }
            }
        }
    }
}
