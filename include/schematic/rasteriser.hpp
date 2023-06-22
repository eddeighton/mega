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

#ifdef _WIN32

#else

//#pragma PUSH_GCC_OPTIONS
#pragma diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#pragma diagnostic ignored "-Wdeprecated-enum-float-conversion"
#pragma diagnostic ignored "-Wreturn-local-addr"
#pragma diagnostic ignored "-Wregister"

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

//#pragma POP_GCC_OPTIONS

#endif

#include <vector>

namespace schematic
{

class Rasteriser
{
public:
    using Float = double;

    // typedef agg::pixfmt_bgra32                          PixelFormatType;
    using PixelFormatType  = agg::pixfmt_gray8;
    using ColourType       = PixelFormatType::color_type;
    using RendererBaseType = agg::renderer_base< PixelFormatType >;
    using ScanlineType     = agg::scanline_p8;
    using RasterizerType   = agg::rasterizer_scanline_aa<>;

public:
    Rasteriser( NavBitmap& buffer, bool bClear = true )
        : m_buffer( buffer )
        , m_renderBuffer( buffer.get(), buffer.getWidth(), buffer.getHeight(), buffer.getStride() )
        , m_pixelFormater( m_renderBuffer )
        , m_renderer( m_pixelFormater )
    {
        if( bClear )
            m_renderer.clear( ColourType( 0u ) );
    }

    template < class T >
    void renderPath( T& path, const ColourType& colour, Float fGamma = 0.0f )
    {
        RasterizerType ras;
        ras.gamma( agg::gamma_threshold( fGamma ) );
        ras.add_path( path );
        agg::render_scanlines_aa_solid( ras, m_scanLine, m_renderer, colour );
    }

    template < class T >
    void renderPath( T& path, const ColourType& colour, Float fX, Float fY, Float fGamma = 0.0f )
    {
        agg::trans_affine transform;
        transform *= agg::trans_affine_translation( fX, fY );
        renderPath( agg::conv_transform< T >( path, transform ), colour, fGamma );
    }

    void setPixel( int x, int y, const ColourType& colour ) { m_renderer.copy_pixel( x, y, colour ); }

    inline const ColourType& getPixel( int x, int y ) const { return m_renderer.pixel( x, y ); }

    NavBitmap& getBuffer() { return m_buffer; }

private:
    NavBitmap&            m_buffer;
    agg::rendering_buffer m_renderBuffer;
    PixelFormatType       m_pixelFormater;
    RendererBaseType      m_renderer;
    ScanlineType          m_scanLine;
};

} // namespace schematic

#endif // RASTERISER_21_09_2013