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

#include "schematic/connection.hpp"
#include "schematic/cgalUtils.hpp"

#include "common/assert_verify.hpp"
#include "common/rounding.hpp"

namespace schematic
{

const std::string& Connection::TypeName()
{
    static const std::string strTypeName( "connect" );
    return strTypeName;
}

Connection::Connection( Node::Ptr pParent, const std::string& strName )
    : Site( pParent, strName )
{
}

Connection::Connection( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Site( pOriginal, pParent, strName )
{
}

Node::Ptr Connection::copy( Node::Ptr pParent, const std::string& strName ) const
{
    return Node::copy< Connection >(
        boost::dynamic_pointer_cast< const Connection >( shared_from_this() ), pParent, strName );
}

void Connection::load( const format::Node& node )
{
    Site::load( node );

    VERIFY_RTE( node.has_site() && node.site().has_connection() );
    const format::Node::Site::Connection& connection = node.site().connection();

    if( m_pControlPoint = get< Feature_Point >( "width" ); m_pControlPoint )
    {
        m_pControlPoint->setPoint( 0, Point( connection.width, connection.half_height ) );
    }
}

void Connection::save( format::Node& node ) const
{
    format::Node::Site::Connection& connection = *node.mutable_site()->mutable_connection();

    if( m_pControlPoint )
    {
        const Point& pt        = m_pControlPoint->getPoint( 0U );
        connection.width       = pt.x();
        connection.half_height = pt.y();
    }

    Site::save( node );
}

std::string Connection::getStatement() const
{
    return Site::getStatement();
}

Rect Connection::getAABB() const
{
    const Point& pt                    = m_pControlPoint->getPoint( 0U );
    const Float  fWidth                = fabs( Math::quantize< Float >( pt.x(), 1.0 ) );
    const Float  fConnectionHalfHeight = fabs( Math::quantize< Float >( pt.y(), 1.0 ) );

    const Segment first( Point( -fWidth, -fConnectionHalfHeight ), Point( -fWidth, fConnectionHalfHeight ) );
    const Segment second( Point( fWidth, -fConnectionHalfHeight ), Point( fWidth, fConnectionHalfHeight ) );

    const std::vector< Segment > segments{ first, second };
    return CGAL::bbox_2( segments.begin(), segments.end() );
}

void Connection::init()
{
    if( !( m_pControlPoint = get< Feature_Point >( "width" ) ) )
    {
        m_pControlPoint.reset( new Feature_Point( getPtr(), "width" ) );
        m_pControlPoint->init();
        m_pControlPoint->setPoint( 0, Point( 3.0f, 8.0f ) );
        add( m_pControlPoint );
    }

    Site::init();

    const Polygon contour = calculateContour();
    m_pMarkupContour->setPolygon( contour );
    m_pMarkupContourOutline->setPolygon( contour );
}

void Connection::init( const Transform& transform )
{
    Connection::init();
    setTransform( transform );
}

Polygon Connection::calculateContour()
{
    const Point& pt                    = m_pControlPoint->getPoint( 0U );
    const Float  fWidth                = fabs( Math::quantize< Float >( pt.x(), 1.0 ) );
    const Float  fConnectionHalfHeight = fabs( Math::quantize< Float >( pt.y(), 1.0 ) );

    m_firstSegment  = Segment( Point( -fWidth, -fConnectionHalfHeight ), Point( -fWidth, fConnectionHalfHeight ) );
    m_secondSegment = Segment( Point( fWidth, -fConnectionHalfHeight ), Point( fWidth, fConnectionHalfHeight ) );

    Polygon siteContour;
    {
        siteContour.push_back( m_firstSegment[ 0 ] );
        siteContour.push_back( m_firstSegment[ 1 ] );
        siteContour.push_back( Point( -fWidth / 2.0, fConnectionHalfHeight ) );
        siteContour.push_back( Point( 0.0, fConnectionHalfHeight * 1.2 ) );
        siteContour.push_back( Point( fWidth / 2.0, fConnectionHalfHeight ) );
        siteContour.push_back( m_secondSegment[ 1 ] );
        siteContour.push_back( m_secondSegment[ 0 ] );
    }

    return siteContour;
}

bool Connection::task_contour()
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