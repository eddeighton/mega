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

#include "map/clip.hpp"
#include "map/file.hpp"

namespace map
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
const std::string& Clip::TypeName()
{
    static const std::string strTypeName( "clip" );
    return strTypeName;
}

Clip::Clip( const std::string& strName )
    : Schematic( strName )
{
}

Clip::Clip( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
    : Schematic( pOriginal, pParent, strName )
{
}

Node::Ptr Clip::copy( Node::Ptr pParent, const std::string& strName ) const
{
    VERIFY_RTE( !pParent );
    return Node::copy< Clip >( boost::dynamic_pointer_cast< const Clip >( shared_from_this() ), pParent, strName );
}

void Clip::load( const format::File& file )
{
    ReaderLock lock1( m_nodeStructureLock );
    ReaderLock lock2( m_nodeDataLock );

    VERIFY_RTE( file.has_schematic() );
    const format::File::Schematic& schematic = file.schematic();

    VERIFY_RTE( schematic.has_clip() );
    const format::File::Schematic::Clip& clip = schematic.clip();

    Schematic::load( schematic );
}

void Clip::save( format::File& file ) const
{
    WriterLock lock1( m_nodeStructureLock );
    WriterLock lock2( m_nodeDataLock );

    format::File::Schematic&       schematic = *file.mutable_schematic();
    format::File::Schematic::Clip& clip      = *schematic.mutable_clip();

    Schematic::save( schematic );
}

} // namespace map
