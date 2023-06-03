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

#include "schematic/site.hpp"

#include "schematic/markup.hpp"

#include "schematic/space.hpp"
#include "schematic/wall.hpp"
#include "schematic/connection.hpp"
#include "schematic/object.hpp"
#include "schematic/file.hpp"
#include "schematic/factory.hpp"
#include "schematic/schematic.hpp"
#include "schematic/cgalUtils.hpp"

#include "common/compose.hpp"
#include "common/assert_verify.hpp"

#include <algorithm>

namespace schematic
{
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

Site::Site( Node::Ptr pParent, const std::string& strName )
    : BaseType( pParent, strName )
    , m_pSiteParent( pParent )
{
}

Site::Site( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : BaseType( pOriginal, pParent, strName )
    , m_pSiteParent( pParent )
{
    m_transform = pOriginal->m_transform;
}

void Site::load( const format::Node& node )
{
    Node::load( node );
    VERIFY_RTE( node.has_site() );
    const format::Node::Site& size = node.site();
    m_transform = fromFormat( size.transform );
}

void Site::save( format::Node& node ) const
{
    format::Node::Site& site = *node.mutable_site();
    toFormat( getTransform(), site.transform );
    Node::save( node );
}

std::string Site::getStatement() const
{
    std::ostringstream os;
    {
        os << "Site: " << getName();
    }
    return os.str();
}

void Site::init()
{
    if( !m_pMarkupContour.get() )
    {
        m_pMarkupContour.reset(
            new SimplePolygonMarkup( *this, this, true, Schematic::eStage_Site ) );
    }

    if( !m_pMarkupContourOutline.get() )
    {
        m_pMarkupContourOutline.reset( new SimplePolygonMarkup(
            *this, this, false, Schematic::eStage_SiteContour ) );
    }

    m_strLabelText.clear();
    /*m_properties.clear();
    {
        std::ostringstream os;
        os << Node::getName();
        {
            for_each_recursive(
                generics::collectIfConvert(
                    m_properties, Node::ConvertPtrType< Property >(), Node::ConvertPtrType< Property >() ),
                Node::ConvertPtrType< Site >() );

            for( Property::Ptr pProperty : m_properties )
            {
                os << "\n" << pProperty->getName() << ": " << pProperty->getValue();
            }
        }
        m_strLabelText = os.str();
    }*/

    if( !m_pLabel.get() )
    {
        m_pLabel.reset( new TextImpl( *this, this, m_strLabelText, Point( 0.0, 0.0 ), Schematic::eStage_Site ) );
    }

    BaseType::init();
}

bool Site::task_contour()
{
    bool bModified = false;

    m_transformCache = inexactToExact( m_transform );

    for( Site::Ptr pChildSite : BaseType::getElements() )
    {
        if( pChildSite->task_contour() )
            bModified = true;
    }

    return bModified;
}

const GlyphSpec* Site::getParent() const
{
    if( Node::Ptr pParentNode = m_pSiteParent.lock() )
    {
        if( Site::Ptr pParentSite = boost::dynamic_pointer_cast< Site >( pParentNode ) )
        {
            return pParentSite.get();
        }
    }
    return nullptr;
}

CompilationStage Site::getCompilationStage() const
{
    return Schematic::eStage_Site;
}

exact::Transform Site::getAbsoluteExactTransform() const
{
    exact::Transform transform( CGAL::IDENTITY );
    Site::PtrCst     pIter = boost::dynamic_pointer_cast< const Site >( getPtr() );
    while( pIter )
    {
        transform = pIter->getExactTransform() * transform;
        pIter     = boost::dynamic_pointer_cast< const Site >( pIter->Node::getParent() );
    }
    return transform;
}

void Site::setTransform( const Transform& transform )
{
    if( m_transform != transform )
    {
        m_transform = transform;
        setModified();
    }
}

void Site::cmd_rotateLeft( const Rect& transformBounds )
{
    m_transform = rotateLeft( m_transform, transformBounds );
    setModified();
}

void Site::cmd_rotateRight( const Rect& transformBounds )
{
    m_transform = rotateRight( m_transform, transformBounds );
    setModified();
}

void Site::cmd_flipHorizontally( const Rect& transformBounds )
{
    m_transform = flipHorizontally( m_transform, transformBounds );
    setModified();
}

void Site::cmd_flipVertically( const Rect& transformBounds )
{
    m_transform = flipVertically( m_transform, transformBounds );
    setModified();
}

void Site::cmd_shrink( double dbAmount )
{
    if( Feature_Contour::Ptr pContour = getContour() )
    {
        exact::Polygon contour = Utils::convert< exact::Kernel >( pContour->getPolygon() );

        if( !contour.is_empty() && contour.is_simple() )
        {
            if( !contour.is_counterclockwise_oriented() )
            {
                std::reverse( contour.begin(), contour.end() );
            }

            typedef boost::shared_ptr< exact::Polygon > PolygonPtr;
            typedef std::vector< PolygonPtr >           PolygonPtrVector;

            PolygonPtrVector extrudedContourPolygons
                = CGAL::create_interior_skeleton_and_offset_polygons_2< exact::Kernel::FT,
                                                                        exact::Polygon,
                                                                        exact::Kernel,
                                                                        exact::Kernel >(
                    dbAmount, contour, ( exact::Kernel() ), ( exact::Kernel() ) );
            if( !extrudedContourPolygons.empty() )
            {
                exact::Polygon extrudedContour = *extrudedContourPolygons.back();
                if( !extrudedContour.is_counterclockwise_oriented() )
                {
                    std::reverse( extrudedContour.begin(), extrudedContour.end() );
                }
                pContour->set( Utils::convert< schematic::Kernel >( extrudedContour ) );
            }
        }
    }
}

void Site::cmd_extrude( double dbAmount )
{
    if( Feature_Contour::Ptr pContour = getContour() )
    {
        exact::Polygon contour = Utils::convert< exact::Kernel >( pContour->getPolygon() );

        if( !contour.is_empty() && contour.is_simple() )
        {
            if( !contour.is_counterclockwise_oriented() )
            {
                std::reverse( contour.begin(), contour.end() );
            }

            typedef boost::shared_ptr< exact::Polygon > PolygonPtr;
            typedef std::vector< PolygonPtr >           PolygonPtrVector;

            PolygonPtrVector extrudedContourPolygons
                = CGAL::create_exterior_skeleton_and_offset_polygons_2< exact::Kernel::FT,
                                                                        exact::Polygon,
                                                                        exact::Kernel,
                                                                        exact::Kernel >(
                    dbAmount, contour, ( exact::Kernel() ), ( exact::Kernel() ) );
            if( !extrudedContourPolygons.empty() )
            {
                exact::Polygon extrudedContour = *extrudedContourPolygons.back();
                if( !extrudedContour.is_counterclockwise_oriented() )
                {
                    std::reverse( extrudedContour.begin(), extrudedContour.end() );
                }
                pContour->set( Utils::convert< schematic::Kernel >( extrudedContour ) );
            }
        }
    }
}

void Site::cmd_filter()
{
    if( Feature_Contour::Ptr pContour = getContour() )
    {
        exact::Polygon contour = Utils::convert< exact::Kernel >( pContour->getPolygon() );

        if( !contour.is_empty() && contour.is_simple() )
        {
            if( !contour.is_counterclockwise_oriented() )
            {
                std::reverse( contour.begin(), contour.end() );
            }
        }

        pContour->set( Utils::convert< schematic::Kernel >( Utils::filterPolygon( contour ) ) );
    }
}

void Site::cmd_aabb()
{
    if( Feature_Contour::Ptr pContour = getContour() )
    {
        std::vector< Point > points;
        for( Site::Ptr pSite : getSites() )
        {
            if( Feature_Contour::Ptr pChildContour = pSite->getContour() )
            {
                Polygon poly = pChildContour->getPolygon();
                for( Point& pt : poly )
                {
                    points.push_back( pSite->getTransform()( pt ) );
                }
            }
        }

        const Rect transformBounds = CGAL::bbox_2( points.begin(), points.end() );

        Polygon contour;
        {
            contour.push_back( transformBounds[ 0 ] );
            contour.push_back( transformBounds[ 1 ] );
            contour.push_back( transformBounds[ 2 ] );
            contour.push_back( transformBounds[ 3 ] );
        }

        if( !contour.is_empty() && contour.is_simple() )
        {
            if( !contour.is_counterclockwise_oriented() )
            {
                std::reverse( contour.begin(), contour.end() );
            }
        }

        pContour->set( contour );
    }
}

void Site::cmd_convexHull()
{
    if( Feature_Contour::Ptr pContour = getContour() )
    {
        std::vector< Point > points;
        for( Site::Ptr pSite : getSites() )
        {
            if( Feature_Contour::Ptr pChildContour = pSite->getContour() )
            {
                Polygon poly = pChildContour->getPolygon();
                for( Point& pt : poly )
                {
                    points.push_back( pSite->getTransform()( pt ) );
                }
            }
        }

        Polygon contour;
        CGAL::ch_graham_andrew( points.begin(), points.end(), std::back_inserter( contour ) );

        if( !contour.is_empty() && contour.is_simple() )
        {
            if( !contour.is_counterclockwise_oriented() )
            {
                std::reverse( contour.begin(), contour.end() );
            }
        }

        pContour->set( contour );
    }
}

} // namespace schematic