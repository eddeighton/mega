
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

namespace Concrete
{
    static mega::TypeID getConcreteInterfaceTypeID( Graph::Vertex* pVertex )
    {
        if( auto pContext = db_cast< const Context >( pVertex ) )
        {
            return pContext->get_interface()->get_interface_id();
        }
        else if( auto pContextGroup = db_cast< const ContextGroup >( pVertex ) )
        {
            return {};
        }
        else if( auto pLink = db_cast< const Dimensions::Link >( pVertex ) )
        {
            return pLink->get_interface_link()->get_interface_id();
        }
        else if( auto pDim = db_cast< const Dimensions::User >( pVertex ) )
        {
            return pDim->get_interface_dimension()->get_interface_id();
        }
        else if( auto pBitset = db_cast< const Dimensions::Bitset >( pVertex ) )
        {
            return pBitset->get_interface_compiler_dimension()->get_interface_id();
        }
        else
        {
            THROW_RTE( "Unknown vertex type" );
        }
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

}