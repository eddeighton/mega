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

#include "schematic/editRoot.hpp"
#include "schematic/file.hpp"

namespace schematic
{

EditRoot::EditRoot( GlyphFactory& glyphFactory, Node::Ptr pNode )
    : EditBase( *this, glyphFactory, pNode )
    , m_pFile( boost::dynamic_pointer_cast< File >( pNode ) )
{
    VERIFY_RTE( m_pFile );

    if( GlyphSpecProducer* pGlyphSpecProducer = dynamic_cast< GlyphSpecProducer* >( m_pNode.get() ) )
    {
        MarkupText::List texts;
        pGlyphSpecProducer->getMarkupTexts( texts );
        for( auto i = texts.begin(), iEnd = texts.end(); i != iEnd; ++i )
        {
            if( IGlyph::Ptr pMarkupGlyph = m_glyphFactory.createMarkupText( *i, IGlyph::Ptr() ) )
                m_markup.insert( std::make_pair( *i, pMarkupGlyph ) );
        }

        MarkupPolygonGroup::List polyGroups;
        pGlyphSpecProducer->getMarkupPolygonGroups( polyGroups );
        for( auto i = polyGroups.begin(), iEnd = polyGroups.end(); i != iEnd; ++i )
        {
            if( IGlyph::Ptr pMarkupGlyph = m_glyphFactory.createMarkupPolygonGroup( *i, IGlyph::Ptr() ) )
                m_markup.insert( std::make_pair( *i, pMarkupGlyph ) );
        }

        ImageSpec::List images;
        pGlyphSpecProducer->getImages( images );
        for( auto i = images.begin(), iEnd = images.end(); i != iEnd; ++i )
        {
            if( IGlyph::Ptr pImageGlyph = m_glyphFactory.createImage( *i, IGlyph::Ptr() ) )
                m_markup.insert( std::make_pair( *i, pImageGlyph ) );
        }

    }
}

} // namespace schematic