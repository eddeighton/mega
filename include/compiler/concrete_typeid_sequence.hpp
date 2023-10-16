
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

    void recurse( Concrete::ContextGroup* pContextGroup, mega::TypeIDSequence& sequence ) const
    {
        if( auto pContext = db_cast< Concrete::Context >( pContextGroup ) )
        {
            sequence.push_back( getTypeID( pContext->get_interface() ) );
            recurse( pContext->get_parent(), sequence );
        }
    }

    mega::TypeIDSequence operator()( Concrete::Graph::Vertex* pVertex ) const
    {
        if( auto pContext = db_cast< Concrete::Context >( pVertex ) )
        {
            mega::TypeIDSequence sequence{ getTypeID( pContext->get_interface() ) };
            recurse( pContext->get_parent(), sequence );
            return sequence;
        }
        else if( auto pDimUser = db_cast< Concrete::Dimensions::User >( pVertex ) )
        {
            mega::TypeIDSequence sequence{ getTypeID( pDimUser->get_interface_dimension() ) };
            recurse( pDimUser->get_parent_context(), sequence );
            return sequence;
        }
        else if( auto pBitset = db_cast< Concrete::Dimensions::Bitset >( pVertex ) )
        {
            mega::TypeIDSequence sequence{ getTypeID( pBitset->get_interface_compiler_dimension() ) };
            recurse( pBitset->get_parent_object(), sequence );
            return sequence;
        }
        else if( auto pDimLink = db_cast< Concrete::Dimensions::Link >( pVertex ) )
        {
            if( auto pUserLink = db_cast< Concrete::Dimensions::UserLink >( pDimLink ) )
            {
                mega::TypeIDSequence sequence{ getTypeID( pUserLink->get_interface_user_link() ) };
                recurse( pUserLink->get_parent_context(), sequence );
                return sequence;
            }
            else if( auto pOwnershipLink = db_cast< Concrete::Dimensions::OwnershipLink >( pDimLink ) )
            {
                mega::TypeIDSequence sequence{ getTypeID( pOwnershipLink->get_interface_owner_link() ) };
                recurse( pOwnershipLink->get_parent_context(), sequence );
                return sequence;
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
};