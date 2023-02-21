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

#include "map/editBase.hpp"

#include "map/editNested.hpp"
#include "map/editRoot.hpp"
#include "map/file.hpp"
#include "map/clip.hpp"
#include "map/space.hpp"
#include "map/wall.hpp"
#include "map/connection.hpp"
#include "map/object.hpp"
#include "map/cgalUtils.hpp"
#include "map/property.hpp"
#include "map/dataBitmap.hpp"
#include "map/factory.hpp"

#include "common/assert_verify.hpp"
#include "common/rounding.hpp"

#include "boost/filesystem.hpp"

#include <sstream>
#include <map>
#include <iomanip>

namespace map
{

EditBase::EditBase( EditRoot& editRoot, GlyphFactory& glyphFactory, Node::Ptr pNode )
    : m_editRoot( editRoot )
    , m_glyphFactory( glyphFactory )
    , m_pNode( pNode )
    , m_pActiveInteraction( 0u )
{
}

void EditBase::onEditted( bool bCommandCompleted )
{
    m_glyphFactory.onEditted( bCommandCompleted );
}

Lock& EditBase::getNodeStructureLock() const
{
    return m_editRoot.getNodeStructureLock();
}
Lock& EditBase::getNodeDataLock() const
{
    return m_editRoot.getNodeDataLock();
}
Lock& EditBase::getSiteContourLock() const
{
    return m_editRoot.getSiteContourLock();
}

bool EditBase::interaction_hover( Float x, Float y, IGlyph*& pPoint1, IGlyph*& pPoint2 )
{
    return false;
}

IInteraction::Ptr EditBase::interaction_start( ToolMode toolMode, Float x, Float y, Float qX, Float qY,
                                               IGlyph* pHitGlyph, const std::set< IGlyph* >& selection )
{
    WriterLock lock1( getNodeStructureLock() );
    WriterLock lock2( getNodeDataLock() );

    ASSERT( !m_pActiveInteraction );

    IGlyph::PtrSet selectionShared;
    for( std::set< IGlyph* >::const_iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
    {
        IGlyph* pGlyph = *i;
        selectionShared.insert( pGlyph->shared_from_this() );
    }
    IGlyph::Ptr pHit     = pHitGlyph ? pHitGlyph->shared_from_this() : IGlyph::Ptr();
    m_pActiveInteraction = new Interaction( *this, toolMode, x, y, qX, qY, pHit, selectionShared );
    return make_interaction_ptr( this, m_pActiveInteraction );
}

IInteraction::Ptr EditBase::interaction_draw_point( Float x, Float y, Float qX, Float qY, IGlyph* pPoint1,
                                                    IGlyph* pPoint2, SiteType siteType )
{
    ASSERT( !m_pActiveInteraction );

    IGlyph::Ptr pHit1 = pPoint1 ? pPoint1->shared_from_this() : IGlyph::Ptr();
    IGlyph::Ptr pHit2 = pPoint2 ? pPoint2->shared_from_this() : IGlyph::Ptr();

    x = Math::quantize_roundUp( x, qX );
    y = Math::quantize_roundUp( y, qY );

    if( pHit1 && pHit2 )
    {
        WriterLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );

        using CtrlPoint = ControlPointCallback< Feature_Contour >;

        const CtrlPoint* pControlPoint1 = dynamic_cast< const CtrlPoint* >( pHit1->getGlyphSpec() );
        const CtrlPoint* pControlPoint2 = dynamic_cast< const CtrlPoint* >( pHit2->getGlyphSpec() );

        VERIFY_RTE( pControlPoint1 && pControlPoint2 );

        Feature_Contour& featureContour = pControlPoint1->getCallback();

        const std::size_t szIndex = pControlPoint1->getIndex() + 1U;

        Site::Ptr pSite = boost::dynamic_pointer_cast< Site >( featureContour.Node::getParent() );
        VERIFY_RTE( pSite );

        IInteraction::Ptr pToolInteraction( new Polygon_Interaction( *pSite, x, y, qX, qY, szIndex ) );
        onEditted( false );
        m_pActiveInteraction = new InteractionToolWrapper( *this, pToolInteraction );
        return make_interaction_ptr( this, m_pActiveInteraction );
    }
    else
    {
        Site::Ptr pNewNode;
        {
            WriterLock lock1( getNodeStructureLock() );
            WriterLock lock2( getNodeDataLock() );

            switch( siteType )
            {
                case eSpace:
                    pNewNode.reset( new Space( m_pNode, m_pNode->generateNewNodeName( "space" ) ) );
                    break;
                case eWall:
                    pNewNode.reset( new Wall( m_pNode, m_pNode->generateNewNodeName( "wall" ) ) );
                    break;
                case eObject:
                    pNewNode.reset( new Object( m_pNode, m_pNode->generateNewNodeName( "object" ) ) );
                    break;
                default:
                    THROW_RTE( "Unknown site type" );
            }

            // create new area
            pNewNode->init( calculateInitTransform( Point( x, y ) ) );

            m_pNode->add( pNewNode );
        }

        update();

        {
            WriterLock lock1( getNodeStructureLock() );
            WriterLock lock2( getNodeDataLock() );

            EditBase* pNewEdit = dynamic_cast< EditBase* >( getNodeContext( pNewNode ) );
            VERIFY_RTE( pNewEdit );

            IInteraction::Ptr pToolInteraction( new Polygon_Interaction( *pNewNode, x, y, qX, qY, 0U ) );
            pNewEdit->m_pActiveInteraction = new InteractionToolWrapper( *pNewEdit, pToolInteraction );
            pNewEdit->onEditted( false );
            return make_interaction_ptr( pNewEdit, pNewEdit->m_pActiveInteraction );
        }
    }
}

IInteraction::Ptr EditBase::interaction_draw_clip( Float x, Float y, Float qX, Float qY, Node::Ptr pNode )
{
    ASSERT( !m_pActiveInteraction );

    x = Math::quantize_roundUp( x, qX );
    y = Math::quantize_roundUp( y, qY );

    // generate space name
    Node::PtrSet newNodes;

    {
        WriterLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );

        const Transform drawTransform = calculateInitTransform( Point( x, y ) );

        if( Schematic::Ptr pSchematic = boost::dynamic_pointer_cast< Schematic >( pNode ) )
        {
            // determine the centroid of the clip and offset accordingly
            for( Node::PtrVector::const_iterator i    = pSchematic->getChildren().begin(),
                                                 iEnd = pSchematic->getChildren().end();
                 i != iEnd;
                 ++i )
            {
                if( Node::Ptr pClipSite = boost::dynamic_pointer_cast< Node >( *i ) )
                {
                    Node::Ptr pClipSiteCopy = boost::dynamic_pointer_cast< Node >(
                        pClipSite->copy( m_pNode, m_pNode->generateNewNodeName( pClipSite ) ) );
                    m_pNode->add( pClipSiteCopy );
                    newNodes.insert( pClipSiteCopy );
                }
            }

            for( Node::PtrSet::iterator i = newNodes.begin(), iEnd = newNodes.end(); i != iEnd; ++i )
            {
                Node::Ptr pClipSiteCopy = *i;
                if( Site::Ptr pSiteCopy = boost::dynamic_pointer_cast< Site >( pClipSiteCopy ) )
                {
                    pSiteCopy->setTransform( drawTransform * pSiteCopy->getTransform() );
                }
            }
        }
        else if( Site::Ptr pSite = boost::dynamic_pointer_cast< Site >( pNode ) )
        {
            Site::Ptr pNewSite
                = boost::dynamic_pointer_cast< Site >( pSite->copy( m_pNode, m_pNode->generateNewNodeName( pSite ) ) );
            pNewSite->init();

            pNewSite->setTransform( drawTransform * pSite->getTransform() );

            m_pNode->add( pNewSite );
            newNodes.insert( pNewSite );
        }
        else
        {
            THROW_RTE( "Invalid node type for drawing" );
        }

        m_pNode->init();
    }

