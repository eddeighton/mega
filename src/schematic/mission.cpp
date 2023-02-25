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

#include "schematic/mission.hpp"

namespace schematic
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
const std::string& Mission::TypeName()
{
    static const std::string strTypeName( "map" );
    return strTypeName;
}

Mission::Mission( const std::string& strName )
    : BaseType( strName )
{
}

Mission::Mission( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : BaseType( pOriginal, pParent, strName )
{
}

Node::Ptr Mission::copy( Node::Ptr pParent, const std::string& strName ) const
{
    VERIFY_RTE( !pParent );
    return Node::copy< Mission >(
        boost::dynamic_pointer_cast< const Mission >( shared_from_this() ), pParent, strName );
}

task::Schedule::Ptr Mission::createSchedule( const CompilationConfig& config )
{
    return task::Schedule::Ptr( new task::Schedule( {} ) );
}

void Mission::load( const format::File& file )
{
    THROW_TODO;
    // VERIFY_RTE( file.has_mission() );
    // const format::File::Mission& map = file.map();
}

void Mission::save( format::File& file ) const
{
    THROW_TODO;
    // format::File::Mission& map = *file.mutable_mission();
}

} // namespace schematic
