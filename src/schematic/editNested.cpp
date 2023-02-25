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

#include "schematic/editNested.hpp"
#include "schematic/editRoot.hpp"
#include "schematic/cgalUtils.hpp"

namespace schematic
{

EditNested::EditNested( EditRoot& editRoot, IEditContext& parentContext, Site::Ptr pSite, GlyphFactory& glyphFactory )
    : EditBase( editRoot, glyphFactory, pSite )
    , m_parent( parentContext )
    , m_pSite( pSite )
{
    // create the main image glyph
    if( Origin* pOrigin = dynamic_cast< Origin* >( m_pNode.get() ) )
    {
        if( EditNested* pParentGlyphs = dynamic_cast< EditNested* >( &m_parent ) )
            m_pMainGlyph = m_glyphFactory.createOrigin( pOrigin, pParentGlyphs->getMainGlyph() );
        else
            m_pMainGlyph = m_glyphFactory.createOrigin( pOrigin, IGlyph::Ptr() );
    }

    if( GlyphSpecProducer* pGlyphSpecProducer = dynamic_cast< GlyphSpecProducer* >( m_pNode.get() ) )
    {
        MarkupText::List texts;
        pGlyphSpecProducer->getMarkupTexts( texts );
        for( MarkupText::List::const_iterator i = texts.begin(), iEnd = texts.end(); i != iEnd; ++i )
        {
            if( IGlyph::Ptr pMarkupGlyph = m_glyphFactory.createMarkupText( *i, m_pMainGlyph ) )
                m_markup.insert( std::make_pair( *i, pMarkupGlyph ) );
        }

        MarkupPolygonGroup::List polyGroups;
        pGlyphSpecProducer->getMarkupPolygonGroups( polyGroups );
        for( MarkupPolygonGroup::List::const_iterator i = polyGroups.begin(), iEnd = polyGroups.end(); i != iEnd; ++i )
        {
            if( IGlyph::Ptr pMarkupGlyph = m_glyphFactory.createMarkupPolygonGroup( *i, m_pMainGlyph ) )
                m_markup.insert( std::make_pair( *i, pMarkupGlyph ) );
        }
    }
}

IGlyph::Ptr EditNested::createControlPointGlyph( ControlPoint* pControlPoint )
{
    VERIFY_RTE( pControlPoint->getParent() );

    IGlyph::Ptr pParentGlyph;
    {
        if( const Origin* pParentOrigin = dynamic_cast< const Origin* >( pControlPoint->getParent() ) )
        {
            VERIFY_RTE( m_pNode.get() == dynamic_cast< const Site* >( pParentOrigin ) );
            pParentGlyph = m_pMainGlyph;
        }
        else
        {
            GlyphMap::const_iterator iFind = m_glyphs.find( pControlPoint->getParent() );
            if( iFind != m_glyphs.end() )
                pParentGlyph = iFind->second;
        }
    }
    VERIFY_RTE( pParentGlyph );

    IGlyph::Ptr pNewGlyph = m_glyphFactory.createControlPoint( pControlPoint, pParentGlyph );
    m_glyphs.insert( std::make_pair( pControlPoint, pNewGlyph ) );

    return pNewGlyph;
}

void EditNested::matchFeatures()
{
    Feature::PtrSet features;
    m_pNode->for_each_recursive(
        generics::collectIfConvert( features, Node::ConvertPtrType< Feature >(), Node::ConvertPtrType< Feature >() ),
        Node::ConvertPtrType< Site >() );

    Feature::PtrVector removals, additions;
    using UpdateVector = std::vector< FeatureGlyphMap::iterator >;
    UpdateVector updates;
    generics::matchGetUpdates(
        m_features.begin(), m_features.end(), features.begin(), features.end(),

        generics::lessthan( generics::first< FeatureGlyphMap::const_iterator >(),
                            generics::deref< Feature::PtrSet::const_iterator >() ),

        []( FeatureGlyphMap::const_iterator i, Feature::PtrSet::const_iterator j )
        {
            const Feature::Ptr&    pFeature = i->first;
            const FeatureGlyphSet& glyphs   = i->second;
            return glyphs.tick < pFeature->getLastModifiedTick();
        },

        generics::collect( removals, generics::first< FeatureGlyphMap::const_iterator >() ),
        generics::collect( additions, generics::deref< Feature::PtrSet::const_iterator >() ),
        generics::collect( updates ) );

    for( UpdateVector::iterator iter = updates.begin(), iEnd = updates.end(); iter != iEnd; ++iter )
    {
        FeatureGlyphMap::iterator iterFeature = *iter;

        Feature::Ptr     pFeature        = iterFeature->first;
        FeatureGlyphSet& featureGlyphSet = iterFeature->second;
        GlyphMap&        glyphs          = featureGlyphSet.glyphs;

        featureGlyphSet.tick.update();

        const ControlPoint::Set& controlPoints = pFeature->getControlPoints();

        std::vector< GlyphMap::iterator >          removals;
        std::vector< ControlPoint::Set::iterator > additions;

        generics::match( glyphs.begin(), glyphs.end(),               // what it is
                         controlPoints.begin(), controlPoints.end(), // what it should be

                         generics::lessthan( generics::first< GlyphMap::const_iterator >(),
                                             generics::deref< ControlPoint::Set::const_iterator >() ),

                         generics::collect( removals ), // rem
                         generics::collect( additions ) // add
        );

        {
            for( std::vector< GlyphMap::iterator >::iterator i = removals.begin(), iEnd = removals.end(); i != iEnd;
                 ++i )
            {
                const GlyphSpec* pControlPoint = ( *i )->first;
                glyphs.erase( *i );
                m_glyphs.erase( pControlPoint );
            }
        }

        {
            for( std::vector< ControlPoint::Set::iterator >::iterator i = additions.begin(), iEnd = additions.end();
                 i != iEnd;
                 ++i )
            {
                ControlPoint* pControlPoint = **i;
                IGlyph::Ptr   pGlyph        = createControlPointGlyph( pControlPoint );
                glyphs.insert( std::make_pair( pControlPoint, pGlyph ) );
            }
        }
    }

    for( Feature::PtrVector::iterator i = removals.begin(), iEnd = removals.end(); i != iEnd; ++i )
    {
        FeatureGlyphMap::iterator iFind = m_features.find( *i );
        VERIFY_RTE( iFind != m_features.end() );

        for( GlyphMap::const_iterator i = iFind->second.glyphs.begin(), iEnd = iFind->second.glyphs.end(); i != iEnd;
             ++i )
        {
            m_glyphs.erase( i->first );
        }
        m_features.erase( iFind );
    }

    struct AddGlyphTask
    {
        Feature::Ptr     pFeature;
        FeatureGlyphSet* pFeatureGlyphSet = nullptr;
    };

    typedef std::vector< AddGlyphTask > AddGlyphTaskVector;
    AddGlyphTaskVector                  tasks;
    for( Feature::PtrVector::iterator i = additions.begin(), iEnd = additions.end(); i != iEnd; ++i )
    {
        // get the glyph from the feature...
        Feature::Ptr pFeature = *i;

        ASSERT( m_features.find( pFeature ) == m_features.end() );

        FeatureGlyphMap::iterator iFind = m_features.end();
        {
            FeatureGlyphSet glyphSet;
            auto            ib = m_features.insert( std::make_pair( pFeature, glyphSet ) );
            VERIFY_RTE( ib.second );
            iFind = ib.first;
        }
        FeatureGlyphSet& glyphSet = iFind->second;

        AddGlyphTask task{
            pFeature, // Feature::Ptr pFeature;
            &glyphSet // FeatureGlyphSet* pFeatureGlyphSet
        };
        tasks.push_back( task );
    }

    bool bMadeProgress = true;
    while( bMadeProgress )
    {
        bMadeProgress = false;

        AddGlyphTaskVector taskSwap;

        for( AddGlyphTaskVector::iterator i = tasks.begin(), iEnd = tasks.end(); i != iEnd; ++i )
        {
            const AddGlyphTask& task = *i;

            const ControlPoint::Set& controlPoints = task.pFeature->getControlPoints();

            if( !controlPoints.empty() )
            {
                ControlPoint* pFirst = *controlPoints.begin();

                IGlyph::Ptr      pParentGlyph;
                const GlyphSpec* pParentGlyphSpec = pFirst->getParent();
                if( pParentGlyphSpec )
                {
                    if( const Origin* pParentOrigin = dynamic_cast< const Origin* >( pParentGlyphSpec ) )
                    {
                        VERIFY_RTE( m_pNode.get() == dynamic_cast< const Site* >( pParentOrigin ) );
                        pParentGlyph = m_pMainGlyph;
                    }
                    else
                    {
                        GlyphMap::const_iterator iFind = m_glyphs.find( pParentGlyphSpec );
                        if( iFind != m_glyphs.end() )
                            pParentGlyph = iFind->second;
                    }
                }

                // see if the glyph parent is created ( or the control point has no parent )
                if( !pParentGlyphSpec || pParentGlyph )
                {
                    for( auto pControlPoint : controlPoints )
                    {
                        IGlyph::Ptr pNewGlyph = m_glyphFactory.createControlPoint( pControlPoint, pParentGlyph );
                        m_glyphs.insert( std::make_pair( pControlPoint, pNewGlyph ) );
                        task.pFeatureGlyphSet->glyphs.insert( std::make_pair( pControlPoint, pNewGlyph ) );
                    }
                    bMadeProgress = true;
                }
                else
                {
                    taskSwap.push_back( *i );
                }
            }
        }
        tasks.swap( taskSwap );
    }
    VERIFY_RTE( tasks.empty() );
}

void EditNested::updateGlyphs()
{
    // recurse
    EditBase::updateGlyphs();

    matchFeatures();

    m_pMainGlyph->update();

    generics::for_each_second( m_glyphs, []( IGlyph::Ptr pGlyph ) { pGlyph->update(); } );
}

bool EditNested::interaction_hover( Float x, Float y, IGlyph*& pPoint1, IGlyph*& pPoint2 )
{
    bool bFound = false;

    if( Site::Ptr pSite = boost::dynamic_pointer_cast< Site >( m_pNode ) )
    {
        if( Feature_Contour::Ptr pContour = pSite->getContour() )
        {
            const Transform siteTransform = pSite->getAbsoluteTransform() * pSite->getTransform();
            const Point     relativePoint = siteTransform.inverse()( Point( x, y ) );

            const Polygon contour = pContour->getPolygon();

            const unsigned int uiIndex     = Utils::getClosestPoint( contour, relativePoint );
            const unsigned int uiIndexNext = ( uiIndex + 1U ) % contour.size();

            const GlyphSpec* pGS1 = nullptr;
            const GlyphSpec* pGS2 = nullptr;
            {
                const ControlPoint::Set& controlPoints = pContour->getControlPoints();
                for( ControlPoint::Set::const_iterator i = controlPoints.begin(), iEnd = controlPoints.end(); i != iEnd;
                     ++i )
                {
                    const ControlPoint::Index index = ( *i )->getIndex();
                    if( index == uiIndex )
                        pGS1 = *i;
                    if( index == uiIndexNext ) // can be the same
                        pGS2 = *i;
                }
            }

            GlyphMap::const_iterator iFind1 = m_glyphs.find( pGS1 );
            GlyphMap::const_iterator iFind2 = m_glyphs.find( pGS2 );

            if( ( iFind1 != m_glyphs.end() ) && ( iFind2 != m_glyphs.end() ) )
            {
                pPoint1 = iFind1->second.get();
                pPoint2 = iFind2->second.get();
                bFound  = true;
            }
        }
    }

    return bFound;
}

const Origin* EditNested::getOrigin() const
{
    return dynamic_cast< const Origin* >( m_pNode.get() );
}

bool EditNested::owns( const GlyphSpec* pGlyphSpec ) const
{
    return m_glyphs.find( pGlyphSpec ) != m_glyphs.end();
}

bool EditNested::owns( IGlyph* pGlyph ) const
{
    if( nullptr != pGlyph )
    {
        if( m_pMainGlyph.get() == pGlyph )
        {
            return true;
        }

        IGlyph::Ptr pGlyphPtr = pGlyph->shared_from_this();
        for( FeatureGlyphMap::const_iterator i = m_features.begin(), iEnd = m_features.end(); i != iEnd; ++i )
        {
            if( i->second.contains( pGlyphPtr ) )
            {
                return true;
            }
        }
    }
    return false;
}

void EditNested::cmd_delete_impl( const std::set< IGlyph* >& selection )
{
    std::vector< const GlyphSpec* > selectionGlyphSpecs;
    for( IGlyph* pGlyph : selection )
    {
        selectionGlyphSpecs.push_back( pGlyph->getGlyphSpec() );
    }

    std::set< GlyphSpecProducer* > glyphSpecProducers;
    for( std::set< IGlyph* >::iterator i = selection.begin(), iEnd = selection.end(); i != iEnd; ++i )
    {
        IGlyph* pGlyph = *i;

        if( const GlyphSpec* pGlyphSpec = pGlyph->getGlyphSpec() )
        {
            if( const GlyphSpecProducer* pGlyphPrd = pGlyphSpec->getProducer() )
            {
                if( !dynamic_cast< const Site* >( pGlyphPrd ) )
                {
                    glyphSpecProducers.insert( const_cast< GlyphSpecProducer* >( pGlyphPrd ) );
                }
            }
        }
    }

    std::set< GlyphSpecProducer* > unhandled;
    for( GlyphSpecProducer* pGlyphSpecProducer : glyphSpecProducers )
    {
        if( !pGlyphSpecProducer->cmd_delete( selectionGlyphSpecs ) )
        {
            unhandled.insert( pGlyphSpecProducer );
        }
    }

    // if no handler delete the selection itself
    for( std::set< GlyphSpecProducer* >::iterator i = unhandled.begin(), iEnd = unhandled.end(); i != iEnd; ++i )
    {
        if( GlyphSpecProducer* pGlyphPrd = *i )
        {
            if( !dynamic_cast< Site* >( pGlyphPrd ) )
            {
                if( Node::Ptr pParent = pGlyphPrd->getParent() )
                {
                    pParent->remove( const_cast< GlyphSpecProducer* >( pGlyphPrd )->getPtr() );
                }
            }
        }
    }

    EditBase::cmd_delete_impl( selection );
}

} // namespace schematic
