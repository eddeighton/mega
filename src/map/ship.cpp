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

#include "map/ship.hpp"

namespace map
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
const std::string& Ship::TypeName()
{
    static const std::string strTypeName( "ship" );
    return strTypeName;
}

Ship::Ship( const std::string& strName )
    : Schematic( strName )
{
}

Ship::Ship( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Schematic( pOriginal, pParent, strName )
{
}

Node::Ptr Ship::copy( Node::Ptr pParent, const std::string& strName ) const
{
    VERIFY_RTE( !pParent );
    return Node::copy< Ship >( boost::dynamic_pointer_cast< const Ship >( shared_from_this() ), pParent, strName );
}

void Ship::load( const format::File& file )
{
    VERIFY_RTE( file.has_schematic() );
    const format::File::Schematic& schematic = file.schematic();

    VERIFY_RTE( schematic.has_ship() );
    const format::File::Schematic::Ship& ship = schematic.ship();

    Schematic::load( schematic );
}

void Ship::save( format::File& file ) const
{
    format::File::Schematic&       schematic = *file.mutable_schematic();
    format::File::Schematic::Ship& ship      = *schematic.mutable_ship();

    Schematic::save( schematic );
}

} // namespace map