    // need the site contour calculated for selection to work
    update();

    {
        WriterLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );

        IGlyph::PtrSet selection;
        for( Node::PtrSet::iterator i = newNodes.begin(), iEnd = newNodes.end(); i != iEnd; ++i )
        {
            NodeMap::iterator iFind = m_glyphMap.find( *i );
            if( iFind != m_glyphMap.end() )
                selection.insert( iFind->second->getMainGlyph() );
        }

        m_pActiveInteraction = new Interaction( *this, eArea, x, y, qX, qY, IGlyph::Ptr(), selection );
    }

    m_pActiveInteraction->OnMove( x, y );

    return make_interaction_ptr( this, m_pActiveInteraction );
}

IEditContext* EditBase::getNestedContext( const std::vector< IGlyph* >& candidates )
{
    IEditContext* pEditContext = 0u;

    for( std::vector< IGlyph* >::const_iterator i = candidates.begin(), iEnd = candidates.end();
         i != iEnd && !pEditContext;
         ++i )
    {
        for( NodeMap::const_iterator j = m_glyphMap.begin(), jEnd = m_glyphMap.end(); j != jEnd; ++j )
        {
            if( j->second->owns( *i ) )
            {
                pEditContext = j->second.get();
                break;
            }
        }
    }

    return pEditContext;
}

