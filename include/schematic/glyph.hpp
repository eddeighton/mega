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

#ifndef GLYPH_16_09_2013
#define GLYPH_16_09_2013

#include "node.hpp"
#include "glyphSpec.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <vector>
#include <set>

namespace schematic
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class IGlyph : public boost::enable_shared_from_this< IGlyph >
{
public:
    using Ptr       = boost::shared_ptr< IGlyph >;
    using PtrVector = std::vector< Ptr >;
    using PtrSet    = std::set< Ptr >;
    IGlyph( const GlyphSpec* pGlyphSpec, Ptr pParent )
        : m_pGlyphSpec( pGlyphSpec )
        , m_pParent( pParent )
    {
    }
    virtual ~IGlyph()         = default;
    virtual void     update() = 0;
    const GlyphSpec* getGlyphSpec() const { return m_pGlyphSpec; }
    Ptr              getParent() const { return m_pParent; }

protected:
    const GlyphSpec* m_pGlyphSpec;
    Ptr              m_pParent;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class GlyphControlPoint : public IGlyph
{
public:
    GlyphControlPoint( ControlPoint* pControlPoint, IGlyph::Ptr pParent )
        : IGlyph( pControlPoint, pParent )
    {
    }
    const ControlPoint* getControlPoint() const { return dynamic_cast< const ControlPoint* >( m_pGlyphSpec ); }
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class GlyphOrigin : public IGlyph
{
    friend class SpaceGlyphs;
    friend class Interaction;

public:
    GlyphOrigin( Origin* pOrigin, IGlyph::Ptr pParent )
        : IGlyph( pOrigin, pParent )
    {
    }

    const Origin* getOrigin() const { return dynamic_cast< const Origin* >( m_pGlyphSpec ); }
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class GlyphPolygonGroup : public IGlyph
{
public:
    GlyphPolygonGroup( MarkupPolygonGroup* pMarkupPolygonGroup, IGlyph::Ptr pParent )
        : IGlyph( pMarkupPolygonGroup, pParent )
    {
    }
    const MarkupPolygonGroup* getMarkupPolygonGroup() const
    {
        return dynamic_cast< const MarkupPolygonGroup* >( m_pGlyphSpec );
    }
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class GlyphText : public IGlyph
{
public:
    GlyphText( MarkupText* pMarkupText, IGlyph::Ptr pParent )
        : IGlyph( pMarkupText, pParent )
    {
    }
    const MarkupText* getMarkupText() const { return dynamic_cast< const MarkupText* >( m_pGlyphSpec ); }
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class GlyphImage : public IGlyph
{
public:
    GlyphImage( ImageSpec* pImageSpec, IGlyph::Ptr pParent )
        : IGlyph( pImageSpec, pParent )
    {
    }
    const ImageSpec* getImageSpec() const { return dynamic_cast< const ImageSpec* >( m_pGlyphSpec ); }
};
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class GlyphFactory
{
public:
    virtual IGlyph::Ptr createControlPoint( ControlPoint* pControlPoint, IGlyph::Ptr pParent )                   = 0;
    virtual IGlyph::Ptr createOrigin( Origin* pOrigin, IGlyph::Ptr pParent )                                     = 0;
    virtual IGlyph::Ptr createMarkupPolygonGroup( MarkupPolygonGroup* pMarkupPolygonGroup, IGlyph::Ptr pParent ) = 0;
    virtual IGlyph::Ptr createMarkupText( MarkupText* pMarkupText, IGlyph::Ptr pParent )                         = 0;
    virtual IGlyph::Ptr createImage( ImageSpec* pImage, IGlyph::Ptr pParent )                                    = 0;

    virtual void onEditted( bool bCommandCompleted ) = 0;
};

} // namespace schematic

#endif // GLYPH_16_09_2013