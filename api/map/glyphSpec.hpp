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

#include "map/transform.hpp"
#include "map/buffer.hpp"
#include "map/cgalSettings.hpp"

#include "common/tick.hpp"

#include <boost/shared_ptr.hpp>

#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>

namespace Parser
{
struct EdNode;
}

namespace map
{

class Factory;
class Node;

using CompilationStage = int;

class GlyphSpecProducer;

class GlyphSpec
{
public:
    virtual ~GlyphSpec() {}
    virtual const GlyphSpec*         getParent() const           = 0;
    virtual bool                     canEdit() const             = 0;
    virtual CompilationStage         getCompilationStage() const = 0;
    virtual const GlyphSpecProducer* getProducer() const         = 0;
};

class Painter
{
public:
    virtual ~Painter(){};

    virtual bool needsUpdate( const Timing::UpdateTick& updateTick ) = 0;
    virtual void updated()                                           = 0;

    virtual void reset()                           = 0;
    virtual void moveTo( const Point& pt )         = 0;
    virtual void lineTo( const Point& pt )         = 0;
    virtual void closePath()                       = 0;
    virtual void polygon( const Polygon& polygon ) = 0;
};

class MarkupPolygonGroup : public GlyphSpec
{
public:
    typedef std::list< MarkupPolygonGroup* > List;

    virtual bool isPolygonsFilled() const        = 0;
    virtual bool paint( Painter& painter ) const = 0;

    virtual bool canEdit() const { return false; }
};

class MarkupText : public GlyphSpec
{
public:
    typedef std::list< MarkupText* > List;
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
    typedef std::set< ControlPoint* > Set;
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

/*
class ImageSpec : public GlyphSpec
{
public:
    virtual const Point& getPoint() const = 0;
    virtual const Vector& getOffsetX() const = 0;
    virtual NavBitmap::Ptr getBuffer() const = 0;
};
*/
} // namespace map

#endif // GLYPHSPEC_18_09_2013