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

#include "schematic/cut.hpp"
#include "schematic/cgalUtils.hpp"

#include "common/assert_verify.hpp"
#include "common/rounding.hpp"

namespace schematic
{

const std::string& Cut::TypeName()
{
    static const std::string strTypeName( "cut" );
    return strTypeName;
}

Cut::Cut( Node::Ptr pParent, const std::string& strName )
    : Site( pParent, strName )
{
}

Cut::Cut( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Site( pOriginal, pParent, strName )
{
}

Node::Ptr Cut::copy( Node::Ptr pParent, const std::string& strName ) const
{
    return Node::copy< Cut >( boost::dynamic_pointer_cast< const Cut >( shared_from_this() ), pParent, strName );
}

void Cut::load( const format::Node& node )
{
    Site::load( node );

    VERIFY_RTE( node.has_site() && node.site().has_cut() );
    const format::Node::Site::Cut& cut = node.site().cut();

    m_pLineSegment = get< Feature_LineSegment >( "segment" );
    VERIFY_RTE_MSG( m_pLineSegment, "Cut segment not found" );
}

void Cut::save( format::Node& node ) const
{
    format::Node::Site::Cut& cut = *node.mutable_site()->mutable_cut();
    Site::save( node );
}

std::string Cut::getStatement() const
{
    return Site::getStatement();
}

Rect Cut::getAABB() const
{
    const std::vector< Segment > segments{ m_pLineSegment->getSegment() };
    return CGAL::bbox_2( segments.begin(), segments.end() );
}

void Cut::init()
{
    if( !( m_pLineSegment = get< Feature_LineSegment >( "segment" ) ) )
    {
        m_pLineSegment.reset( new Feature_LineSegment( getPtr(), "segment" ) );
        m_pLineSegment->init();
        m_pLineSegment->setPoint( 0, Point( 0.0f, 0.0f ) );
        m_pLineSegment->setPoint( 1, Point( 0.0f, 8.0f ) );
        add( m_pLineSegment );
    }

    Site::init();

    const Polygon contour = calculateContour();
    m_pMarkupContour->setPolygon( contour );
    m_pMarkupContourOutline->setPolygon( contour );
}

void Cut::init( const Transform& transform )
{
    Cut::init();
    setTransform( transform );
}

Polygon Cut::calculateContour()
{
    const Segment segment = m_pLineSegment->getSegment();

    Polygon siteContour;
    {
        Vector v    = segment[ 0 ] - segment[ 1 ];
        v           = v.perpendicular( CGAL::Orientation::COUNTERCLOCKWISE );
        double size = CGAL::approximate_sqrt( v.squared_length() );
        if( size > 0 )
        {
            v = v / size;

            siteContour.push_back( segment[ 0 ] - v * 0.25 );
            siteContour.push_back( segment[ 0 ] + v * 0.25 );
            siteContour.push_back( segment[ 1 ] + v * 0.25 );
            siteContour.push_back( segment[ 1 ] - v * 0.25 );
        }
        else
        {
            siteContour.clear();
        }
    }
    if( !siteContour.is_empty() && !siteContour.is_counterclockwise_oriented() )
    {
        std::reverse( siteContour.begin(), siteContour.end() );
    }

    return siteContour;
}

bool Cut::task_contour()
{
    bool bModified = false;

    const bool bSubTreeModified = Site::task_contour();

    const Polygon& siteContour = calculateContour();

    if( bSubTreeModified || ( siteContour != m_siteContourCache ) || ( m_siteContourTick < getLastModifiedTick() ) )
    {
        m_siteContourTick.update();
        m_siteContourCache = siteContour;
        m_pMarkupContour->setPolygon( siteContour );
        m_pMarkupContourOutline->setPolygon( siteContour );

        bModified = true;
    }

    return bModified;
}

} // namespace schematic