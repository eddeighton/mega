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

#include "schematic/editInteractions.hpp"
#include "schematic/editBase.hpp"
#include "schematic/cgalUtils.hpp"
#include "schematic/file.hpp"

namespace schematic
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Interaction::Interaction( EditBase& edit, IEditContext::ToolMode toolMode, Float x, Float y, Float qX, Float qY,
                          IGlyph::Ptr pHitGlyph, const IGlyph::PtrSet& totalSelection )
    : m_edit( edit )
    , m_toolMode( toolMode )
    , m_startX( x )
    , m_startY( y )
    , m_qX( qX )
    , m_qY( qY )
    , m_pHitGlyph( pHitGlyph )
{
    IGlyph::PtrSet allowed;
    {
        for( auto i = totalSelection.begin(), iEnd = totalSelection.end(); i != iEnd; ++i )
        {
            IGlyph::Ptr pGlyph = *i;
            // determine whether the glyph belongs to the current edit context...
            if( m_edit.canEdit( pGlyph.get(), IEditContext::eSelect, m_toolMode, true, true, true ) )
            {
                allowed.insert( pGlyph );
            }
        }
    }

    // resolve set of glyphs to edit that do not contain themselves
    for( IGlyph::Ptr pGlyph : allowed )
    {
        // TODO ...
    }
    m_selection = allowed;

    if( pHitGlyph )
    {
        m_selection.insert( pHitGlyph );
    }

    if( !m_selection.empty() )
    {
        for( auto i = m_selection.begin(), iEnd = m_selection.end(); i != iEnd; ++i )
        {
            IGlyph::Ptr      pGlyph     = *i;
            const GlyphSpec* pGlyphSpec = pGlyph->getGlyphSpec();

            if( const Origin* pOrigin = dynamic_cast< const Origin* >( pGlyphSpec ) )
            {
                m_interactives.push_back( const_cast< GlyphSpec* >( pGlyphSpec ) );
                m_initialOffsets.push_back( Point( 0.0, 0.0 ) );

                const Transform relativeTransform = pOrigin->getAbsoluteTransform();

                m_initialTransforms.push_back( pOrigin->getTransform() );
                m_initialRelativeTransforms.push_back( relativeTransform );
                m_initialTransformInverses.push_back( relativeTransform.inverse() );
            }
            else if( const ControlPoint* pControlPoint = dynamic_cast< const ControlPoint* >( pGlyphSpec ) )
            {
                m_interactives.push_back( const_cast< GlyphSpec* >( pGlyphSpec ) );
                m_initialOffsets.push_back( pControlPoint->getPoint() );

                const GlyphSpecProducer* pProducer = pControlPoint->getProducer();
                VERIFY_RTE( pProducer );

                if( const Feature* pFeature = dynamic_cast< const Feature* >( pProducer ) )
                {
                    // find origin
                    const Origin* pFoundOrigin = nullptr;
                    const Node*   pIter        = pFeature;
                    while( pIter )
                    {
                        if( pFoundOrigin = dynamic_cast< const Origin* >( pIter ); pFoundOrigin )
                        {
                            break;
                        }
                        pIter = pIter->getParent().get();
                    }

                    VERIFY_RTE( pFoundOrigin );

                    const Transform relativeTransform
                        = pFoundOrigin->getAbsoluteTransform() * pFoundOrigin->getTransform();

                    // Transform relativeTransform =
                    //     pFoundOrigin->getAbsoluteTransform();
                    //
                    ////if( pIter != edit.getNode().get() )
                    //    relativeTransform = relativeTransform * pFoundOrigin->getTransform();

                    m_initialTransforms.push_back( pFoundOrigin->getTransform() );
                    m_initialRelativeTransforms.push_back( relativeTransform );
                    m_initialTransformInverses.push_back( relativeTransform.inverse() );
                }
                else if( const Origin* pProducerOrigin = dynamic_cast< const Origin* >( pProducer ) )
                {
                    const Transform relativeTransform
                        = pProducerOrigin->getAbsoluteTransform() * pProducerOrigin->getTransform();

                    m_initialTransforms.push_back( pProducerOrigin->getTransform() );
                    m_initialRelativeTransforms.push_back( relativeTransform );
                    m_initialTransformInverses.push_back( relativeTransform.inverse() );
                }
                else if( dynamic_cast< const File* >( pProducer ) )
                {
                    const Transform transform( CGAL::IDENTITY );
                    m_initialTransforms.push_back( transform );
                    m_initialRelativeTransforms.push_back( transform );
                    m_initialTransformInverses.push_back( transform );
                }
                else
                {
                    THROW_RTE( "Unknown glyph spec producer type" );
                }
            }
        }
    }
}