IEditContext* EditBase::getNodeContext( Node::Ptr pNode )
{
    NodeMap::const_iterator iFind = m_glyphMap.find( pNode );
    if( iFind != m_glyphMap.end() )
    {
        return iFind->second.get();
    }
    return nullptr;
}

bool EditBase::canEdit( IGlyph* pGlyph, ToolType toolType, ToolMode toolMode, bool bAllowPoints, bool bAllowSites,
                        bool bAllowConnections ) const
{
    const GlyphSpec* pGlyphSpec = pGlyph->getGlyphSpec();

    if( !pGlyphSpec )
        return false;

    if( !pGlyphSpec->canEdit() )
        return false;

    const GlyphSpecProducer* pGlyphPrd = pGlyphSpec->getProducer();
    VERIFY_RTE( pGlyphPrd );

    if( pGlyphSpec == dynamic_cast< const Origin* >( m_pNode.get() ) )
        return false;

    switch( toolMode )
    {
        case eArea:
        {
            if( dynamic_cast< const ControlPoint* >( pGlyphSpec ) )
                return false;
            if( !bAllowSites )
            {
                if( dynamic_cast< const Space* >( pGlyphPrd ) )
                    return false;
                if( dynamic_cast< const Wall* >( pGlyphPrd ) )
                    return false;
                if( dynamic_cast< const Object* >( pGlyphPrd ) )
                    return false;
            }
            if( !bAllowConnections )
            {
                if( dynamic_cast< const Connection* >( pGlyphPrd ) )
                    return false;
            }
        }
        break;
        case eContour:
        {
            if( bAllowPoints )
            {
                if( !dynamic_cast< const ControlPoint* >( pGlyphSpec ) )
                    return false;
            }
            else
            {
                if( dynamic_cast< const ControlPoint* >( pGlyphSpec ) )
                    return false;

                if( !bAllowSites )
                {
                    if( dynamic_cast< const Space* >( pGlyphPrd ) )
                        return false;
                    if( dynamic_cast< const Wall* >( pGlyphPrd ) )
                        return false;
                    if( dynamic_cast< const Object* >( pGlyphPrd ) )
                        return false;
                }
                if( !bAllowConnections )
                {
                    if( dynamic_cast< const Connection* >( pGlyphPrd ) )
                        return false;
                }
            }
        }
        break;
        default:
            THROW_RTE( "Unknown tool mode" );
    }

    // ensure glyph is within active context
    bool bWithin = false;
    {
        const Node* pNodeIter = pGlyphPrd;
        while( pNodeIter )
        {
            if( pNodeIter == m_pNode.get() )
            {
                bWithin = true;
                break;
            }
            pNodeIter = pNodeIter->getParent().get();
        }
    }
    if( !bWithin )
        return false;

    return true;
}

void EditBase::update()
{
    ReaderLock lock1( getNodeStructureLock() );
    ReaderLock lock2( getNodeDataLock() );
    ReaderLock lock3( getSiteContourLock() );

    updateGlyphs();
}

void EditBase::updateGlyphs()
{
    Node::PtrSet nodes;

    for( Node::Ptr pChildNode : m_pNode->getChildren() )
    {
        if( dynamic_cast< Origin* >( pChildNode.get() ) )
            nodes.insert( pChildNode );
    }

    NodeMap         removals;
    Node::PtrVector additions;
    generics::match( m_glyphMap.begin(), m_glyphMap.end(), nodes.begin(), nodes.end(),
                     generics::lessthan( generics::first< NodeMap::const_iterator >(),
                                         generics::deref< Node::PtrSet::const_iterator >() ),
                     generics::collect( removals, generics::deref< NodeMap::const_iterator >() ),
                     generics::collect( additions, generics::deref< Node::PtrSet::const_iterator >() ) );

    for( NodeMap::iterator i = removals.begin(), iEnd = removals.end(); i != iEnd; ++i )
    {
        m_glyphMap.erase( i->first );
    }

    for( Node::PtrVector::iterator i = additions.begin(), iEnd = additions.end(); i != iEnd; ++i )
    {
        if( Site::Ptr pSite = boost::dynamic_pointer_cast< Site >( *i ) )
        {
            m_glyphMap.insert(
                std::make_pair( *i, EditNested::Ptr( new EditNested( m_editRoot, *this, pSite, m_glyphFactory ) ) ) );
        }
    }

    generics::for_each_second(
        m_glyphMap, []( boost::shared_ptr< EditNested > pSpaceGlyphs ) { pSpaceGlyphs->updateGlyphs(); } );

    generics::for_each_second( m_markup, []( IGlyph::Ptr pGlyph ) { pGlyph->update(); } );
}

