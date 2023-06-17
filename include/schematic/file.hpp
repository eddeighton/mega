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

#ifndef FILE_29_JAN_2021
#define FILE_29_JAN_2021

#include "schematic/node.hpp"
#include "schematic/container.hpp"
#include "schematic/compilation_stage.hpp"
#include "schematic/glyphSpecProducer.hpp"

#include "common/scheduler.hpp"

#include <bitset>
#include <atomic>

namespace schematic
{

class File : public GlyphSpecProducer
{
public:
    using Ptr     = boost::shared_ptr< File >;
    using PtrCst  = boost::shared_ptr< const File >;
    using PtrList = std::list< Ptr >;

    File( const std::string& strName );
    File( File::PtrCst pOriginal, Node::Ptr pNewParent, const std::string& strName );

    virtual void load( const format::Node& node );
    virtual void save( format::Node& node ) const;

    static CompilationStage getDefaultCompilationConfig();
};

} // namespace schematic

#endif // FILE_29_JAN_2021