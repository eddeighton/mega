
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

#ifndef GUARD_2023_June_06_viewConfig
#define GUARD_2023_June_06_viewConfig

#include "schematic/analysis/edge_mask.hpp"

#include <QColor>

#include <bitset>
#include <memory>
#include <array>
#include <optional>

namespace editor
{

class ViewConfig
{
public:
    using Ptr = std::shared_ptr< ViewConfig >;

    // visibility handling
    using GlyphVisibilityConfig = std::bitset< 16 >;
    enum GlyphVisibility
    {
        eGlyphVis_Text,
        eGlyphVis_Points,
        eGlyphVis_Sites,
        eGlyphVis_Connections,
        TOTAL_GLYPH_SETTINGS
    };

    enum MaskSetting
    {
        eMaskIgnor,
        eMaskExclude,
        eMaskNormal,
        eMaskHighlight
    };

    using MaskVisibility = std::array< MaskSetting, exact::EdgeMask::TOTAL_MASK_TYPES >;

    enum ShowType
    {
        eShowNone,
        eShowNormal,
        eShowHighlight,
        TOTAL_SHOW_TYPES
    };

    inline ViewConfig()
    {
        m_glyphVisibility.set( eGlyphVis_Text );
        m_glyphVisibility.set( eGlyphVis_Points );
        m_glyphVisibility.set( eGlyphVis_Sites );
        m_glyphVisibility.set( eGlyphVis_Connections );

        for( auto& m : m_maskVisibility )
            m = eMaskIgnor;
    }

    inline std::optional< QColor > getPolyTypeColour( const std::string& strStyle ) const
    {
        if( strStyle == "Corridor" )
        {
            return QColor( 0, 255, 255, 125 );
        }
        else if( strStyle == "Window" )
        {
            return QColor( 255, 0, 0, 125 );
        }
        else
        {
            return {};
        }
    }

    inline bool        isGlyphVisible( GlyphVisibility glyphType ) const { return m_glyphVisibility.test( glyphType ); }
    inline void        setGlyphVisible( GlyphVisibility glyphType ) { m_glyphVisibility.set( glyphType ); }
    inline void        resetGlyphVisible( GlyphVisibility glyphType ) { m_glyphVisibility.reset( glyphType ); }
    inline MaskSetting getMaskSetting( exact::EdgeMask::Type maskType ) const { return m_maskVisibility[ maskType ]; }
    inline void        setMaskSetting( exact::EdgeMask::Type maskType, MaskSetting setting )
    {
        m_maskVisibility[ maskType ] = setting;
        switch( setting )
        {
            case eMaskIgnor:
            {
                m_maskInclude.reset( maskType );
                m_maskHighlight.reset( maskType );
                m_maskExclude.reset( maskType );
            }
            break;
            case eMaskExclude:
            {
                m_maskInclude.reset( maskType );
                m_maskHighlight.reset( maskType );
                m_maskExclude.set( maskType );
            }
            break;
            case eMaskNormal:
            {
                m_maskInclude.set( maskType );
                m_maskHighlight.reset( maskType );
                m_maskExclude.reset( maskType );
            }
            break;
            case eMaskHighlight:
            {
                m_maskInclude.reset( maskType );
                m_maskHighlight.set( maskType );
                m_maskExclude.reset( maskType );
            }
            break;
        }
    }

    inline ShowType showSegment( exact::EdgeMask::Set mask ) const
    {
        const exact::EdgeMask::Set exclusion = mask & m_maskExclude;
        const exact::EdgeMask::Set inclusion = mask & m_maskInclude;
        const exact::EdgeMask::Set highlight = mask & m_maskHighlight;

        // if the exclude mask matches anything then exclude
        if( exclusion.any() )
        {
            return eShowNone;
        }
        // otherwise highlight if anything is set to highlight
        else if( highlight.any() )
        {
            return eShowHighlight;
        }
        // otherwise include if anything set to include
        else if( inclusion.any() )
        {
            return eShowNormal;
        }
        // by default ignor
        else
        {
            return eShowNone;
        }
    }

private:
    GlyphVisibilityConfig m_glyphVisibility;
    MaskVisibility        m_maskVisibility;
    exact::EdgeMask::Set  m_maskInclude, m_maskHighlight, m_maskExclude;
};
} // namespace editor

#endif // GUARD_2023_June_06_viewConfig