void EditBase::interaction_end( IInteraction* pInteraction )
{
    {
        ASSERT( m_pActiveInteraction == pInteraction );
        delete m_pActiveInteraction;
        m_pActiveInteraction = 0u;
    }

    onEditted( true );
}

void EditBase::cmd_delete( const std::set< IGlyph* >& selection )
{
    {
        WriterLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        WriterLock lock3( getSiteContourLock() );

        cmd_delete_impl( selection );
    }

    onEditted( true );
}

void EditBase::cmd_delete_impl( const std::set< IGlyph* >& selection )
{
    for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
    {
        IGlyph* pGlyph = *i;

        for( NodeMap::const_iterator j = m_glyphMap.begin(), jEnd = m_glyphMap.end(); j != jEnd; ++j )
        {
            Node::Ptr pNode = j->first;
            if( dynamic_cast< const GlyphSpec* >( pNode.get() ) == pGlyph->getGlyphSpec() )
            {
                m_glyphMap.erase( j );
                m_pNode->remove( pNode );
                break;
            }
        }
    }
    m_pNode->init();
}

Node::Ptr EditBase::cmd_cut( const std::set< IGlyph* >& selection )
{
    Node::Ptr pResult;
    {
        WriterLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        WriterLock lock3( getSiteContourLock() );

        Node::PtrSet nodes;
        for( std::set< IGlyph* >::const_iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
        {
            IGlyph* pGlyph = *i;

            for( NodeMap::const_iterator j = m_glyphMap.begin(), jEnd = m_glyphMap.end(); j != jEnd; ++j )
            {
                Node::Ptr pNode = j->first;
                if( dynamic_cast< const GlyphSpec* >( pNode.get() ) == pGlyph->getGlyphSpec() )
                {
                    m_glyphMap.erase( j );
                    m_pNode->remove( pNode );
                    nodes.insert( pNode );
                    break;
                }
            }
        }

        if( !nodes.empty() )
        {
            Clip::Ptr pClip( new Clip( "clip" ) );
            for( Node::PtrSet::iterator i = nodes.begin(), iEnd = nodes.end(); i != iEnd; ++i )
            {
                VERIFY_RTE( pClip->add( ( *i )->copy( pClip, ( *i )->Node::getName() ) ) );
            }
            pResult = pClip;
        }

        m_pNode->init();
    }

    onEditted( true );

    return pResult;
}

Node::Ptr EditBase::cmd_copy( const std::set< IGlyph* >& selection )
{
    Node::Ptr pResult;
    {
        WriterLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        WriterLock lock3( getSiteContourLock() );

        Node::PtrSet nodes;
        for( std::set< IGlyph* >::const_iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
        {
            IGlyph* pGlyph = *i;

            for( NodeMap::const_iterator j = m_glyphMap.begin(), jEnd = m_glyphMap.end(); j != jEnd; ++j )
            {
                Node::Ptr pNode = j->first;
                if( dynamic_cast< const GlyphSpec* >( pNode.get() ) == pGlyph->getGlyphSpec() )
                {
                    nodes.insert( pNode );
                    break;
                }
            }
        }

        if( !nodes.empty() )
        {
            Clip::Ptr pClip( new Clip( "copy" ) );
            for( Node::PtrSet::iterator i = nodes.begin(), iEnd = nodes.end(); i != iEnd; ++i )
            {
                bool bResult = pClip->add( ( *i )->copy( pClip, ( *i )->Node::getName() ) );
                VERIFY_RTE( bResult );
            }
            pResult = pClip;
        }
    }

    onEditted( true );

    return pResult;
}

IInteraction::Ptr EditBase::cmd_paste( Node::Ptr pPaste, Float x, Float y, Float qX, Float qY )
{
    Node::PtrVector nodes;
    if( Schematic::Ptr pClip = boost::dynamic_pointer_cast< Schematic >( pPaste ) )
    {
        for( Node::PtrVector::const_iterator i = pClip->getChildren().begin(), iEnd = pClip->getChildren().end();
             i != iEnd;
             ++i )
        {
            if( Node::Ptr pSite = boost::dynamic_pointer_cast< Node >( *i ) )
                nodes.push_back( pSite );
        }
    }
    else if( Node::Ptr pSite = boost::dynamic_pointer_cast< Node >( pPaste ) )
    {
        nodes.push_back( pSite );
    }

    return cmd_paste( nodes, x, y, qX, qY );
}

IInteraction::Ptr EditBase::cmd_paste( IGlyph* pGlyph, Float x, Float y, Float qX, Float qY )
{
    Node::PtrVector nodes;
    if( const Node* pSite = dynamic_cast< const Node* >( pGlyph->getGlyphSpec() ) )
    {
        Node::Ptr pNode = const_cast< Node* >( pSite )->getPtr();
        VERIFY_RTE( pNode );
        Node::Ptr pSitePtr = boost::dynamic_pointer_cast< Node >( pNode );
        VERIFY_RTE( pSitePtr );
        nodes.push_back( pSitePtr );
    }
    return cmd_paste( nodes, x, y, qX, qY );
}
IInteraction::Ptr EditBase::cmd_paste( const std::set< IGlyph* >& selection, Float x, Float y, Float qX, Float qY )
{
    Node::PtrVector nodes;
    for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
    {
        if( const Node* pSite = dynamic_cast< const Node* >( ( *i )->getGlyphSpec() ) )
        {
            Node::Ptr pNode = const_cast< Node* >( pSite )->getPtr();
            VERIFY_RTE( pNode );
            Node::Ptr pSitePtr = boost::dynamic_pointer_cast< Node >( pNode );
            VERIFY_RTE( pSitePtr );
            nodes.push_back( pSitePtr );
        }
    }
    return cmd_paste( nodes, x, y, qX, qY );
}

IInteraction::Ptr EditBase::cmd_paste( Node::PtrVector nodes, Float x, Float y, Float qX, Float qY )
{
    IInteraction::Ptr pNewInteraction;
    {
        WriterLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        WriterLock lock3( getSiteContourLock() );

        ASSERT( !m_pActiveInteraction );

        IGlyph::PtrSet copies;
        for( Node::PtrVector::const_iterator i = nodes.begin(), iEnd = nodes.end(); i != iEnd; ++i )
        {
            const std::string strNewKey = m_pNode->generateNewNodeName( *i );
            Node::Ptr         pCopy     = boost::dynamic_pointer_cast< Node >( ( *i )->copy( m_pNode, strNewKey ) );
            VERIFY_RTE( pCopy );
            m_pNode->add( pCopy );

            if( Site::Ptr pSiteCopy = boost::dynamic_pointer_cast< Site >( pCopy ) )
            {
                EditNested::Ptr pSpaceGlyphs( new EditNested( m_editRoot, *this, pSiteCopy, m_glyphFactory ) );
                m_glyphMap.insert( std::make_pair( pCopy, pSpaceGlyphs ) );

                copies.insert( pSpaceGlyphs->getMainGlyph() );
            }
        }

        m_pActiveInteraction = new Interaction( *this, IEditContext::eArea, x, y, qX, qY, IGlyph::Ptr(), copies );
        pNewInteraction      = make_interaction_ptr( this, m_pActiveInteraction );

        m_pNode->init();
    }

    onEditted( true );

    return pNewInteraction;
}

void EditBase::cmd_rotateLeft( const std::set< IGlyph* >& selection )
{
    {
        ReaderLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        // WriterLock lock3( getSiteContourLock() );

        std::vector< Site* > nodes;
        for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
        {
            if( const Site* pSite = dynamic_cast< const Site* >( ( *i )->getGlyphSpec() ) )
            {
                nodes.push_back( const_cast< Site* >( pSite ) );
            }
        }

        Rect transformBounds;
        Utils::getSelectionBounds( nodes, transformBounds );

        for( Site* pSite : nodes )
        {
            pSite->cmd_rotateLeft( transformBounds );
        }
    }

    onEditted( true );
}
void EditBase::cmd_rotateRight( const std::set< IGlyph* >& selection )
{
    {
        ReaderLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );

        std::vector< Site* > areas;
        for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
        {
            if( const Site* pSite = dynamic_cast< const Site* >( ( *i )->getGlyphSpec() ) )
                areas.push_back( const_cast< Site* >( pSite ) );
        }

        Rect transformBounds;
        Utils::getSelectionBounds( areas, transformBounds );

        for( Site* pArea : areas )
        {
            pArea->cmd_rotateRight( transformBounds );
        }
    }
    onEditted( true );
}
void EditBase::cmd_flipHorizontally( const std::set< IGlyph* >& selection )
{
    {
        ReaderLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );

        std::vector< Site* > areas;
        for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
        {
            if( const Site* pSite = dynamic_cast< const Site* >( ( *i )->getGlyphSpec() ) )
                areas.push_back( const_cast< Site* >( pSite ) );
        }

        Rect transformBounds;
        Utils::getSelectionBounds( areas, transformBounds );

        for( Site* pArea : areas )
        {
            pArea->cmd_flipHorizontally( transformBounds );
        }
    }
    onEditted( true );
}
void EditBase::cmd_flipVertically( const std::set< IGlyph* >& selection )
{
    {
        ReaderLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );

        std::vector< Site* > areas;
        for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
        {
            if( const Site* pSite = dynamic_cast< const Site* >( ( *i )->getGlyphSpec() ) )
                areas.push_back( const_cast< Site* >( pSite ) );
        }

        Rect transformBounds;
        Utils::getSelectionBounds( areas, transformBounds );

        for( Site* pArea : areas )
        {
            pArea->cmd_flipVertically( transformBounds );
        }
    }
    onEditted( true );
}

