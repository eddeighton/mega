
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

    mega::TypeIDSequence operator()( Concrete::Context* pContext ) const
    {
        mega::TypeIDSequence sequence{ getTypeID( pContext->get_interface() ) };
        recurse( pContext->get_parent(), sequence );
        return sequence;
    }

    mega::TypeIDSequence operator()( Concrete::Dimensions::User* pDimUser ) const
    {
        mega::TypeIDSequence sequence{ getTypeID( pDimUser->get_interface_dimension() ) };
        recurse( pDimUser->get_parent_context(), sequence );
        return sequence;
    }

    mega::TypeIDSequence operator()( Concrete::Dimensions::Link* pDimLink ) const
    {
        if( auto pUserLink = db_cast< Concrete::Dimensions::UserLink >( pDimLink ) )
        {
            mega::TypeIDSequence sequence{ getTypeID( pUserLink->get_interface_link() ) };
            recurse( pUserLink->get_parent_context(), sequence );
            return sequence;
        }
        else if( auto pOwnershipLink = db_cast< Concrete::Dimensions::OwnershipLink >( pDimLink ) )
        {
            // just specify the parent context twice
            mega::TypeIDSequence sequence{ getTypeID( pOwnershipLink->get_parent_context()->get_interface() ) };
            recurse( pOwnershipLink->get_parent_context(), sequence );
            return sequence;
        }
        else
        {
            THROW_RTE( "Unknown link type" );
        }
    }

    mega::TypeIDSequence operator()( Concrete::Dimensions::Allocation* pDimAlloc ) const
    {
        if( auto pDimAllocator = db_cast< Concrete::Dimensions::Allocator >( pDimAlloc ) )
        {
            auto pAllocator        = pDimAllocator->get_allocator();
            auto pAllocatedContext = pAllocator->get_allocated_context();

            mega::TypeIDSequence sequence{ getTypeID( pAllocatedContext->get_interface() ) };
            recurse( pAllocatedContext->get_parent(), sequence );

            return sequence;
        }
        else
        {
            THROW_RTE( "Unknown allocator dimension type" );
        }
    }
};