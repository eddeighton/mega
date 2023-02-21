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

#include "map/glyph.hpp"

namespace map
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

Transform Origin::getAbsoluteTransform() const
{
    return getRelativeTransform( nullptr );
}

Transform Origin::getRelativeTransform( const Node* pParent ) const
{
    Transform transform( CGAL::IDENTITY );

    const Node* pIter = dynamic_cast< const Node* >( this );
    while( pIter && ( pIter != pParent ) )
    {
        pIter = pIter->Node::getParent().get();

        if( const Origin* pIterOrigin = dynamic_cast< const Origin* >( pIter ) )
        {
            transform = pIterOrigin->getTransform() * transform;
        }
    }
    return transform;
}

} // namespace map