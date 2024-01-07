
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

#ifndef GUARD_2024_January_07_concrete
#define GUARD_2024_January_07_concrete

namespace Concrete
{
static inline mega::interface::TypeID getInterfaceTypeID( const Concrete::Node* pNode )
{
    if( pNode->get_node_opt().has_value() )
    {
        return pNode->get_node_opt().value()->get_interface_id()->get_type_id();
    }
    else if( db_cast< Concrete::OwnershipLink >( pNode ) )
    {
        return mega::interface::OWNER_TYPE_ID;
    }
    else if( db_cast< Concrete::ActivationBitSet >( pNode ) )
    {
        return mega::interface::STATE_TYPE_ID;
    }
    else
    {
        THROW_RTE( "Unknown concrete node type" );
    }
}

static inline std::string getKind( const Concrete::Node* pNode )
{
    if( pNode->get_node_opt().has_value() )
    {
        return Interface::getKind( pNode->get_node_opt().value() );
    }
    else if( db_cast< Concrete::OwnershipLink >( pNode ) )
    {
        return mega::EG_OWNER;
    }
    else if( db_cast< Concrete::ActivationBitSet >( pNode ) )
    {
        return mega::EG_STATE;
    }
    else
    {
        THROW_RTE( "Unknown concrete node type" );
    }
}
} // namespace Concrete

#endif // GUARD_2024_January_07_concrete
