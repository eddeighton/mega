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

#include "schematic/wall.hpp"
#include "schematic/cgalUtils.hpp"
#include "schematic/factory.hpp"

namespace schematic
{

const std::string& Wall::TypeName()
{
    static const std::string strTypeName( "wall" );
    return strTypeName;
}

Wall::Wall( Node::Ptr pParent, const std::string& strName )
    : Site( pParent, strName )
{
}

Wall::Wall( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Site( pOriginal, pParent, strName )
{
}

Node::Ptr Wall::copy( Node::Ptr pParent, const std::string& strName ) const
{
    return Node::copy< Wall >( boost::dynamic_pointer_cast< const Wall >( shared_from_this() ), pParent, strName );
}

void Wall::load( const format::Node& node )
{
    Site::load( node );
    VERIFY_RTE( node.has_site() && node.site().has_wall() );
    const format::Node::Site::Wall& wall = node.site().wall();
    m_pContour = get< Feature_Contour >( "contour" );
    VERIFY_RTE_MSG( m_pContour, "Wall contour not found" );
}

void Wall::save( format::Node& node ) const
{
    format::Node::Site::Wall& wall = *node.mutable_site()->mutable_wall();
    Site::save( node );
}

std::string Wall::getStatement() const
{
    return Site::getStatement();
}

Rect Wall::getAABB() const
{
    const Polygon& siteContour = m_pContour->getPolygon();
    return CGAL::bbox_2( siteContour.begin(), siteContour.end() );
}

void Wall::init()
{
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
        m_pWidthProperty->setStatement("0.25f");
        add( m_pWidthProperty );
    }

    Site::init();

    m_pMarkupContour->setPolygon( m_pContour->getPolygon() );
    m_pMarkupContourOutline->setPolygon( m_pContour->getPolygon() );
}

void Wall::init( const Transform& transform )
{
    m_pContour = Feature_Contour::Ptr( new Feature_Contour( shared_from_this(), "contour" ) );
    m_pContour->init();
    add( m_pContour );

    m_pWidthProperty = Property::Ptr( new Property( shared_from_this(), "width" ) );
    m_pWidthProperty->init();
    m_pWidthProperty->setStatement( "0.25f" );
    add( m_pWidthProperty );

    Wall::init();

    setTransform( transform );
}

bool Wall::task_contour()
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
        m_exteriorPolygon.clear();

        bModified = true;
    }

    return bModified;
}

void Wall::task_extrusions()
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

        // calculate exterior
        {
            PolygonPtrVector outer_offset_polygons
                = CGAL::create_exterior_skeleton_and_offset_polygons_2< exact::Kernel::FT, exact::Polygon,
                                                                        exact::Kernel, exact::Kernel >(
                    wallWidth, m_sitePolygon, ( exact::Kernel() ), ( exact::Kernel() ) );
            if( !outer_offset_polygons.empty() )
            {
                m_exteriorPolygon = *outer_offset_polygons.back();
            }
        }
    }
}

} // namespace schematic