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

#include "schematic/object.hpp"
#include "schematic/cgalUtils.hpp"
#include "schematic/factory.hpp"

#include "common/assert_verify.hpp"

namespace schematic
{

const std::string& Object::TypeName()
{
    static const std::string strTypeName( "object" );
    return strTypeName;
}

Object::Object( Node::Ptr pParent, const std::string& strName )
    : Site( pParent, strName )
{
}

Object::Object( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Site( pOriginal, pParent, strName )
{
}

Node::Ptr Object::copy( Node::Ptr pParent, const std::string& strName ) const
{
    return Node::copy< Object >( boost::dynamic_pointer_cast< const Object >( shared_from_this() ), pParent, strName );
}

void Object::load( const format::Site& site )
{
    Site::load( site );

    VERIFY_RTE( site.has_object() );
    const format::Site::Object& object = site.object();

    // site contour
    m_pContour->set( formatPolygonFromPath( object.contour ) );
}

void Object::save( format::Site& site ) const
{
    format::Site::Object& object = *site.mutable_object();

    // site contour
    formatPolygonToPath( m_pContour->getPolygon(), object.contour );

    Site::save( site );
}

std::string Object::getStatement() const
{
    return Site::getStatement();
}

Rect Object::getAABB() const
{
    const Polygon& siteContour = m_pContour->getPolygon();
    return CGAL::bbox_2( siteContour.begin(), siteContour.end() );
}

void Object::init()
{
    if( !( m_pContour = get< Feature_Contour >( "contour" ) ) )
    {
        m_pContour = Feature_Contour::Ptr( new Feature_Contour( getPtr(), "contour" ) );
        m_pContour->init();
        m_pContour->set( Utils::getDefaultPolygon() );
        add( m_pContour );
    }

    Site::init();

    m_pMarkupContour->setPolygon( m_pContour->getPolygon() );
    m_pMarkupContourOutline->setPolygon( m_pContour->getPolygon() );
}

void Object::init( const Transform& transform )
{
    m_pContour = Feature_Contour::Ptr( new Feature_Contour( shared_from_this(), "contour" ) );
    m_pContour->init();
    add( m_pContour );

    Object::init();

    setTransform( transform );
}

bool Object::task_contour()
{
    bool bModified = false;

    const bool bSubTreeModified = Site::task_contour();

    const Polygon& siteContour = m_pContour->getPolygon();

    if( bSubTreeModified || ( siteContour != m_siteContourCache ) || ( m_siteContourTick < getLastModifiedTick() ) )
    {
        m_siteContourTick.update();
        m_siteContourCache = siteContour;
        m_pMarkupContour->setPolygon( siteContour );
        m_pMarkupContourOutline->setPolygon( siteContour );

        // m_sitePolygon.clear();
        // m_interiorPolygon.clear();
        // m_exteriorPolygon.clear();
        // m_innerExteriors.clear();
        // m_innerExteriorUnions.clear();

        bModified = true;
    }

    return bModified;
}
} // namespace schematic