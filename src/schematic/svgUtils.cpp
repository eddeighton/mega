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

#include "schematic/svgUtils.hpp"

#include "common/file.hpp"

namespace
{

template < typename TCGALValueType >
double toSVG( const TCGALValueType& value )
{
    return -CGAL::to_double( value );
}

static const std::vector< const char* > SVG_COLOURS
    = { "blue", "green", "red", "yellow", "orange", "purple", "brown", "black" };

void svgLine( const schematic::SVGStyle& style, exact::Analysis::Arrangement::Halfedge_const_handle h, double minX,
              double minY, double scale, const char* pszColour, std::ostream& os )
{
    if( h->target()->point() == h->curve().source() )
        h = h->twin();

    const double startX = ( toSVG( h->source()->point().x() ) - minX ) * scale;
    const double startY = ( toSVG( h->source()->point().y() ) - minY ) * scale;
    const double endX   = ( toSVG( h->target()->point().x() ) - minX ) * scale;
    const double endY   = ( toSVG( h->target()->point().y() ) - minY ) * scale;

    std::ostringstream osEdge;
    osEdge << startX << "," << startY << " " << ( startX + ( endX - startX ) / 2.0 ) << ","
           << ( startY + ( endY - startY ) / 2.0 ) << " " << endX << "," << endY;

    if( style.bArrows )
    {
        os << "       <polyline points=\"" << osEdge.str() << "\" style=\"fill:none;stroke:" << pszColour
           << ";stroke-width:1\" marker-mid=\"url(#mid)\" />\n";
    }
    else
    {
        os << "       <polyline points=\"" << osEdge.str() << "\" style=\"fill:none;stroke:" << pszColour
           << ";stroke-width:1\" />\n";
    }

    if( style.bDots )
    {
        os << "       <circle cx=\"" << startX << "\" cy=\"" << startY << "\" r=\"3\" stroke=\"" << pszColour
           << "\" stroke-width=\"1\" fill=\"" << pszColour << "\" />\n";
        os << "       <circle cx=\"" << endX << "\" cy=\"" << endY << "\" r=\"3\" stroke=\"" << pszColour
           << "\" stroke-width=\"1\" fill=\"" << pszColour << "\" />\n";
    }
}

} // namespace

namespace schematic
{
void generateHTML( const boost::filesystem::path&      filepath,
                   const exact::Analysis::Arrangement& arr,
                   const EdgeVectorVector&             edgeGroups,
                   const SVGStyle&                     style )
{
    std::unique_ptr< boost::filesystem::ofstream > os = createNewFileStream( filepath );

    double scale = 16.0;
    double minX = std::numeric_limits< double >::max(), minY = std::numeric_limits< double >::max();
    double maxX = -std::numeric_limits< double >::max(), maxY = -std::numeric_limits< double >::max();
    for( auto i = arr.edges_begin(); i != arr.edges_end(); ++i )
    {
        {
            const double x = toSVG( i->source()->point().x() );
            const double y = toSVG( i->source()->point().y() );
            if( x < minX )
                minX = x;
            if( y < minY )
                minY = y;
            if( x > maxX )
                maxX = x;
            if( y > maxY )
                maxY = y;
        }

        {
            const double x = toSVG( i->target()->point().x() );
            const double y = toSVG( i->target()->point().y() );
            if( x < minX )
                minX = x;
            if( y < minY )
                minY = y;
            if( x > maxX )
                maxX = x;
            if( y > maxY )
                maxY = y;
        }
    }
    const double sizeX = maxX - minX;
    const double sizeY = maxY - minY;

    *os << "<!DOCTYPE html>\n";
    *os << "<html>\n";
    *os << "  <head>\n";
    *os << "    <title>Compilation Output</title>\n";
    *os << "  </head>\n";
    *os << "  <body>\n";
    *os << "    <h1>" << filepath.string() << "</h1>\n";

    *os << "    <svg width=\"" << 100 + sizeX * scale << "\" height=\"" << 100 + sizeY * scale << "\" >\n";
    *os << "      <defs>\n";
    *os << "      <marker id=\"mid\" markerWidth=\"10\" markerHeight=\"10\" refX=\"0\" refY=\"3\" orient=\"auto\" "
           "markerUnits=\"strokeWidth\">\n";
    *os << "      <path d=\"M0,0 L0,6 L9,3 z\" fill=\"#f00\" />\n";
    *os << "      </marker>\n";
    *os << "      </defs>\n";
    *os << "       <text x=\"" << 10 << "\" y=\"" << 10
        << "\" fill=\"green\"  >Vertex Count: " << arr.number_of_vertices() << " </text>\n";
    *os << "       <text x=\"" << 10 << "\" y=\"" << 30 << "\" fill=\"green\"  >Edge Count: " << arr.number_of_edges()
        << " </text>\n";
    *os << "       <text x=\"" << 10 << "\" y=\"" << 50 << "\" fill=\"green\"  >Face Count: " << arr.number_of_faces()
        << " </text>\n";

    int iColour = 0;
    for( const EdgeVector& edges : edgeGroups )
    {
        const char* pszColour = SVG_COLOURS[ iColour ];
        iColour               = ( iColour + 1 ) % SVG_COLOURS.size();
        for( exact::Analysis::Arrangement::Halfedge_const_handle h : edges )
        {
            if( h->target()->point() == h->curve().source() )
                h = h->twin();

            svgLine( style, h, minX, minY, scale, pszColour, *os );

            {
                const double startX = ( toSVG( h->source()->point().x() ) - minX ) * scale;
                const double startY = ( toSVG( h->source()->point().y() ) - minY ) * scale;
                const double endX   = ( toSVG( h->target()->point().x() ) - minX ) * scale;
                const double endY   = ( toSVG( h->target()->point().y() ) - minY ) * scale;

                std::ostringstream osText;
                {
                    // const void* pData       = h->data();
                    // const void* pTwinData   = h->twin()->data();
                    /*if( const schematic::Area* pArea = (const schematic::Area*)pData )
                    {
                        osText << "l:" << pArea->getName();
                    }
                    else
                    {
                        osText << "l:";
                    }
                    if( const schematic::Area* pArea = (const schematic::Area*)pTwinData )
                    {
                        osText << " r:" << pArea->getName();
                    }
                    else
                    {
                        osText << " r:";
                    }*/
                } {
                    Float x = startX + ( endX - startX ) / 2.0f;
                    Float y = startY + ( endY - startY ) / 2.0f;
                    *os << "       <text x=\"" << x << "\" y=\"" << y << "\" fill=\"green\" transform=\"rotate(30 " << x
                        << "," << y << ")\" >" << osText.str() << " </text>\n";
                }
            }
        }
    }

    *os << "    </svg>\n";
    *os << "  </body>\n";
    *os << "</html>\n";
}
} // namespace schematic
