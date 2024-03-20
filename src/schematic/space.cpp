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

#include "schematic/space.hpp"
#include "schematic/cgalSettings.hpp"
#include "schematic/cgalUtils.hpp"
#include "schematic/file.hpp"
#include "schematic/factory.hpp"
#include "schematic/wall.hpp"
#include "schematic/schematic.hpp"

namespace schematic
{

const std::string& Space::TypeName()
{
    static const std::string strTypeName( "space" );
    return strTypeName;
}

Space::Space( Node::Ptr pParent, const std::string& strName )
    : Site( pParent, strName )
{
}

Space::Space( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Site( pOriginal, pParent, strName )
{
}

Node::Ptr Space::copy( Node::Ptr pParent, const std::string& strName ) const
{
    return Node::copy< Space >( boost::dynamic_pointer_cast< const Space >( shared_from_this() ), pParent, strName );
}

void Space::load( const format::Node& node )
{
    Site::load( node );
    VERIFY_RTE( node.has_site() && node.site().has_space() );

    m_pContour = get< Feature_Contour >( "contour" );
    VERIFY_RTE_MSG( m_pContour, "Space contour not found" );
}

void Space::save( format::Node& node ) const
{
    Site::save( node );
}

std::string Space::getStatement() const
{
    return Site::getStatement();
}

Rect Space::getAABB() const
{
    const Polygon& siteContour = m_pContour->getPolygon();
    return CGAL::bbox_2( siteContour.begin(), siteContour.end() );
}

void Space::init()
{
    if( !m_pInteriorPolygonMarkup.get() )
    {
        m_pInteriorPolygonMarkup.reset( new SimplePolygonMarkup( *this, this, false, eStage_Extrusion ) );
    }
    if( !m_pInnerExteriorPolygonsMarkup.get() )
    {
        m_pInnerExteriorPolygonsMarkup.reset(
            new MultiPolygonMarkup( *this, this, false, eStage_Extrusion ) );
    }

    if( !( m_pContour = get< Feature_Contour >( "contour" ) ) )
    {
        m_pContour = Feature_Contour::Ptr( new Feature_Contour( getPtr(), "contour" ) );
        m_pContour->init();
        m_pContour->set( Utils::getDefaultPolygon() );
        add( m_pContour );
    }

    if( !( m_pWidthProperty = get< Property >( "width" ) ) )
    {
        m_pWidthProperty = Property::Ptr( new Property( getPtr(), "width" ) );
        m_pWidthProperty->init();
        m_pWidthProperty->setStatement( "0.25f" );
        add( m_pWidthProperty );
    }

    Site::init();

    m_pMarkupContour->setPolygon( m_pContour->getPolygon() );
    m_pMarkupContourOutline->setPolygon( m_pContour->getPolygon() );
}

void Space::init( const Transform& transform )
{
    // generate a contour with NO points - so that points can be drawn from scratch
    m_pContour = Feature_Contour::Ptr( new Feature_Contour( shared_from_this(), "contour" ) );
    m_pContour->init();
    add( m_pContour );

    m_pWidthProperty = Property::Ptr( new Property( shared_from_this(), "width" ) );
    m_pWidthProperty->init();
    m_pWidthProperty->setStatement( "0.25f" );
    add( m_pWidthProperty );

    Space::init();

    setTransform( transform );
}

bool Space::task_contour()
{
    bool bModified = false;

    const bool bSubTreeModified = Site::task_contour();

    const Polygon& siteContour = m_pContour->getPolygon();

    // clang-format off
    if( bSubTreeModified || 
        ( siteContour != m_siteContourCache ) || 
        ( m_siteContourTick < getLastModifiedTick() ) || 
        ( m_pWidthProperty && ( m_siteContourTick < m_pWidthProperty->getLastModifiedTick() ) )
        // clang-format on
    )
    {
        m_siteContourTick.update();
        m_siteContourCache = siteContour;
        m_pMarkupContour->setPolygon( siteContour );
        m_pMarkupContourOutline->setPolygon( siteContour );

        m_sitePolygon.clear();
        m_interiorPolygon.clear();
        m_exteriorPolygon.clear();
        m_innerExteriors.clear();
        m_innerExteriorUnions.clear();

        bModified = true;
    }

    return bModified;
}

void Space::task_extrusions()
{
    Kernel::FT wallWidth = 0.25f;
    if( m_pWidthProperty )
    {
        wallWidth = m_pWidthProperty->getValue( 0.25f, m_lastWidthOpt );
    }

    if( !m_sitePolygon.is_empty() )
        return;

    // convert site contour to exact
    m_sitePolygon = Utils::convert< exact::Kernel >( m_siteContourCache );

    if( !m_sitePolygon.is_empty() && m_sitePolygon.is_simple() )
    {
        typedef boost::shared_ptr< exact::Polygon > PolygonPtr;
        typedef std::vector< PolygonPtr >           PolygonPtrVector;

        // calculate interior
        {
            PolygonPtrVector inner_offset_polygons
                = CGAL::create_interior_skeleton_and_offset_polygons_2< exact::Kernel::FT,
                                                                        exact::Polygon,
                                                                        exact::Kernel,
                                                                        exact::Kernel >(
                    wallWidth, m_sitePolygon, ( exact::Kernel() ), ( exact::Kernel() ) );
            if( !inner_offset_polygons.empty() )
            {
                m_interiorPolygon = *inner_offset_polygons.front();
            }
        }

        // calculate exterior
        {
            PolygonPtrVector outer_offset_polygons
                = CGAL::create_exterior_skeleton_and_offset_polygons_2< exact::Kernel::FT,
                                                                        exact::Polygon,
                                                                        exact::Kernel,
                                                                        exact::Kernel >(
                    wallWidth, m_sitePolygon, ( exact::Kernel() ), ( exact::Kernel() ) );
            if( !outer_offset_polygons.empty() )
            {
                m_exteriorPolygon = *outer_offset_polygons.back();
            }
        }
    }

    // bottom up recursion
    m_innerExteriors.clear();
    for( Site::Ptr pSite : BaseType::getElements() )
    {
        if( Space::Ptr pSpace = boost::dynamic_pointer_cast< Space >( pSite ) )
        {
            pSpace->task_extrusions();

            exact::Polygon         poly           = pSpace->getExteriorPolygon();
            const exact::Transform exactTransform = pSpace->getExactTransform();
            if( !poly.is_empty() && poly.is_simple() )
            {
                for( auto& p : poly )
                    p = exactTransform( p );

                if( !poly.is_counterclockwise_oriented() )
                    poly.reverse_orientation();

                m_innerExteriors.push_back( poly );
            }
        }
        else if( Wall::Ptr pWall = boost::dynamic_pointer_cast< Wall >( pSite ) )
        {
            pWall->task_extrusions();

            exact::Polygon         poly           = pWall->getExteriorPolygon();
            const exact::Transform exactTransform = pWall->getExactTransform();
            if( !poly.is_empty() && poly.is_simple() )
            {
                for( auto& p : poly )
                    p = exactTransform( p );

                if( !poly.is_counterclockwise_oriented() )
                    poly.reverse_orientation();

                m_innerExteriors.push_back( poly );
            }
        }
    }

    m_innerExteriorUnions.clear();

    // compute the union of ALL inner exterior contours
    std::vector< exact::Polygon_with_holes > exteriorUnion;
    CGAL::join( m_innerExteriors.begin(), m_innerExteriors.end(), std::back_inserter( exteriorUnion ) );

    for( const exact::Polygon_with_holes& polyWithHole : exteriorUnion )
    {
        if( !polyWithHole.is_unbounded() )
        {
            const exact::Polygon& outer = polyWithHole.outer_boundary();
            if( !outer.is_empty() && outer.is_simple() )
            {
                // clip the exterior to the interior
                std::vector< exact::Polygon_with_holes > clippedExterior;
                CGAL::intersection( outer, m_interiorPolygon, std::back_inserter( clippedExterior ) );

                // gather the outer boundaries of the results
                for( const exact::Polygon_with_holes& clip : clippedExterior )
                {
                    if( !clip.is_unbounded() )
                    {
                        const exact::Polygon& clipOuter = clip.outer_boundary();
                        if( !clipOuter.is_empty() && clipOuter.is_simple() )
                        {
                            m_innerExteriorUnions.push_back( clipOuter );
                        }
                    }
                }
            }
        }
    }

    // update
    {
        m_pInteriorPolygonMarkup->setPolygon( Utils::convert< Kernel >( m_interiorPolygon ) );

        std::vector< Polygon > innerExteriorUnionsInexact;
        for( const exact::Polygon& exactPoly : m_innerExteriorUnions )
        {
            innerExteriorUnionsInexact.push_back( Utils::convert< Kernel >( exactPoly ) );
        }
        m_pInnerExteriorPolygonsMarkup->setPolygons( innerExteriorUnionsInexact );
    }
}

} // namespace schematic