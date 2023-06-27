
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

#include "schematic/rasteriser.hpp"

namespace schematic
{

// from ye olde days: /workspace/OldWorkspace/SourceMods/ai_mod/src/NAVBITMAP/NavBitmapAlgorithms.h
void Rasteriser::line( Int p1x, Int p1y, Int p2x, Int p2y, const ColourType& colour )
{
    /**
     Draws a line between two points p1(p1x,p1y) and p2(p2x,p2y).
     This function is based on the Bresenham's line algorithm and is highly 
     optimized to be able to draw lines very quickly. There is no floating point 
     arithmetic nor multiplications and divisions involved. Only addition, 
     subtraction and bit shifting are used. 

     Note that you have to define your own customized setPixel(x,y) function, 
     which essentially lights a pixel on the screen.
    */
    Int F, x, y;

    if (p1x > p2x)  // Swap points if p1 is on the right of p2
    {
        std::swap(p1x, p2x);
        std::swap(p1y, p2y);
    }

    // Handle trivial cases separately for algorithm speed up.
    // Trivial case 1: m = +/-INF (Vertical line)
    if (p1x == p2x)
    {
        if ( p2y > p1y )
        {
            x = p1x;
            y = p1y;
            while ( y <= p2y  )
            {
                setPixel(x, y, colour);
                y++;
            }
        }
        else
        {
            x = p1x;
            y = p1y;
            while ( y > p2y  )
            {
                setPixel(x, y, colour);
                y--;
            }
        }
        return;
    }
    // Trivial case 2: m = 0 (Horizontal line)
    else if (p1y == p2y)
    {
        x = p1x;
        y = p1y;

        while ( x <= p2x )
        {
            setPixel(x, y, colour);
            x++;
        }
        return;
    }

    Int dy            = p2y - p1y;  // y-increment from p1 to p2
    Int dx            = p2x - p1x;  // x-increment from p1 to p2
    Int dy2           = (dy << 1);  // dy << 1 == 2*dy
    Int dx2           = (dx << 1);
    Int dy2_minus_dx2 = dy2 - dx2;  // precompute constant for speed up
    Int dy2_plus_dx2  = dy2 + dx2;

    if (dy >= 0)    // m >= 0
    {
        // Case 1: 0 <= m <= 1 (Original case)
        if (dy <= dx)   
        {
            F = dy2 - dx;    // initial F
            x = p1x;
            y = p1y;
            while ( x <= p2x )
            {
                setPixel(x, y, colour);
                if (F <= 0)
                    F += dy2;
                else
                {
                    y++;
                    F += dy2_minus_dx2;
                }
                x++;
            }
        }
        // Case 2: 1 < m < INF (Mirror about y=x line
        // replace all dy by dx and dx by dy)
        else
        {
            F = dx2 - dy;    // initial F
            y = p1y;
            x = p1x;
            while ( y <= p2y )
            {
                setPixel(x, y, colour);
                if (F <= 0)
                    F += dx2;
                else
                {
                    x++;
                    F -= dy2_minus_dx2;
                }
                y++;
            }
        }
    }
    else    // m < 0
    {
        // Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
        if (dx >= -dy)
        {
            F = -dy2 - dx;    // initial F
            x = p1x;
            y = p1y;
            while ( x <= p2x )
            {
                setPixel(x, y, colour);
                if (F <= 0)
                    F -= dy2;
                else
                {
                    y--;
                    F -= dy2_plus_dx2;
                }
                x++;
            }
        }
        // Case 4: -INF < m < -1 (Mirror about x-axis and mirror 
        // about y=x line, replace all dx by -dy and dy by dx)
        else    
        {
            F = dx2 + dy;    // initial F
            y = p1y;
            x = p1x;
            while ( y >= p2y  )
            {
                setPixel(x, y, colour);
                if (F <= 0)
                    F += dx2;
                else
                {
                    x++;
                    F += dy2_plus_dx2;
                }
                y--;
            }
        }
    }
}
}

