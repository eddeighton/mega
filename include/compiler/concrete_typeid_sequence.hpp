
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

struct TypeIDSequenceGen
{
    using TypeIDSeqPair = mega::SymbolTraits::TypeIDSequencePair;

    inline void recurse( Concrete::Node* pNode, TypeIDSeqPair& sequence, bool bFoundObject ) const
    {
        if( !bFoundObject )
        {
            if( db_cast< Concrete::Object >( pNode ) )
            {
                bFoundObject = true;
            }
        }
        else
        {
            VERIFY_RTE( !db_cast< Concrete::Object >( pNode ) );
        }
        if( bFoundObject )
        {
            sequence.first.push_back( getInterfaceTypeID( pNode ) );
        }
        else
        {
            sequence.second.push_back( getInterfaceTypeID( pNode ) );
        }

        if( auto pParent = db_cast< Concrete::Node >( pNode->get_parent() ) )
        {
            recurse( pParent, sequence, bFoundObject );
        }
    }

    inline TypeIDSeqPair operator()( Concrete::Node* pNode ) const
    {
        TypeIDSeqPair sequence;
        recurse( pNode, sequence, false );
        if( !sequence.first.empty() )
        {
            std::reverse( sequence.first.begin(), sequence.first.end() );
        }
        std::reverse( sequence.second.begin(), sequence.second.end() );

        return sequence;
    }
};