void EditBase::cmd_shrink( const std::set< IGlyph* >& selection, double dbAmount )
{
    {
        ReaderLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        WriterLock lock3( getSiteContourLock() );

        std::vector< Site* > areas;
        for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
        {
            if( const Site* pSite = dynamic_cast< const Site* >( ( *i )->getGlyphSpec() ) )
                areas.push_back( const_cast< Site* >( pSite ) );
        }

        for( Site* pArea : areas )
        {
            pArea->cmd_shrink( dbAmount );
        }
    }
    onEditted( true );
}

void EditBase::cmd_extrude( const std::set< IGlyph* >& selection, double dbAmount )
{
    {
        ReaderLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        WriterLock lock3( getSiteContourLock() );

        std::vector< Site* > areas;
        for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
        {
            if( const Site* pSite = dynamic_cast< const Site* >( ( *i )->getGlyphSpec() ) )
                areas.push_back( const_cast< Site* >( pSite ) );
        }

        for( Site* pArea : areas )
        {
            pArea->cmd_extrude( dbAmount );
        }
    }
    onEditted( true );
}

void EditBase::cmd_union_impl( const std::vector< Site* >& sites, bool bCreateWallsForHoles )
{
    if( sites.size() < 2U )
        return;

    using SiteSet    = std::set< Site* >;
    using SiteSetMap = std::map< SiteSet, Site::Ptr >;

    SiteSetMap disjointSets;
    for( Site* pSite : sites )
    {
        SiteSet   initialSet{ pSite };
        Site::Ptr pSitePtr = boost::dynamic_pointer_cast< Site >( pSite->getPtr() );
        VERIFY_RTE( pSitePtr );
        disjointSets.insert( std::make_pair( initialSet, pSitePtr ) );
    }

    std::set< Site::Ptr > deletedSites;

    // enumerate all unique pairs
    for( std::size_t szX = 0U; szX < ( sites.size() - 1U ); ++szX )
    {
        for( std::size_t szY = szX + 1U; szY < sites.size(); ++szY )
        {
            Site* pSiteOne = sites[ szX ];
            Site* pSiteTwo = sites[ szY ];

            // find associated disjoint sets
            SiteSetMap::iterator iSiteOneIter = disjointSets.end(), iSiteTwoIter = disjointSets.end();
            {
                for( SiteSetMap::iterator i = disjointSets.begin(), iEnd = disjointSets.end(); i != iEnd; ++i )
                {
                    if( i->first.count( pSiteOne ) )
                        iSiteOneIter = i;
                    if( i->first.count( pSiteTwo ) )
                        iSiteTwoIter = i;

                    if( iSiteOneIter != disjointSets.end() && iSiteTwoIter != disjointSets.end() )
                        break;
                }
                VERIFY_RTE( iSiteOneIter != disjointSets.end() && iSiteTwoIter != disjointSets.end() );
            }

            // if sites are not already in same disjoint set...
            if( iSiteOneIter != iSiteTwoIter )
            {
                // test for boolean intersection
                std::vector< exact::Polygon > sitePolygons;

                Site::Ptr pUnionSiteOne = iSiteOneIter->second;
                {
                    exact::Polygon siteOnePoly
                        = Utils::convert< exact::Kernel >( pUnionSiteOne->getContour()->getPolygon() );
                    const exact::Transform exactTransformOne = inexactToExact( pUnionSiteOne->getTransform() );

                    for( auto& p : siteOnePoly )
                        p = exactTransformOne( p );

                    if( !siteOnePoly.is_empty() && siteOnePoly.is_simple() )
                    {
                        if( !siteOnePoly.is_counterclockwise_oriented() )
                        {
                            siteOnePoly.reverse_orientation();
                        }
                    }
                    sitePolygons.push_back( siteOnePoly );
                }

                Site::Ptr pUnionSiteTwo = iSiteTwoIter->second;
                {
                    exact::Polygon siteTwoPoly
                        = Utils::convert< exact::Kernel >( pUnionSiteTwo->getContour()->getPolygon() );
                    const exact::Transform exactTransformTwo = inexactToExact( pUnionSiteTwo->getTransform() );

                    for( auto& p : siteTwoPoly )
                        p = exactTransformTwo( p );

                    if( !siteTwoPoly.is_empty() && siteTwoPoly.is_simple() )
                    {
                        if( !siteTwoPoly.is_counterclockwise_oriented() )
                        {
                            siteTwoPoly.reverse_orientation();
                        }
                    }
                    sitePolygons.push_back( siteTwoPoly );
                }

                std::vector< exact::Polygon_with_holes > siteUnion;
                CGAL::join( sitePolygons.begin(), sitePolygons.end(), std::back_inserter( siteUnion ) );

                if( siteUnion.size() == 1U )
                {
                    // update the contour to the union
                    const exact::Transform exactTransformOne = inexactToExact( pUnionSiteOne->getTransform() );

                    const exact::Transform inverseTransformOne = exactTransformOne.inverse();

                    const exact::Transform exactTransformTwo = inexactToExact( pUnionSiteTwo->getTransform() );

                    // bCreateWallsForHoles...

                    // calculate the new contour
                    {
                        const exact::Polygon_with_holes& singularPolyWithHoles = siteUnion.back();
                        VERIFY_RTE( !singularPolyWithHoles.is_unbounded() );

                        exact::Polygon unionContour = singularPolyWithHoles.outer_boundary();
                        // VERIFY_RTE( !unionContour.is_empty() && unionContour.is_simple() );

                        for( auto& p : unionContour )
                        {
                            p = inverseTransformOne( p );
                        }

                        if( !unionContour.is_empty() && unionContour.is_simple() )
                        {
                            if( !unionContour.is_counterclockwise_oriented() )
                            {
                                unionContour.reverse_orientation();
                            }
                        }

                        // filter the polygon with inexact precision
                        pUnionSiteOne->getContour()->set( Utils::convert< map::Kernel >( unionContour ) );

                        pUnionSiteOne->cmd_filter();
                    }

                    // merge the disjoint sets
                    {
                        SiteSet siteSetOne = iSiteOneIter->first;
                        SiteSet siteSetTwo = iSiteTwoIter->first;

                        disjointSets.erase( iSiteOneIter );
                        disjointSets.erase( iSiteTwoIter );

                        for( Site* pSite : siteSetTwo )
                            siteSetOne.insert( pSite );

                        disjointSets.insert( std::make_pair( siteSetOne, pUnionSiteOne ) );
                    }

                    // copy contents from pUnionSiteTwo into pUnionSiteOne
                    // and adjust by transform
                    for( Node::Ptr pChild : pUnionSiteTwo->getChildren() )
                    {
                        if( Site::Ptr pChildSite = boost::dynamic_pointer_cast< Site >( pChild ) )
                        {
                            const std::string strNewKey = pUnionSiteOne->generateNewNodeName( pChild );
                            Node::Ptr         pCopy
                                = boost::dynamic_pointer_cast< Node >( pChild->copy( pUnionSiteOne, strNewKey ) );
                            VERIFY_RTE( pCopy );

                            Site::Ptr pSiteCopy = boost::dynamic_pointer_cast< Site >( pCopy );
                            VERIFY_RTE( pSiteCopy );

                            exact::Transform siteTransform = inexactToExact( pSiteCopy->getTransform() );

                            siteTransform = exactTransformTwo * siteTransform;
                            siteTransform = inverseTransformOne * siteTransform;

                            pSiteCopy->setTransform( exactToInexact( siteTransform ) );

                            pUnionSiteOne->add( pSiteCopy );
                        }
                    }

                    // record the deleted site
                    deletedSites.insert( pUnionSiteTwo );
                }
            }
        }
    }

    for( Site::Ptr pDeleted : deletedSites )
    {
        m_pNode->remove( pDeleted );
    }
}

