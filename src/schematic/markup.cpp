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

#include "schematic/markup.hpp"

namespace schematic
{

SimplePolygonMarkup::SimplePolygonMarkup( const GlyphSpecProducer& producer,
                                          const GlyphSpec*         pParent,
                                          bool                     bFill,
                                          CompilationStage         compilationStage )
    : m_producer( producer )
    , m_pParent( pParent )
    , m_bFill( bFill )
    , m_compilationStage( compilationStage )
{
}

const GlyphSpec* SimplePolygonMarkup::getParent() const
{
    return m_pParent;
}

bool SimplePolygonMarkup::isPolygonsFilled() const
{
    return m_bFill;
}

bool SimplePolygonMarkup::paint( Painter& painter ) const
{
    if( painter.needsUpdate( m_updateTick ) )
    {
        painter.reset();
        painter.polygon( m_polygon );
        painter.updated();
        return true;
    }
    else
    {
        return false;
    }
}

void SimplePolygonMarkup::reset()
{
    m_polygon.clear();
    m_updateTick.update();
}

void SimplePolygonMarkup::setPolygon( const Polygon& polygon )
{
    m_polygon = polygon;
    m_updateTick.update();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
MultiPolygonMarkup::MultiPolygonMarkup( const GlyphSpecProducer& producer,
                                        const GlyphSpec*         pParent,
                                        bool                     bFill,
                                        CompilationStage         compilationStage )
    : m_producer( producer )
    , m_pParent( pParent )
    , m_bFill( bFill )
    , m_compilationStage( compilationStage )
{
}

const GlyphSpec* MultiPolygonMarkup::getParent() const
{
    return m_pParent;
}

bool MultiPolygonMarkup::isPolygonsFilled() const
{
    return m_bFill;
}

bool MultiPolygonMarkup::paint( Painter& painter ) const
{
    if( painter.needsUpdate( m_updateTick ) )
    {
        painter.reset();
        for( const Polygon& poly : m_polygons )
            painter.polygon( poly );
        painter.updated();
        return true;
    }
    else
    {
        return false;
    }
}

void MultiPolygonMarkup::reset()
{
    m_polygons.clear();
    m_updateTick.update();
}

void MultiPolygonMarkup::setPolygons( const PolygonVector& polygons )
{
    m_polygons = polygons;
    m_updateTick.update();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
MultiPathMarkup::MultiPathMarkup( const GlyphSpecProducer& producer,
                                  const GlyphSpec*         pParent,
                                  CompilationStage         compilationStage )
    : m_producer( producer )
    , m_pParent( pParent )
    , m_compilationStage( compilationStage )
{
}

const GlyphSpec* MultiPathMarkup::getParent() const
{
    return m_pParent;
}

bool MultiPathMarkup::isPolygonsFilled() const
{
    return false;
}

bool MultiPathMarkup::paint( Painter& painter ) const
{
    if( painter.needsUpdate( m_updateTick ) )
    {
        painter.reset();

        for( const Segment& segment : m_segments )
        {
            painter.moveTo( segment[ 0 ] );
            painter.lineTo( segment[ 1 ] );
        }

        painter.updated();
        return true;
    }
    else
    {
        return false;
    }
}

void MultiPathMarkup::reset()
{
    m_updateTick.update();
}

void MultiPathMarkup::set( const std::vector< Segment >& segments )
{
    m_segments = segments;
    m_updateTick.update();
}

} // namespace schematic