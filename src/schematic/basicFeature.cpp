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

#include "schematic/basicFeature.hpp"

#include "schematic/factory.hpp"
#include "schematic/site.hpp"
#include "schematic/schematic.hpp"
#include "schematic/cgalUtils.hpp"

#include <boost/optional.hpp>

#include "common/assert_verify.hpp"
#include "common/stl.hpp"

namespace schematic
{

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
const std::string& Feature::TypeName()
{
    static const std::string strTypeName( "feature" );
    return strTypeName;
}
void Feature::init()
{
    Node::init();
}
Node::Ptr Feature::copy( Node::Ptr pParent, const std::string& strName ) const
{
    return Node::copy< Feature >( shared_from_this(), pParent, strName );
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
const std::string& Feature_Point::TypeName()
{
    static const std::string strTypeName( "point" );
    return strTypeName;
}
Feature_Point::Feature_Point( Node::Ptr pParent, const std::string& strName )
    : Feature( pParent, strName )
    , m_point( *this, 0, Schematic::eStage_SiteContour )
    , m_ptOrigin( 0.0f, 0.0f )
{
    m_controlPointSet.insert( &m_point );
}
Feature_Point::Feature_Point( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Feature( pOriginal, pParent, strName )
    , m_point( *this, 0, Schematic::eStage_SiteContour )
    , m_ptOrigin( pOriginal->m_ptOrigin )
{
    m_controlPointSet.insert( &m_point );
}

Node::Ptr Feature_Point::copy( Node::Ptr pParent, const std::string& strName ) const
{
    return Node::copy< Feature_Point >(
        boost::dynamic_pointer_cast< const Feature_Point >( shared_from_this() ), pParent, strName );
}

std::string Feature_Point::getStatement() const
{
    std::ostringstream os;
    {
        THROW_RTE( "TODO" );
        /*Ed::Shorthand sh;
        {
            Ed::OShorthandStream ossh( sh );
            ossh << m_ptOrigin;
        }
        os << sh;*/
    }
    return os.str();
}

const GlyphSpec* Feature_Point::getParent( ControlPoint::Index id ) const
{
    if( Feature_Point::Ptr pParent = boost::dynamic_pointer_cast< Feature_Point >( m_pParent.lock() ) )
        return &pParent->m_point;
    else if( Site::Ptr pParent = boost::dynamic_pointer_cast< Site >( m_pParent.lock() ) )
        return pParent.get();
    else
        return nullptr;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
const std::string& Feature_Contour::TypeName()
{
    static const std::string strTypeName( "contour" );
    return strTypeName;
}
Feature_Contour::Feature_Contour( Node::Ptr pParent, const std::string& strName )
    : Feature( pParent, strName )
{
}

Feature_Contour::Feature_Contour( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Feature( pOriginal, pParent, strName )
    , m_polygon( pOriginal->m_polygon )
{
    recalculateControlPoints();
}

void Feature_Contour::init()
{
    Feature::init();
}

Node::Ptr Feature_Contour::copy( Node::Ptr pParent, const std::string& strName ) const
{
    return Node::copy< Feature_Contour >(
        boost::dynamic_pointer_cast< const Feature_Contour >( shared_from_this() ), pParent, strName );
}

Feature_Contour::~Feature_Contour()
{
    generics::deleteAndClear( m_points );
}

std::string Feature_Contour::getStatement() const
{
    std::ostringstream os;
    {
        THROW_RTE( "TODO" );
        /*
        Ed::Shorthand sh;
        {
            Ed::OShorthandStream ossh( sh );
            ossh << m_polygon;
        }
        os << sh;*/
    }
    return os.str();
}

const GlyphSpec* Feature_Contour::getParent( ControlPoint::Index id ) const
{
    if( Feature_Point::Ptr pParent = boost::dynamic_pointer_cast< Feature_Point >( m_pParent.lock() ) )
        return &pParent->m_point;
    else if( Site::Ptr pParent = boost::dynamic_pointer_cast< Site >( m_pParent.lock() ) )
        return pParent.get();
    else
        return 0u;
}

const Point& Feature_Contour::getPoint( ControlPoint::Index id ) const
{
    VERIFY_RTE( id >= 0 && id < m_polygon.size() );
    return m_polygon[ id ];
}

void Feature_Contour::setPoint( ControlPoint::Index id, const Point& point )
{
    if( id >= 0 && id < m_polygon.size() )
    {
        if( m_polygon[ id ] != point )
        {
            m_polygon[ id ] = point;
            setModified();
        }
    }
}

void Feature_Contour::set( const Polygon& shape )
{
    if( !( m_polygon.size() == shape.size() ) || !std::equal( m_polygon.begin(), m_polygon.end(), shape.begin() ) )
    {
        const bool bReallocate = !( m_polygon.size() == shape.size() );

        m_polygon = shape;

        for( auto i = m_polygon.begin(), iEnd = m_polygon.end(); i != iEnd; ++i )
        {
            *i = Point(
                Map_FloorAverage()( CGAL::to_double( i->x() ) ), Map_FloorAverage()( CGAL::to_double( i->y() ) ) );
        }

        if( bReallocate )
        {
            recalculateControlPoints();
        }
        setModified();
    }
}

void Feature_Contour::recalculateControlPoints()
{
    generics::deleteAndClear( m_points );

    {
        ControlPoint::Index id = 0U;
        for( auto i = m_polygon.begin(), iEnd = m_polygon.end(); i != iEnd; ++i, ++id )
        {
            m_points.push_back( new PointType( *this, id, Schematic::eStage_SiteContour ) );
        }
        m_controlPointSet.clear();
        m_controlPointSet.insert( m_points.begin(), m_points.end() );
    }
}

bool Feature_Contour::cmd_delete( const std::vector< const GlyphSpec* >& selection )
{
    std::vector< ControlPoint::Index > removals;

    {
        ControlPoint::Index iCounter = m_points.size() - 1;
        for( auto i = m_points.rbegin(), iEnd = m_points.rend(); i != iEnd; ++i, --iCounter )
        {
            for( const GlyphSpec* pGlyphSpec : selection )
            {
                if( pGlyphSpec == *i )
                {
                    removals.push_back( iCounter );
                }
            }
        }
    }

    {
        for( ControlPoint::Index i : removals )
        {
            ControlPoint* pControlPoint = *( m_points.begin() + i );

            m_polygon.erase( m_polygon.begin() + i );
            m_controlPointSet.erase( pControlPoint );
            m_points.erase( m_points.begin() + i );

            delete pControlPoint;
        }
    }

    {
        ControlPoint::Index sz = 0U;
        for( PointType* pPoint : m_points )
            pPoint->setIndex( sz++ );
    }

    if( !removals.empty() )
    {
        setModified();
    }

    return !removals.empty();
}
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/*
const std::string& Feature_ContourPoint::TypeName()
{
    static const std::string strTypeName( "contourpoint" );
    return strTypeName;
}
Feature_ContourPoint::Feature_ContourPoint( Feature_Contour::Ptr pParent, const std::string& strName )
    :   Feature( pParent, strName ),
        m_point( *this, 0 ),
        m_pContour( pParent ),
        m_uiContourPointIndex( 0u ),
        m_fRatio( 0.0f )
{
}
Feature_ContourPoint::Feature_ContourPoint( PtrCst pOriginal, Feature_Contour::Ptr pParent, const std::string& strName )
    :   Feature( pOriginal, pParent, strName ),
        m_point( *this, 0 ),
        m_pContour( pParent ),
        m_uiContourPointIndex( pOriginal->m_uiContourPointIndex ),
        m_fRatio( pOriginal->m_fRatio )
{

}

Node::Ptr Feature_ContourPoint::copy( Node::Ptr pParent, const std::string& strName ) const
{
    Feature_Contour::Ptr pFeatureContourParent = boost::dynamic_pointer_cast< Feature_Contour >( pParent );
    VERIFY_RTE( pFeatureContourParent );
    return Node::copy< Feature_ContourPoint >(
        boost::dynamic_pointer_cast< const Feature_ContourPoint >( shared_from_this() ), pFeatureContourParent, strName
);
}

void Feature_ContourPoint::load( Loader& loader )
{
    Node::load( getPtr(), loader );
    THROW_RTE( "TODO" );
}

void Feature_ContourPoint::save( Storer& storer ) const
{
    Node::save( storer );
    THROW_RTE( "TODO" );
}
void Feature_ContourPoint::load( Factory& factory, const Ed::Node& node )
{
    Node::load( shared_from_this(), factory, node );

    if( boost::optional< const Ed::Shorthand& > shOpt = node.getShorty() )
    {
        Ed::IShorthandStream is( shOpt.get() );
        is >> m_uiContourPointIndex >> m_fRatio;
    }
}

void Feature_ContourPoint::save( Ed::Node& node ) const
{
    node.statement.addTag( Ed::Identifier( TypeName() ) );

    Node::save( node );

    if( !node.statement.shorthand ) node.statement.shorthand = Ed::Shorthand();
    Ed::OShorthandStream os( node.statement.shorthand.get() );
     os << m_uiContourPointIndex << m_fRatio;
}

std::string Feature_ContourPoint::getStatement() const
{
    std::ostringstream os;
    {
        Ed::Shorthand sh;
        {
            Ed::OShorthandStream ossh( sh );
            ossh << m_uiContourPointIndex << m_fRatio;
        }
        os << sh;
    }
    return os.str();
}

const GlyphSpec* Feature_ContourPoint::getParent( int id ) const
{
    if( Feature_Contour::Ptr pParent = boost::dynamic_pointer_cast< Feature_Contour >( m_pParent.lock() ) )
    {
        const GlyphSpec* pParentRoot = pParent->getRootControlPoint();
        ASSERT( pParentRoot );
        return pParentRoot;
    }
    else
    {
        ASSERT( false );
        return 0u;
    }
}

Float Feature_ContourPoint::getX( int id ) const
{
    THROW_RTE( "TODO" );
    Feature_Contour::PtrCst pParent = m_pContour.lock();
    ASSERT( pParent );
    Float fX = 0.0f;
    const Polygon& contour = pParent->getPolygon();
    if( contour.size() )
    {
        unsigned int uiIndex        = m_uiContourPointIndex % contour.size();
        unsigned int uiIndexNext    = ( m_uiContourPointIndex + 1u ) % contour.size();
        fX = contour[ uiIndex ].x + ( contour[ uiIndexNext ].x - contour[ uiIndex ].x ) * m_fRatio;
        fX -= contour[0u].x;
    }

    return fX;
}
Float Feature_ContourPoint::getY( int id ) const
{
    THROW_RTE( "TODO" );
    Feature_Contour::PtrCst pParent = m_pContour.lock();
    ASSERT( pParent );
    Float fY = 0.0f;
    const Polygon& contour = pParent->getPolygon();
    if( contour.size() )
    {
        unsigned int uiIndex        = m_uiContourPointIndex % contour.size();
        unsigned int uiIndexNext    = ( m_uiContourPointIndex + 1u ) % contour.size();
        fY = contour[ uiIndex ].y + ( contour[ uiIndexNext ].y - contour[ uiIndex ].y ) * m_fRatio;
        fY -= contour[0u].y;
    }

    return fY;
}
void Feature_ContourPoint::set( int id, Float fX, Float fY )
{
    THROW_RTE( "TODO" );

    Feature_Contour::PtrCst pParent = m_pContour.lock();
    ASSERT( pParent );
    const Point& origin = pParent->getPolygon()[0];

    const Point ptInteractPos( fX + origin.x, fY + origin.y );
    SegmentIDRatioPointTuple result =
        findClosestPointOnContour( pParent->getPolygon(), ptInteractPos );
    if( m_fRatio != std::get< 1 >( result ) ||  m_uiContourPointIndex != std::get< 0 >( result ) )
    {
        m_fRatio = std::get< 1 >( result );
        m_uiContourPointIndex = std::get< 0 >( result );
        setModified();
    }
}*/

} // namespace schematic