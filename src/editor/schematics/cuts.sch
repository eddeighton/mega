Schematic(base;,0)
{
    Space(space_0000;,0)
    {
        transform(1,0,-2,0,1,-10)
        Contour(contour;,0)
        {
            path(2,10,126,10,126,94,2,94)
        }
        Property(width;,1)
        {
            value(0.5f;)
        }
        Space(spac_0002;,2)
        {
            transform(1,0,28,0,1,38)
            Contour(contour;,0)
            {
                path(-16,-10,92,-10,92,42,-16,42)
            }
            Property(width;,1)
            {
                value(0.5f;)
            }
            Connection(conn_0002;,2)
            {
                transform(-2.66454e-15,1,-16,-1,-2.66454e-15,16)
                Point(width;,0)
                {
                    position(11.5,3)
                }
            }
            Wall(wall_0003;,3)
            {
                transform(1,0,76,0,1,24)
                Contour(contour;,0)
                {
                    path(-64,-16,16,-16,16,2,-64,2)
                }
                Property(width;,1)
                {
                    value(0.5f;)
                }
            }
            Cut(cut_0004;,4)
            {
                transform(1,0,18,0,1,4)
                LineSegment(segment;,0)
                {
                    start(0,0)
                    end(0,26)
                }
            }
            Cut(cut__0005;,5)
            {
                transform(1,0,26,0,1,4)
                LineSegment(segment;,0)
                {
                    start(0,0)
                    end(0,26)
                }
            }
            Cut(cut__0006;,6)
            {
                transform(1,0,42,0,1,4)
                LineSegment(segment;,0)
                {
                    start(0,0)
                    end(0,26)
                }
            }
            Cut(cut__0007;,7)
            {
                transform(1,0,34,0,1,4)
                LineSegment(segment;,0)
                {
                    start(0,0)
                    end(0,26)
                }
            }
            Cut(cut__0008;,8)
            {
                transform(1,0,58,0,1,4)
                LineSegment(segment;,0)
                {
                    start(0,0)
                    end(0,26)
                }
            }
            Cut(cut__0009;,9)
            {
                transform(1,0,50,0,1,4)
                LineSegment(segment;,0)
                {
                    start(0,0)
                    end(0,26)
                }
            }
            Cut(cut__0010;,10)
            {
                transform(1,0,66,0,1,4)
                LineSegment(segment;,0)
                {
                    start(0,0)
                    end(0,26)
                }
            }
            Cut(cut__0011;,11)
            {
                transform(1,0,74,0,1,4)
                LineSegment(segment;,0)
                {
                    start(0,0)
                    end(0,26)
                }
            }
            Cut(cut__0012;,12)
            {
                transform(1,0,82,0,1,4)
                LineSegment(segment;,0)
                {
                    start(0,0)
                    end(0,26)
                }
            }
            Pin(pin_0013;,13)
            {
                position(15,18)
                Property(plane;,0)
                {
                    value(Hole;)
                }
            }
            Pin(pin_0014;,14)
            {
                position(22,18)
                Property(plane;,0)
                {
                    value(Ground;)
                }
            }
            Pin(pin_0015;,15)
            {
                position(29,19)
                Property(plane;,0)
                {
                    value(Hole;)
                }
            }
            Pin(pin_0016;,16)
            {
                position(38,17)
                Property(plane;,0)
                {
                    value(Mid;)
                }
            }
            Pin(pin_0017;,17)
            {
                position(46,16)
                Property(plane;,0)
                {
                    value(Hole;)
                }
            }
            Pin(pin_0018;,18)
            {
                position(53,18)
                Property(plane;,0)
                {
                    value(Ceiling;)
                }
            }
            Pin(pin_0019;,19)
            {
                position(62,17)
                Property(plane;,0)
                {
                    value(Ground;)
                }
            }
            Pin(pin_0020;,20)
            {
                position(70,17)
                Property(plane;,0)
                {
                    value(Ceiling;)
                }
            }
            Pin(pin_0021;,21)
            {
                position(78,19)
                Property(plane;,0)
                {
                    value(Ground;)
                }
            }
            Pin(pin_0022;,22)
            {
                position(87,18)
                Property(plane;,0)
                {
                    value(Ceiling;)
                }
            }
        }
    }
}
