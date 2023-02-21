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

#ifndef SITE_13_09_2013
#define SITE_13_09_2013

#include "format/format.hpp"

#include "map/cgalSettings.hpp"
#include "map/geometry.hpp"
#include "map/glyphSpec.hpp"
#include "map/glyphSpecProducer.hpp"
#include "map/markup.hpp"
#include "map/basicFeature.hpp"
#include "map/container.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <list>
#include <map>
#include <string>
#include <vector>

namespace map
{

class Factory;
class Site;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class Site : public Container< Site, GlyphSpecProducer >, public Origin
{
    Site& operator=( const Site& );

public:
    using BaseType = Container< Site, GlyphSpecProducer >;

    using Ptr         = boost::shared_ptr< Site >;
    using PtrCst      = boost::shared_ptr< const Site >;
    using WeakPtr     = boost::weak_ptr< Site >;
    using PtrSet      = std::set< Ptr >;
    using PtrList     = std::list< Ptr >;
    using PtrVector   = std::vector< Ptr >;
    using WeakPtrList = std::list< WeakPtr >;
    using PtrMap      = std::map< Ptr, Ptr >;

    Site( Node::Ptr pParent, const std::string& strName );
    Site( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName );

    virtual void load( const format::Site& site );
    virtual void save( format::Site& site ) const;

    virtual std::string getStatement() const;

    virtual void init();
    virtual void init( const Transform& transform ) = 0;

    // GlyphSpec
    virtual bool                     canEdit() const { return true; }
    virtual const std::string&       getName() const { return Node::getName(); }
    virtual const GlyphSpec*         getParent() const;
    virtual CompilationStage         getCompilationStage() const;
    virtual const GlyphSpecProducer* getProducer() const { return this; }

    // origin transform
    virtual void                      setTransform( const Transform& transform );
    virtual const MarkupPolygonGroup* getMarkupContour() const { return m_pMarkupContour.get(); }

    // GlyphSpecProducer
    virtual void getMarkupTexts( MarkupText::List& text )
    {
        if( m_pLabel.get() )
            text.push_back( m_pLabel.get() );
    }
    virtual void getMarkupPolygonGroups( MarkupPolygonGroup::List& polyGroups )
    {
        if( m_pMarkupContourOutline.get() )
            polyGroups.push_back( m_pMarkupContourOutline.get() );
    }

    // spaces
    const Site::PtrVector& getSites() const { return BaseType::getElements(); }

    virtual bool task_contour();

    const exact::Transform& getExactTransform() const { return m_transformCache; }
    exact::Transform        getAbsoluteExactTransform() const;

    // cmds
    void cmd_rotateLeft( const Rect& transformBounds );
    void cmd_rotateRight( const Rect& transformBounds );
    void cmd_flipHorizontally( const Rect& transformBounds );
    void cmd_flipVertically( const Rect& transformBounds );
    void cmd_shrink( double dbAmount );
    void cmd_extrude( double dbAmount );
    void cmd_filter();
    void cmd_aabb();
    void cmd_convexHull();

    // can return optional Feature_Contour::Ptr
    virtual Feature_Contour::Ptr getContour() const = 0;

    // MUST return site AABB
    virtual Rect getAABB() const = 0;

protected:
    exact::Transform m_transformCache;

    using PropertyVector = std::vector< Property::Ptr >;

    Node::WeakPtr  m_pSiteParent;
    PropertyVector m_properties;

    std::string m_strLabelText;

    std::unique_ptr< SimplePolygonMarkup > m_pMarkupContour;
    std::unique_ptr< SimplePolygonMarkup > m_pMarkupContourOutline;
    std::unique_ptr< TextImpl >            m_pLabel;
};

} // namespace map

#endif // mission_13_09_2013
