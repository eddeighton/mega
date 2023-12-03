
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
    mega::TypeID getTypeID( const Interface::IContext* pIContext ) const { return pIContext->get_interface_id(); }
    mega::TypeID getTypeID( const Interface::DimensionTrait* pIDim ) const { return pIDim->get_interface_id(); }
    mega::TypeID getTypeID( const Interface::LinkTrait* pILink ) const { return pILink->get_interface_id(); }

    using TypeIDSeqPair = mega::SymbolTraits::TypeIDSequencePair;

    void recurse( Concrete::Context* pContext, TypeIDSeqPair& sequence, bool bFoundObject ) const
    {
        if( !bFoundObject )
        {
            if( db_cast< Concrete::Object >( pContext ) )
            {
                bFoundObject = true;
            }
        }
        else
        {
            VERIFY_RTE( !db_cast< Concrete::Object >( pContext ) );
        }
        if( bFoundObject )
        {
            sequence.first.push_back( getTypeID( pContext->get_interface() ) );
        }
        else
        {
            sequence.second.push_back( getTypeID( pContext->get_interface() ) );
        }

        if( auto pParent = db_cast< Concrete::Context >( pContext->get_parent() ) )
        {
            recurse( pParent, sequence, bFoundObject );
        }
    }

    TypeIDSeqPair operator()( Concrete::Graph::Vertex* pVertex ) const
    {
        TypeIDSeqPair sequence;
        {
            if( auto pContext = db_cast< Concrete::Context >( pVertex ) )
            {
                recurse( pContext, sequence, false );
            }
            else if( auto pDimUser = db_cast< Concrete::Dimensions::User >( pVertex ) )
            {
                sequence.second.push_back( getTypeID( pDimUser->get_interface_dimension() ) );
                recurse( pDimUser->get_parent_context(), sequence, false );
            }
            else if( auto pBitset = db_cast< Concrete::Dimensions::Bitset >( pVertex ) )
            {
                sequence.second.push_back( getTypeID( pBitset->get_interface_compiler_dimension() ) );
                recurse( pBitset->get_parent_object(), sequence, false  );
            }
            else if( auto pDimLink = db_cast< Concrete::Dimensions::Link >( pVertex ) )
            {
                if( auto pUserLink = db_cast< Concrete::Dimensions::UserLink >( pDimLink ) )
                {
                    sequence.second.push_back( getTypeID( pUserLink->get_interface_user_link() ) );
                    recurse( pUserLink->get_parent_context(), sequence, false );
                }
                else if( auto pOwnershipLink = db_cast< Concrete::Dimensions::OwnershipLink >( pDimLink ) )
                {
                    sequence.second.push_back( getTypeID( pOwnershipLink->get_interface_owner_link() ) );
                    recurse( pOwnershipLink->get_parent_context(), sequence, false );
                }
                else
                {
                    THROW_RTE( "Unknown link type" );
                }
            }
            else
            {
                THROW_RTE( "Unknown vertex type" );
            }
        }

        if( sequence.first.empty() )
        {
            sequence.first.push_back( mega::NULL_SYMBOL_ID );
        }
        else
        {
            std::reverse( sequence.first.begin(), sequence.first.end() );
        }
        std::reverse( sequence.second.begin(), sequence.second.end() );

        return sequence;
    }
};