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

#include "schematic/base.hpp"
#include "schematic/file.hpp"

#include "common/assert_verify.hpp"

namespace schematic
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
const std::string& Base::TypeName()
{
    static const std::string strTypeName( "base" );
    return strTypeName;
}

Base::Base( const std::string& strName )
    : Schematic( strName )
{
}

Base::Base( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Schematic( pOriginal, pParent, strName )
{
}

Node::Ptr Base::copy( Node::Ptr pParent, const std::string& strName ) const
{
    VERIFY_RTE( !pParent );
    return Node::copy< Base >( boost::dynamic_pointer_cast< const Base >( shared_from_this() ), pParent, strName );
}

void Base::load( const format::File& file )
{
    VERIFY_RTE( file.has_schematic() );
    const format::File::Schematic& schematic = file.schematic();

    VERIFY_RTE( schematic.has_base() );
    const format::File::Schematic::Base& base = schematic.base();

    Schematic::load( schematic );
}

void Base::save( format::File& file ) const
{
    format::File::Schematic&       schematic = *file.mutable_schematic();
    format::File::Schematic::Base& base      = *schematic.mutable_base();

    Schematic::save( schematic );
}

} // namespace map
