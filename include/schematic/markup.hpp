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

#ifndef MARKUP_IMPL_01_DEC_2020
#define MARKUP_IMPL_01_DEC_2020

#include "schematic/cgalSettings.hpp"
#include "schematic/geometry.hpp"
#include "schematic/glyphSpec.hpp"

namespace schematic
{

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
template < class T >
class ControlPointCallback : public ControlPoint
{
    T&                  m_callback;
    ControlPoint::Index m_id;
    CompilationStage    m_compilationStage;

public:
    ControlPointCallback( T& callback, std::size_t id, CompilationStage compilationStage )
        : m_callback( callback )
        , m_id( id )
        , m_compilationStage( compilationStage )
    {
    }

    // GlyphSpec
    virtual const GlyphSpec*         getParent() const { return m_callback.getParent( m_id ); }
    virtual const GlyphSpecProducer* getProducer() const { return &m_callback; }
    virtual bool                     canEdit() const { return true; }
    virtual CompilationStage         getCompilationStage() const { return m_compilationStage; }

    // ControlPoint
    virtual ControlPoint::Index getIndex() const { return m_id; }
    virtual const Point&        getPoint() const { return m_callback.getPoint( m_id ); }
    virtual void                setPoint( const Point& point ) { m_callback.setPoint( m_id, point ); }

    // other
    void setIndex( ControlPoint::Index iIndex ) { m_id = iIndex; }
    T&   getCallback() const { return m_callback; }
};

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
class TextImpl : public MarkupText
{
public:
    TextImpl( const GlyphSpecProducer& producer, const GlyphSpec* pParent, const std::string& strText,
              const Point& point, CompilationStage compilationStage, MarkupText::TextType type = eUnimportant )
        : m_producer( producer )
        , m_pParent( pParent )
        , m_strText( strText )
        , m_point( point )
        , m_compilationStage( compilationStage )
        , m_type( type )
    {
    }

    virtual const GlyphSpec*         getParent() const { return m_pParent; }
    virtual const GlyphSpecProducer* getProducer() const { return &m_producer; }
    virtual const std::string&       getText() const { return m_strText; }
    virtual const Point&             getPoint() const { return m_point; }
    virtual CompilationStage         getCompilationStage() const { return m_compilationStage; }
    virtual MarkupText::TextType     getType() const { return m_type; }

private:
    const GlyphSpecProducer&   m_producer;
    const GlyphSpec*           m_pParent;
    const std::string&         m_strText;
    Point                      m_point;
    CompilationStage           m_compilationStage;
    const MarkupText::TextType m_type;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class SimplePolygonMarkup : public MarkupPolygonGroup
{
public:
    SimplePolygonMarkup( const GlyphSpecProducer& producer,
                         const GlyphSpec*         pParent,
                         bool                     bFill,
                         CompilationStage         compilationStage );

    virtual const GlyphSpec*         getParent() const;
    virtual const GlyphSpecProducer* getProducer() const { return &m_producer; }
    virtual CompilationStage         getCompilationStage() const { return m_compilationStage; }
    virtual bool                     isPolygonsFilled() const;
    virtual bool                     paint( Painter& painter ) const;
    void                             reset();
    void                             setPolygon( const Polygon& polygon );

private:
    const GlyphSpecProducer& m_producer;
    const GlyphSpec*         m_pParent;
    Timing::UpdateTick       m_updateTick;
    Polygon                  m_polygon;
    bool                     m_bFill;
    CompilationStage         m_compilationStage;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class MultiPolygonMarkup : public MarkupPolygonGroup
{
public:
    using PolygonVector = std::vector< Polygon >;

    MultiPolygonMarkup( const GlyphSpecProducer& producer,
                        const GlyphSpec*         pParent,
                        bool                     bFill,
                        CompilationStage         compilationStage );

    virtual const GlyphSpec*         getParent() const;
    virtual const GlyphSpecProducer* getProducer() const { return &m_producer; }
    virtual CompilationStage         getCompilationStage() const { return m_compilationStage; }
    virtual bool                     isPolygonsFilled() const;
    virtual bool                     paint( Painter& painter ) const;
    void                             reset();
    void                             setPolygons( const PolygonVector& polygons );

private:
    const GlyphSpecProducer& m_producer;
    const GlyphSpec*         m_pParent;
    Timing::UpdateTick       m_updateTick;
    PolygonVector            m_polygons;
    bool                     m_bFill;
    CompilationStage         m_compilationStage;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class MultiPathMarkup : public MarkupPolygonGroup
{
public:
    MultiPathMarkup( const GlyphSpecProducer& producer,
                     const GlyphSpec*         pParent,
                     CompilationStage         compilationStage );

    virtual const GlyphSpec*         getParent() const;
    virtual const GlyphSpecProducer* getProducer() const { return &m_producer; }
    virtual CompilationStage         getCompilationStage() const { return m_compilationStage; }
    virtual bool                     isPolygonsFilled() const;
    virtual bool                     paint( Painter& painter ) const;
    void                             reset();
    void                             set( const std::vector< Segment >& segments );

private:
    const GlyphSpecProducer& m_producer;
    const GlyphSpec*         m_pParent;
    Timing::UpdateTick       m_updateTick;
    std::vector< Segment >   m_segments;
    CompilationStage         m_compilationStage;
};

} // namespace schematic

#endif // MARKUP_IMPL_01_DEC_2020