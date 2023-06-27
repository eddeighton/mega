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

#ifndef SPACE_01_DEC_2020
#define SPACE_01_DEC_2020

#include "format/format.hpp"

#include "schematic/site.hpp"

namespace schematic
{

class Space : public Site, public boost::enable_shared_from_this< Space >
{
public:
    using Ptr     = boost::shared_ptr< Space >;
    using WeakPtr = boost::weak_ptr< Space >;
    using PtrCst  = boost::shared_ptr< const Space >;

    static const std::string& TypeName();

    Space( Node::Ptr pParent, const std::string& strName );
    Space( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName );

    virtual Node::Ptr    copy( Node::Ptr pParent, const std::string& strName ) const;
    virtual Node::PtrCst getPtr() const { return shared_from_this(); }
    virtual Node::Ptr    getPtr() { return shared_from_this(); }

    virtual void load( const format::Node& node );
    virtual void save( format::Node& node ) const;

    virtual std::string getStatement() const;

    void init();
    void init( const Transform& transform );

    // schematic tasks
    virtual bool task_contour();
    virtual void task_extrusions();

    // GlyphSpecProducer
    virtual void getMarkupPolygonGroups( MarkupPolygonGroup::List& polyGroups )
    {
        Site::getMarkupPolygonGroups( polyGroups );
        if( m_pInteriorPolygonMarkup.get() )
            polyGroups.push_back( m_pInteriorPolygonMarkup.get() );
        if( m_pInnerExteriorPolygonsMarkup.get() )
            polyGroups.push_back( m_pInnerExteriorPolygonsMarkup.get() );
    }

    virtual Feature_Contour::Ptr getContour() const { return m_pContour; }
    virtual Rect                 getAABB() const;

    const exact::Polygon&                getSitePolygon() const { return m_sitePolygon; }
    const exact::Polygon&                getInteriorPolygon() const { return m_interiorPolygon; }
    const std::vector< exact::Polygon >& getInnerExteriorUnions() const { return m_innerExteriorUnions; }

protected:
    const exact::Polygon& getExteriorPolygon() const { return m_exteriorPolygon; }

    Feature_Contour::Ptr   m_pContour;
    Property::Ptr          m_pWidthProperty;
    std::optional< float > m_lastWidthOpt;

    Polygon            m_siteContourCache;
    Timing::UpdateTick m_siteContourTick;

    exact::Polygon                m_sitePolygon;
    exact::Polygon                m_exteriorPolygon;
    exact::Polygon                m_interiorPolygon;
    std::vector< exact::Polygon > m_innerExteriors;
    std::vector< exact::Polygon > m_innerExteriorUnions;

    std::unique_ptr< SimplePolygonMarkup > m_pInteriorPolygonMarkup;
    std::unique_ptr< MultiPolygonMarkup >  m_pInnerExteriorPolygonsMarkup;
};

} // namespace schematic

#endif // SPACE_01_DEC_2020