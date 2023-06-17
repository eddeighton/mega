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

#ifndef BASE_EDIT_17_DEC_2020
#define BASE_EDIT_17_DEC_2020

#include "schematic/buffer.hpp"
#include "schematic/node.hpp"
#include "schematic/glyph.hpp"
#include "schematic/editInteractions.hpp"
#include "schematic/transform.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <vector>
#include <set>

namespace schematic
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class EditNested;
class EditRoot;

class EditBase : public IEditContext
{
public:
    using Ptr = boost::shared_ptr< EditBase >;

protected:
    using NodeToEditNestedMap = std::map< Node::Ptr, boost::shared_ptr< EditNested > >;
    using GlyphMap            = std::map< const GlyphSpec*, IGlyph::Ptr >;

    EditBase( EditRoot& editRoot, GlyphFactory& glyphFactory, Node::Ptr pNode );

public:
    void onEditted( bool bCommandCompleted );

    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////
    // IEditContext
    virtual void update();
    virtual void updateGlyphs();
    
    virtual void setProperty( Property::PtrCst pProperty, const std::string& strValue );

    virtual bool              interaction_hover( Float x, Float y, IGlyph*& pPoint1, IGlyph*& pPoint2 );
    virtual IInteraction::Ptr interaction_start( ToolMode toolMode, Float x, Float y, Float qX, Float qY,
                                                 IGlyph* pHitGlyph, const std::set< IGlyph* >& selection );
    virtual IInteraction::Ptr interaction_draw_point( Float x, Float y, Float qX, Float qY, IGlyph* pPoint1,
                                                      IGlyph* pPoint2, SiteType siteType );
    virtual IInteraction::Ptr interaction_draw_clip( Float x, Float y, Float qX, Float qY, Node::Ptr pNode );
    virtual void              interaction_end( IInteraction* pInteraction );

    virtual IEditContext* getNestedContext( const std::vector< IGlyph* >& candidates );
    virtual IEditContext* getParent() { return nullptr; }

    virtual bool          isNodeContext( Node::PtrCst pNode ) const { return pNode == m_pNode; }
    virtual bool          isNodeContext( Node::Ptr pNode ) const { return pNode == m_pNode; }
    virtual IEditContext* getNodeContext( Node::Ptr pNode );

    virtual bool canEdit( IGlyph* pGlyph, ToolType toolType, ToolMode toolMode, bool bAllowPoints, bool bAllowSites,
                          bool bAllowConnections ) const;

    virtual const Origin* getOrigin() const { return nullptr; }

    // command handling
    virtual void              cmd_delete( const std::set< IGlyph* >& selection );
    virtual Node::Ptr         cmd_cut( const std::set< IGlyph* >& selection );
    virtual Node::Ptr         cmd_copy( const std::set< IGlyph* >& selection );
    virtual IInteraction::Ptr cmd_paste( Node::Ptr pPaste, Float x, Float y, Float qX, Float qY );
    virtual IInteraction::Ptr cmd_paste( IGlyph* pGlyph, Float x, Float y, Float qX, Float qY );
    virtual IInteraction::Ptr cmd_paste( const std::set< IGlyph* >& selection, Float x, Float y, Float qX, Float qY );
    virtual void              cmd_rotateLeft( const std::set< IGlyph* >& selection );
    virtual void              cmd_rotateRight( const std::set< IGlyph* >& selection );
    virtual void              cmd_flipHorizontally( const std::set< IGlyph* >& selection );
    virtual void              cmd_flipVertically( const std::set< IGlyph* >& selection );
    virtual void              cmd_shrink( const std::set< IGlyph* >& selection, double dbAmount );
    virtual void              cmd_extrude( const std::set< IGlyph* >& selection, double dbAmount );
    virtual void              cmd_union( const std::set< IGlyph* >& selection );
    virtual void              cmd_filter( const std::set< IGlyph* >& selection );
    virtual void              cmd_aabb( const std::set< IGlyph* >& selection );
    virtual void              cmd_convexHull( const std::set< IGlyph* >& selection );
    virtual void              cmd_reparent( const std::set< IGlyph* >& selection );

    ///////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////
    // update handling
    Node::Ptr getNode() const { return m_pNode; }

protected:
    inline Transform calculateInitTransform( const Point& absLocation ) const
    {
        Transform drawTransform = translate( Vector( absLocation.x(), absLocation.y() ) );
        if( const Origin* pOrigin = getOrigin() )
        {
            drawTransform
                = ( pOrigin->getTransform().inverse() * pOrigin->getAbsoluteTransform().inverse() ) * drawTransform;
        }
        return drawTransform;
    }

    virtual void      cmd_delete_impl( const std::set< IGlyph* >& selection );
    IInteraction::Ptr cmd_paste( Node::PtrVector nodes, Float x, Float y, Float qX, Float qY );
    void              cmd_union_impl( const std::vector< Site* >& sites, bool bCreateWallsForHoles );

    virtual IGlyph::Ptr getControlPointGlyph( ControlPoint* pControlPoint ) { return {}; };

    EditRoot&     m_editRoot;
    GlyphFactory& m_glyphFactory;
    Node::Ptr     m_pNode;
    IInteraction* m_pActiveInteraction;

    NodeToEditNestedMap m_glyphMap;

    GlyphMap m_markup;
};

} // namespace schematic

#endif // BASE_EDIT_17_DEC_2020