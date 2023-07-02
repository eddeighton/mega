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

#ifndef GLYPHSPEC_18_09_2013
#define GLYPHSPEC_18_09_2013

#include "schematic/compilation_stage.hpp"
#include "schematic/cgalSettings.hpp"
#include "schematic/compilation_stage.hpp"
#include "schematic/buffer.hpp"

#include "schematic/analysis/edge_mask.hpp"

#include "common/tick.hpp"

#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>

namespace schematic
{

class Factory;
class Node;

class GlyphSpecProducer;

class GlyphSpec
{
public:
    virtual ~GlyphSpec()                                         = default;
    virtual const GlyphSpec*         getParent() const           = 0;
    virtual bool                     canEdit() const             = 0;
    virtual CompilationStage         getCompilationStage() const = 0;
    virtual const GlyphSpecProducer* getProducer() const         = 0;
};

class Painter
{
public:
    virtual ~Painter() = default;

    virtual bool needsUpdate( const Timing::UpdateTick& updateTick ) = 0;
    virtual void updated()                                           = 0;

    virtual void reset()                                                   = 0;
    virtual void style( const std::string& strStyle )                      = 0;
    virtual void moveTo( const Point& pt )                                 = 0;
    virtual void lineTo( const Point& pt )                                 = 0;
    virtual void closePath()                                               = 0;
    virtual void polygon( const Polygon& polygon )                         = 0;
    virtual void polygonWithHoles( const Polygon_with_holes& polygon )     = 0;
    virtual void segment( const Segment& from, exact::EdgeMask::Set mask ) = 0;
};

class MarkupPolygonGroup : public GlyphSpec
{
public:
    using List = std::list< MarkupPolygonGroup* >;

    virtual bool        isPolygonsFilled() const        = 0;
    virtual bool        isMultiSegment() const          = 0;
    virtual const char* polygonType() const             = 0;
    virtual bool        paint( Painter& painter ) const = 0;
    virtual bool        canEdit() const { return false; }
};

class MarkupText : public GlyphSpec
{
public:
    using List = std::list< MarkupText* >;
    enum TextType
    {
        eImportant,
        eUnimportant
    };
    virtual const std::string& getText() const  = 0;
    virtual TextType           getType() const  = 0;
    virtual const Point&       getPoint() const = 0;
    virtual bool               canEdit() const { return false; }
};

class ControlPoint : public GlyphSpec
{
public:
    using Set   = std::set< ControlPoint* >;
    using Index = std::size_t;

    virtual Index        getIndex() const               = 0;
    virtual const Point& getPoint() const               = 0;
    virtual void         setPoint( const Point& point ) = 0;
};

class Origin : public GlyphSpec
{
public:
    Origin()
        : m_transform( CGAL::IDENTITY )
    {
    }

    virtual const MarkupPolygonGroup* getMarkupContour() const { return nullptr; }
    virtual const Transform&          getTransform() const { return m_transform; }
    virtual void                      setTransform( const Transform& transform ) { m_transform = transform; }

    virtual Transform getAbsoluteTransform() const;
    virtual Transform getRelativeTransform( const Node* pParent ) const;

protected:
    Transform m_transform;
};

class ImageSpec : public GlyphSpec
{
public:
    using List = std::list< ImageSpec* >;

    virtual const Vector&     getOffset() const  = 0;
    virtual int               getScaling() const = 0;
    virtual const MonoBitmap& getBuffer() const  = 0;
};

} // namespace schematic

#endif // GLYPHSPEC_18_09_2013