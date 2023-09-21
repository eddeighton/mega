
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

#ifndef GUARD_2023_September_21_interface
#define GUARD_2023_September_21_interface

template < typename TContext >
inline mega::U64 getSizeTraitSize( const TContext* pInterfaceContext )
{
    mega::U64                              allocationSize = 1U;
    std::optional< Interface::SizeTrait* > sizeTraitOpt   = pInterfaceContext->get_size_trait();
    if( sizeTraitOpt.has_value() )
    {
        allocationSize = sizeTraitOpt.value()->get_size();
        VERIFY_RTE_MSG( allocationSize > 0U, "Invalid size for: " << pInterfaceContext->get_identifier() );
    }
    return allocationSize;
}

static mega::U64 getLocalDomainSize( Concrete::Context* pContext )
{
    if( auto pNamespace = db_cast< Concrete::Namespace >( pContext ) )
    {
        return 1;
    }
    else if( auto pState = db_cast< Concrete::State >( pContext ) )
    {
        return getSizeTraitSize( pState->get_interface_state() );
    }
    else if( auto pEvent = db_cast< Concrete::Event >( pContext ) )
    {
        return getSizeTraitSize( pEvent->get_interface_event() );
    }
    else if( auto pInterupt = db_cast< Concrete::Interupt >( pContext ) )
    {
        return 1;
    }
    else if( auto pFunction = db_cast< Concrete::Function >( pContext ) )
    {
        return 1;
    }
    else if( auto pObject = db_cast< Concrete::Object >( pContext ) )
    {
        return 1;
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }
}

#endif // GUARD_2023_September_21_interface
