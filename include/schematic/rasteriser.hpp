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

#ifndef RASTERISER_21_09_2013
#define RASTERISER_21_09_2013

#include "common/optimisation.hpp"

#include "schematic/buffer.hpp"


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
#pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wno-deprecated-enum-enum-conversion"
// #pragma GCC diagnostic ignored "-Wno-deprecated-enum-float-conversion"
// #pragma GCC diagnostic ignored "-Wno-return-local-addr"
// #pragma GCC diagnostic ignored "-Wno-register"

#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "agg_basics.h"
// #include "agg_pixfmt_rgba.h"
#include "agg_pixfmt_gray.h"
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_renderer_scanline.h"
#include "agg_path_storage.h"
#include "agg_conv_stroke.h"
#include "agg_trans_affine.h"
#include "agg_conv_transform.h"

#pragma GCC diagnostic pop
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

#include <vector>

namespace schematic
{

class Rasteriser
{
public:
    using Float = double;

    using PixelFormatType   = agg::pixfmt_gray8;
    using ColourType        = PixelFormatType::color_type;
    using RendererBaseType  = agg::renderer_base< PixelFormatType >;
    using ScanlineType      = agg::scanline_p8;
    using PolygonRasterizer = agg::rasterizer_scanline_aa<>;

    static_assert( std::is_same< ColourType, agg::gray8 >::value );

    using Int = int;

public:
    Rasteriser( MonoBitmap& buffer, bool bClear = true )
        : m_buffer( buffer )
        , m_renderBuffer( buffer.get(), buffer.getWidth(), buffer.getHeight(), buffer.getStride() )
        , m_pixelFormater( m_renderBuffer )
        , m_renderer( m_pixelFormater )
    {
        if( bClear )
            m_renderer.clear( ColourType( 0u ) );
    }

    inline void render( PolygonRasterizer& rasterizer, const ColourType& colour )
    {
        agg::render_scanlines_aa_solid( rasterizer, m_scanLine, m_renderer, colour );
    }
    inline void        setPixel( Int x, Int y, const ColourType& colour ) { m_renderer.copy_pixel( x, y, colour ); }
    inline ColourType  getPixel( Int x, Int y ) const { return m_renderer.pixel( x, y ); }
    inline MonoBitmap& getBuffer() { return m_buffer; }

    void line( Int p1x, Int p1y, Int p2x, Int p2y, const ColourType& colour );

private:
    MonoBitmap&           m_buffer;
    agg::rendering_buffer m_renderBuffer;
    PixelFormatType       m_pixelFormater;
    RendererBaseType      m_renderer;
    ScanlineType          m_scanLine;
};

} // namespace schematic

#endif // RASTERISER_21_09_2013