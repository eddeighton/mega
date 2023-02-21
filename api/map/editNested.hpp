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

#ifndef NESTED_EDIT_17_DEC_2020
#define NESTED_EDIT_17_DEC_2020

#include "map/editBase.hpp"

#include "common/tick.hpp"

namespace map
{
class EditRoot;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class EditNested : public EditBase
{
    using GlyphMap = std::map< const GlyphSpec*, IGlyph::Ptr >;

    struct FeatureGlyphSet
    {
        Timing::UpdateTick tick;
        GlyphMap           glyphs;

        inline bool contains( IGlyph::Ptr pGlyph ) const
        {
            for( GlyphMap::const_iterator i = glyphs.begin(), iEnd = glyphs.end(); i != iEnd; ++i )
            {
                if( i->second == pGlyph )
                    return true;
            }
            return false;
        }
    };
    using FeatureGlyphMap = std::map< Feature::Ptr, FeatureGlyphSet >;

public:
    using Ptr = boost::shared_ptr< EditNested >;

    EditNested( EditRoot& editRoot, IEditContext& parentContext, Site::Ptr pSite, GlyphFactory& glyphFactory );

    virtual void updateGlyphs();
    virtual bool interaction_hover( Float x, Float y, IGlyph*& pPoint1, IGlyph*& pPoint2 );

    virtual IEditContext* getParent() { return &m_parent; }
    virtual const Origin* getOrigin() const;

    void        matchFeatures();
    bool        owns( IGlyph* pGlyph ) const;
    bool        owns( const GlyphSpec* pGlyphSpec ) const;
    IGlyph::Ptr getMainGlyph() const { return m_pMainGlyph; }

protected:
    virtual void cmd_delete_impl( const std::set< IGlyph* >& selection );
    IGlyph::Ptr  createControlPointGlyph( ControlPoint* pControlPoint );

    Site::Ptr       m_pSite;
    IEditContext&   m_parent;
    FeatureGlyphMap m_features;
    GlyphMap        m_glyphs;
    IGlyph::Ptr     m_pMainGlyph;
};

} // namespace map

#endif // NESTED_EDIT_17_DEC_2020/