void EditBase::cmd_union( const std::set< IGlyph* >& selection )
{
    {
        WriterLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        WriterLock lock3( getSiteContourLock() );

        std::vector< Site* > spaces;
        std::vector< Site* > walls;
        std::vector< Site* > objects;

        for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
        {
            if( const Space* pSite = dynamic_cast< const Space* >( ( *i )->getGlyphSpec() ) )
                spaces.push_back( const_cast< Space* >( pSite ) );
            else if( const Wall* pSite = dynamic_cast< const Wall* >( ( *i )->getGlyphSpec() ) )
                walls.push_back( const_cast< Wall* >( pSite ) );
            else if( const Object* pSite = dynamic_cast< const Object* >( ( *i )->getGlyphSpec() ) )
                objects.push_back( const_cast< Object* >( pSite ) );
        }

        cmd_union_impl( spaces, true );
        cmd_union_impl( walls, false );
        cmd_union_impl( objects, false );
    }
    onEditted( true );
}

void EditBase::cmd_filter( const std::set< IGlyph* >& selection )
{
    {
        ReaderLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        WriterLock lock3( getSiteContourLock() );

        for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
        {
            if( const Site* pSiteCst = dynamic_cast< const Site* >( ( *i )->getGlyphSpec() ) )
            {
                Site* pSite = const_cast< Site* >( pSiteCst );
                pSite->cmd_filter();
            }
        }
    }
    onEditted( true );
}

