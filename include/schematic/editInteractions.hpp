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

#ifndef INTERACTIONS_EDIT_17_DEC_202
#define INTERACTIONS_EDIT_17_DEC_202

#include "schematic/cgalSettings.hpp"
#include "schematic/buffer.hpp"
#include "schematic/site.hpp"
#include "schematic/glyph.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>
#include <set>

namespace schematic
{

class EditBase;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class IInteraction : public boost::enable_shared_from_this< IInteraction >
{
public:
    typedef boost::shared_ptr< IInteraction > Ptr;
    virtual ~IInteraction() {}
    virtual void      OnMove( Float x, Float y )                           = 0;
    virtual Node::Ptr GetInteractionNode() const                           = 0;
    virtual void      GetSelection( std::set< IGlyph* >& selection ) const = 0;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class IEditContext
{
public:
    enum ToolType
    {
        eSelect,
        eDraw
    };

    enum ToolMode
    {
        eArea,
        eFeature
    };

    enum SiteType
    {
        eSpace,
        eWall,
        eObject
    };

    virtual ~IEditContext() {}

    virtual void update() = 0;

    virtual bool              interaction_hover( Float x, Float y, IGlyph*& pPoint1, IGlyph*& pPoint2 ) = 0;
    virtual IInteraction::Ptr interaction_start( ToolMode toolMode, Float x, Float y, Float qX, Float qY,
                                                 IGlyph* pHitGlyph, const std::set< IGlyph* >& selection )
        = 0;
    virtual IInteraction::Ptr interaction_draw_point( Float x, Float y, Float qX, Float qY, IGlyph* pPoint1,
                                                      IGlyph* pPoint2, SiteType siteType )
        = 0;
    virtual IInteraction::Ptr interaction_draw_clip( Float x, Float y, Float qX, Float qY, Node::Ptr pNode ) = 0;

    virtual void interaction_end( IInteraction* pInteraction ) = 0;

    virtual IEditContext* getNestedContext( const std::vector< IGlyph* >& candidates ) = 0;
    virtual IEditContext* getParent()                                                  = 0;

    virtual bool          isNodeContext( Node::Ptr pNode ) const = 0;
    virtual IEditContext* getNodeContext( Node::Ptr pNode )      = 0;

    virtual const Origin* getOrigin() const = 0;
    virtual bool canEdit( IGlyph* pGlyph, ToolType toolType, ToolMode toolMode, bool bAllowPoints, bool bAllowSites,
                          bool bAllowConnections ) const
        = 0;

    virtual void              cmd_delete( const std::set< IGlyph* >& selection )                  = 0;
    virtual Node::Ptr         cmd_cut( const std::set< IGlyph* >& selection )                     = 0;
    virtual Node::Ptr         cmd_copy( const std::set< IGlyph* >& selection )                    = 0;
    virtual IInteraction::Ptr cmd_paste( Node::Ptr pPaste, Float x, Float y, Float qX, Float qY ) = 0;
    virtual IInteraction::Ptr cmd_paste( IGlyph* pGlyph, Float x, Float y, Float qX, Float qY )   = 0;
    virtual IInteraction::Ptr cmd_paste( const std::set< IGlyph* >& selection, Float x, Float y, Float qX, Float qY )
        = 0;
    virtual void cmd_rotateLeft( const std::set< IGlyph* >& selection )               = 0;
    virtual void cmd_rotateRight( const std::set< IGlyph* >& selection )              = 0;
    virtual void cmd_flipHorizontally( const std::set< IGlyph* >& selection )         = 0;
    virtual void cmd_flipVertically( const std::set< IGlyph* >& selection )           = 0;
    virtual void cmd_shrink( const std::set< IGlyph* >& selection, double dbAmount )  = 0;
    virtual void cmd_extrude( const std::set< IGlyph* >& selection, double dbAmount ) = 0;
    virtual void cmd_union( const std::set< IGlyph* >& selection )                    = 0;
    virtual void cmd_filter( const std::set< IGlyph* >& selection )                   = 0;
    virtual void cmd_aabb( const std::set< IGlyph* >& selection )                     = 0;
    virtual void cmd_convexHull( const std::set< IGlyph* >& selection )               = 0;
    virtual void cmd_reparent( const std::set< IGlyph* >& selection )                 = 0;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class Interaction : public IInteraction
{
    friend class EditBase;

public:
    using InitialTransformVector = std::vector< Transform >;
    using PointVector            = std::vector< Point >;
    using GlyphSpecVector        = std::vector< GlyphSpec* >;

private:
    Interaction( EditBase& edit, IEditContext::ToolMode toolMode, Float x, Float y, Float qX, Float qY,
                 IGlyph::Ptr pHitGlyph, const IGlyph::PtrSet& selection );

public:
    virtual void      OnMove( Float x, Float y );
    virtual Node::Ptr GetInteractionNode() const;
    virtual void      GetSelection( std::set< IGlyph* >& selection ) const;

private:
    EditBase&                    m_edit;
    const IEditContext::ToolMode m_toolMode;
    Float                        m_startX, m_startY, m_qX, m_qY;

    IGlyph::PtrSet m_selection;

    GlyphSpecVector        m_interactives;
    InitialTransformVector m_initialTransforms, m_initialRelativeTransforms, m_initialTransformInverses;
    PointVector            m_initialOffsets;

    IGlyph::Ptr m_pHitGlyph;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class InteractionToolWrapper : public IInteraction
{
    friend class EditBase;

private:
    InteractionToolWrapper( EditBase& edit, IInteraction::Ptr pWrapped );

public:
    virtual void      OnMove( Float x, Float y );
    virtual Node::Ptr GetInteractionNode() const;
    virtual void      GetSelection( std::set< IGlyph* >& selection ) const {}

private:
    EditBase&         m_edit;
    IInteraction::Ptr m_pToolInteraction;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class Polygon_Interaction : public IInteraction
{
    friend class EditBase;

private:
    Polygon_Interaction( Site& site, Float x, Float y, Float qX, Float qY, std::size_t szIndex );

public:
    virtual void      OnMove( Float x, Float y );
    virtual Node::Ptr GetInteractionNode() const;
    virtual void      GetSelection( std::set< IGlyph* >& selection ) const {}

private:
    Site&       m_site;
    Polygon     m_originalPolygon;
    Float       m_startX, m_startY, m_qX, m_qY;
    std::size_t m_szIndex;

    Transform m_inverseTransform;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
inline IInteraction::Ptr make_interaction_ptr( IEditContext* pEdit, IInteraction* pInteraction )
{
    return IInteraction::Ptr( pInteraction, [ pEdit ]( IInteraction* p ) { pEdit->interaction_end( p ); } );
}

} // namespace schematic

#endif // INTERACTIONS_EDIT_17_DEC_202