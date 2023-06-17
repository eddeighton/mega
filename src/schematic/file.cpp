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

#include "schematic/file.hpp"
#include "schematic/schematic.hpp"

namespace schematic
{

File::File( const std::string& strName )
    : GlyphSpecProducer( ( Node::Ptr() ), strName )
{
}

File::File( File::PtrCst pOriginal, Node::Ptr pNewParent, const std::string& strName )
    : GlyphSpecProducer( pOriginal, pNewParent, strName )
{
    VERIFY_RTE( !pNewParent );
}

schematic::CompilationStage File::getDefaultCompilationConfig()
{
    return eStage_SiteContour;
}
void File::load( const format::Node& node )
{
    Node::load( node );
    VERIFY_RTE( node.has_file() );
}

void File::save( format::Node& node ) const
{
    format::Node::File& file = *node.mutable_file();
    Node::save( node );
}

} // namespace schematic