void EditBase::cmd_aabb( const std::set< IGlyph* >& selection )
{
    {
        WriterLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        WriterLock lock3( getSiteContourLock() );

        Site::PtrVector sites;
        {
            for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
            {
                if( const Site* pSiteCst = dynamic_cast< const Site* >( ( *i )->getGlyphSpec() ) )
                {
                    Site* pSite = const_cast< Site* >( pSiteCst );
                    sites.push_back( boost::dynamic_pointer_cast< Site >( pSite->getPtr() ) );
                }
            }
        }

        for( Site::Ptr pSite : sites )
        {
            pSite->cmd_aabb();
        }
    }
    onEditted( true );
}

void EditBase::cmd_convexHull( const std::set< IGlyph* >& selection )
{
    {
        WriterLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        WriterLock lock3( getSiteContourLock() );

        Site::PtrVector sites;
        {
            for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
            {
                if( const Site* pSiteCst = dynamic_cast< const Site* >( ( *i )->getGlyphSpec() ) )
                {
                    Site* pSite = const_cast< Site* >( pSiteCst );
                    sites.push_back( boost::dynamic_pointer_cast< Site >( pSite->getPtr() ) );
                }
            }
        }

        for( Site::Ptr pSite : sites )
        {
            pSite->cmd_convexHull();
        }
    }
    onEditted( true );
}