void Interaction::OnMove( Float x, Float y )
{
    {
        Float fDeltaX = Math::quantize_roundUp( x - m_startX, m_qX );
        Float fDeltaY = Math::quantize_roundUp( y - m_startY, m_qY );

        InitialTransformVector::iterator iTransformIter = m_initialTransforms.begin();
        InitialTransformVector::iterator iRelativeIter  = m_initialRelativeTransforms.begin();
        InitialTransformVector::iterator iInverseIter   = m_initialTransformInverses.begin();
        PointVector::iterator            iOffsetIter    = m_initialOffsets.begin();
        GlyphSpecVector::iterator        i = m_interactives.begin(), iEnd = m_interactives.end();

        for( ; i != iEnd; ++i, ++iTransformIter, ++iRelativeIter, ++iInverseIter, ++iOffsetIter )
        {
            GlyphSpec*       pGlyphSpec        = *i;
            const Transform& initialTransform  = *iTransformIter;
            const Transform& relativeTransform = *iRelativeIter;
            const Transform& inverseTransform  = *iInverseIter;
            const Point&     initialOffset     = *iOffsetIter;

            if( Origin* pOrigin = dynamic_cast< Origin* >( pGlyphSpec ) )
            {
                Transform t = initialTransform;
                t           = relativeTransform * t;
                t           = translate( Vector( fDeltaX, fDeltaY ) ) * t;
                t           = inverseTransform * t;

                pOrigin->setTransform( t );
            }
            else if( ControlPoint* pControlPoint = dynamic_cast< ControlPoint* >( pGlyphSpec ) )
            {
                Point pt = initialOffset;
                pt       = relativeTransform( pt );
                pt       = pt + Vector( fDeltaX, fDeltaY );
                pt       = inverseTransform( pt );

                pControlPoint->setPoint( pt );
            }
        }
    }
    m_edit.onEditted( false );
}

Node::Ptr Interaction::GetInteractionNode() const
{
    return m_edit.getNode();
}

void Interaction::GetSelection( std::set< IGlyph* >& selection ) const
{
    selection.clear();
    for( IGlyph::Ptr pGlyph : m_selection )
        selection.insert( pGlyph.get() );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
InteractionToolWrapper::InteractionToolWrapper( EditBase& edit, IInteraction::Ptr pWrapped )
    : m_edit( edit )
    , m_pToolInteraction( pWrapped )
{
}

void InteractionToolWrapper::OnMove( Float x, Float y )
{
    m_pToolInteraction->OnMove( x, y );
    m_edit.onEditted( false );
}

Node::Ptr InteractionToolWrapper::GetInteractionNode() const
{
    return m_edit.getNode();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Polygon_Interaction::Polygon_Interaction( Site& site, Float x, Float y, Float qX, Float qY, std::size_t szIndex )
    : m_site( site )
    , m_qX( qX )
    , m_qY( qY )
    , m_szIndex( szIndex )
{
    m_startX = x = Math::quantize_roundUp( x, qX );
    m_startY = y = Math::quantize_roundUp( y, qY );

    m_inverseTransform = site.getTransform().inverse() * site.getAbsoluteTransform().inverse();

    if( Feature_Contour::Ptr pContour = m_site.getContour() )
    {
        m_originalPolygon = pContour->getPolygon();

        if( ( m_originalPolygon.size() > 2U ) && m_originalPolygon.is_simple()
            && !m_originalPolygon.is_counterclockwise_oriented() )
        {
            std::reverse( m_originalPolygon.begin(), m_originalPolygon.end() );
        }

        const Point pt = m_inverseTransform( Point( m_startX, m_startY ) );

        Polygon newPoly;

        for( std::size_t sz = 0U; ( sz != m_szIndex ) && ( sz < m_originalPolygon.size() ); ++sz )
            newPoly.push_back( m_originalPolygon[ sz ] );

        newPoly.push_back( pt );

        for( std::size_t sz = m_szIndex; sz < m_originalPolygon.size(); ++sz )
            newPoly.push_back( m_originalPolygon[ sz ] );

        if( ( newPoly.size() > 2U ) && newPoly.is_simple() && !newPoly.is_counterclockwise_oriented() )
        {
            std::reverse( newPoly.begin(), newPoly.end() );
            m_szIndex = ( newPoly.size() - 1U ) - std::min( m_szIndex, newPoly.size() - 1U );
        }

        pContour->set( newPoly );
    }
}

void Polygon_Interaction::OnMove( Float x, Float y )
{
    if( Feature_Contour::Ptr pContour = m_site.getContour() )
    {
        const Float fDeltaX = Math::quantize_roundUp( x - m_startX, m_qX );
        const Float fDeltaY = Math::quantize_roundUp( y - m_startY, m_qY );

        const Point pt = m_inverseTransform( Point( m_startX + fDeltaX, m_startY + fDeltaY ) );

        pContour->setPoint( m_szIndex, pt );
    }
}

Node::Ptr Polygon_Interaction::GetInteractionNode() const
{
    return boost::dynamic_pointer_cast< Node >( m_site.getPtr() );
}

} // namespace schematic