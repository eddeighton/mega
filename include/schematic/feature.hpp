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

#ifndef FEATURE_18_09_2013
#define FEATURE_18_09_2013

#include "schematic/cgalSettings.hpp"
#include "schematic/property.hpp"
#include "schematic/glyphSpec.hpp"
#include "schematic/glyphSpecProducer.hpp"
#include "schematic/markup.hpp"

#include <boost/shared_ptr.hpp>

#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>

namespace schematic
{

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class Feature : public GlyphSpecProducer, public boost::enable_shared_from_this< Feature >
{
public:
    using Ptr       = boost::shared_ptr< Feature >;
    using PtrCst    = boost::shared_ptr< const Feature >;
    using PtrSet    = std::set< Ptr >;
    using PtrVector = std::vector< Ptr >;
    using PtrMap    = std::map< std::string, Ptr >;

    static const std::string& TypeName();
    Feature( Node::Ptr pParent, const std::string& strName )
        : GlyphSpecProducer( pParent, strName )
    {
    }
    Feature( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
        : GlyphSpecProducer( pOriginal, pParent, strName )
    {
    }
    virtual Node::PtrCst getPtr() const { return shared_from_this(); }
    virtual Node::Ptr    getPtr() { return shared_from_this(); }
    virtual Node::Ptr    copy( Node::Ptr pParent, const std::string& strName ) const;
    virtual void         init();
    virtual void         load( const format::Node& node );
    virtual void         save( format::Node& node ) const;
    virtual std::string  getStatement() const { return ""; }
};

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class Feature_Point : public Feature
{
public:
    using Ptr    = boost::shared_ptr< Feature_Point >;
    using PtrCst = boost::shared_ptr< const Feature_Point >;

    static const std::string& TypeName();
    Feature_Point( Node::Ptr pParent, const std::string& strName );
    Feature_Point( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName );
    virtual Node::Ptr   copy( Node::Ptr pParent, const std::string& strName ) const;
    virtual void        init();
    virtual void        load( const format::Node& node );
    virtual void        save( format::Node& node ) const;
    virtual std::string getStatement() const;

    // ControlPointCallback
    const GlyphSpec* getParent( ControlPoint::Index id ) const;
    const Point&     getPoint( ControlPoint::Index ) const { return m_ptOrigin; }
    void             setPoint( ControlPoint::Index, const Point& point )
    {
        if( m_ptOrigin != point )
        {
            m_ptOrigin = point;
            setModified();
        }
    }

    Point                                 m_ptOrigin;
    ControlPointCallback< Feature_Point > m_point;
};

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class Feature_Contour : public Feature
{
    using PointType   = ControlPointCallback< Feature_Contour >;
    using PointVector = std::vector< PointType* >;

public:
    using Ptr        = boost::shared_ptr< Feature_Contour >;
    using WeakPtr    = boost::weak_ptr< Feature_Contour >;
    using WeakPtrCst = boost::weak_ptr< const Feature_Contour >;
    using PtrCst     = boost::shared_ptr< const Feature_Contour >;

    static const std::string& TypeName();
    Feature_Contour( Node::Ptr pParent, const std::string& strName );
    Feature_Contour( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName );
    virtual ~Feature_Contour();

    // node
    virtual void        init();
    virtual void        load( const format::Node& node );
    virtual void        save( format::Node& node ) const;
    virtual Node::Ptr   copy( Node::Ptr pParent, const std::string& strName ) const;
    virtual std::string getStatement() const;

    // ControlPointCallback
    const GlyphSpec* getParent( ControlPoint::Index id ) const;
    const Point&     getPoint( ControlPoint::Index id ) const;
    void             setPoint( ControlPoint::Index id, const Point& point );
    void             set( const Polygon& shape );

    virtual bool cmd_delete( const std::vector< const GlyphSpec* >& selection );

    const Polygon& getPolygon() const { return m_polygon; }

private:
    void recalculateControlPoints();

    Polygon     m_polygon;
    PointVector m_points;
};

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class Feature_Pin : public Feature
{
public:
    using Ptr    = boost::shared_ptr< Feature_Pin >;
    using PtrCst = boost::shared_ptr< const Feature_Pin >;

    static const std::string& TypeName();
    Feature_Pin( Node::Ptr pParent, const std::string& strName );
    Feature_Pin( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName );
    virtual Node::Ptr   copy( Node::Ptr pParent, const std::string& strName ) const;
    virtual void        init();
    virtual void        load( const format::Node& node );
    virtual void        save( format::Node& node ) const;
    virtual std::string getStatement() const;

    // ControlPointCallback
    const GlyphSpec* getParent( ControlPoint::Index id ) const;
    const Point&     getPoint( ControlPoint::Index ) const { return m_ptOrigin; }
    void             setPoint( ControlPoint::Index, const Point& point )
    {
        if( m_ptOrigin != point )
        {
            m_ptOrigin = point;
            setModified();
        }
    }

    std::optional< exact::Transform > getAbsoluteExactTransform() const;

    Point                               m_ptOrigin;
    ControlPointCallback< Feature_Pin > m_point;
};

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class Feature_LineSegment : public Feature
{
public:
    using Ptr    = boost::shared_ptr< Feature_LineSegment >;
    using PtrCst = boost::shared_ptr< const Feature_LineSegment >;

    static const std::string& TypeName();
    Feature_LineSegment( Node::Ptr pParent, const std::string& strName );
    Feature_LineSegment( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName );
    virtual Node::Ptr   copy( Node::Ptr pParent, const std::string& strName ) const;
    virtual void        init();
    virtual void        load( const format::Node& node );
    virtual void        save( format::Node& node ) const;
    virtual std::string getStatement() const;

    // ControlPointCallback
    const GlyphSpec* getParent( ControlPoint::Index id ) const;
    inline const Point&     getPoint( ControlPoint::Index index ) const
    {
        switch( index )
        {
            case 0:
            {
                return m_ptStart;
            }
            break;
            case 1:
            {
                return m_ptEnd;
            }
            break;
            default:
            {
                THROW_RTE( "Invalid control point index" );
            }
        }
    }
    inline void setPoint( ControlPoint::Index index, const Point& point )
    {
        switch( index )
        {
            case 0:
            {
                if( m_ptStart != point )
                {
                    m_ptStart = point;
                    setModified();
                }
            }
            break;
            case 1:
            {
                if( m_ptEnd != point )
                {
                    m_ptEnd = point;
                    setModified();
                }
            }
            break;
            default:
            {
                THROW_RTE( "Invalid control point index" );
            }
        }
    }

    inline Segment getSegment() const { return Segment{ m_ptStart, m_ptEnd }; }

    Point                                       m_ptStart;
    Point                                       m_ptEnd;
    ControlPointCallback< Feature_LineSegment > m_start;
    ControlPointCallback< Feature_LineSegment > m_end;
};
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/*
class Feature_ContourPoint : public Feature
{
public:
    typedef boost::shared_ptr< Feature_ContourPoint > Ptr;
    typedef boost::shared_ptr< const Feature_ContourPoint > PtrCst;

    static const std::string& TypeName();
    Feature_ContourPoint( Feature_Contour::Ptr pParent, const std::string& strName );
    Feature_ContourPoint( PtrCst pOriginal, Feature_Contour::Ptr pParent, const std::string& strName );
    virtual Node::Ptr copy( Node::Ptr pParent, const std::string& strName ) const;
    //virtual void load( Loader& loader );
    //virtual void save( Storer& storer ) const;
    virtual std::string getStatement() const;

    const unsigned int getContourPointIndex() const { return m_uiContourPointIndex; }
    Float getRatio() const { return m_fRatio; }

    //ControlPointCallback
    virtual const GlyphSpec* getParent( ControlPoint::Index id id ) const;
    virtual Float getX( ControlPoint::Index id id ) const;
    virtual Float getY( ControlPoint::Index id id ) const;
    virtual void set( ControlPoint::Index id id, Float fX, Float fY );

protected:
    unsigned int m_uiContourPointIndex;
    Float m_fRatio;

    ControlPointCallback< Feature_ContourPoint > m_point;
    Feature_Contour::WeakPtrCst m_pContour;
};
*/

} // namespace schematic

#endif // FEATURE_18_09_2013