void EditBase::cmd_reparent( const std::set< IGlyph* >& selection )
{
    {
        WriterLock lock1( getNodeStructureLock() );
        WriterLock lock2( getNodeDataLock() );
        WriterLock lock3( getSiteContourLock() );

        Site::PtrVector sites;
        {
            Site::PtrSet sitesSet;
            for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
            {
                if( const Site* pSiteCst = dynamic_cast< const Site* >( ( *i )->getGlyphSpec() ) )
                {
                    Site*     pSite    = const_cast< Site* >( pSiteCst );
                    Site::Ptr pSitePtr = boost::dynamic_pointer_cast< Site >( pSite->getPtr() );
                    VERIFY_RTE( pSitePtr );
                    VERIFY_RTE( pSitePtr->Node::getParent() == m_pNode );

                    sitesSet.insert( pSitePtr );
                }
            }
            sites.assign( sitesSet.begin(), sitesSet.end() );
        }

        Rect transformBounds;
        Utils::getSelectionBounds( sites, transformBounds );

        Site::Ptr pNewNode( new Space( m_pNode, m_pNode->generateNewNodeName( "space" ) ) );

        const Point ptCentre( transformBounds.xmin() + ( ( transformBounds.xmax() - transformBounds.xmin() ) / 2.0 ),
                              transformBounds.ymin() + ( ( transformBounds.ymax() - transformBounds.ymin() ) / 2.0 ) );

        pNewNode->init( calculateInitTransform( ptCentre ) );

        m_pNode->add( pNewNode );

        const exact::Transform exactTransformOne = inexactToExact( pNewNode->getTransform() );

        const exact::Transform inverseTransformOne = exactTransformOne.inverse();

        for( Site::Ptr pSite : sites )
        {
            const std::string strNewKey = pNewNode->generateNewNodeName( pSite );
            Node::Ptr         pCopy     = boost::dynamic_pointer_cast< Node >( pSite->copy( pNewNode, strNewKey ) );
            VERIFY_RTE( pCopy );

            Site::Ptr pSiteCopy = boost::dynamic_pointer_cast< Site >( pCopy );
            VERIFY_RTE( pSiteCopy );

            exact::Transform siteTransform = inexactToExact( pSiteCopy->getTransform() );

            siteTransform = inverseTransformOne * siteTransform;

            pSiteCopy->setTransform( exactToInexact( siteTransform ) );

            pNewNode->add( pSiteCopy );
        }

        pNewNode->cmd_aabb();

        for( Site::Ptr pSite : sites )
        {
            m_pNode->remove( pSite );
        }
    }
    onEditted( true );
}

} // namespace map