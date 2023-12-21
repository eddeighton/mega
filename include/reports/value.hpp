
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

#ifndef GUARD_2023_October_17_value
#define GUARD_2023_October_17_value

// #include "database/directories.hpp"
// #include "database/manifest_data.hpp"
// #include "database/component_info.hpp"

#include "mega/values/compilation/interface/object_id.hpp"
#include "mega/values/compilation/interface/relation_id.hpp"
#include "mega/values/compilation/interface/sub_object_id.hpp"
#include "mega/values/compilation/interface/symbol_id.hpp"
#include "mega/values/compilation/interface/type_id.hpp"

#include "mega/values/compilation/concrete/instance.hpp"
#include "mega/values/compilation/concrete/object_id.hpp"
#include "mega/values/compilation/concrete/sub_object_id_instance.hpp"
#include "mega/values/compilation/concrete/sub_object_id.hpp"
#include "mega/values/compilation/concrete/type_id_instance.hpp"
#include "mega/values/compilation/concrete/type_id.hpp"

#include "mega/values/compilation/compilation_configuration.hpp"
#include "mega/values/compilation/megastructure_installation.hpp"
#include "mega/values/compilation/invocation_id.hpp"
#include "mega/values/compilation/size_alignment.hpp"
#include "mega/values/compilation/type_id_sequence.hpp"
#include "mega/values/compilation/icontext_flags.hpp"

#include "mega/values/runtime/pointer.hpp"
#include "mega/values/runtime/pointer_io.hpp"
#include "mega/values/runtime/mpo.hpp"

#include "mega/values/service/logical_thread_id.hpp"
#include "mega/values/service/node.hpp"
#include "mega/values/service/project.hpp"
#include "mega/values/service/program.hpp"
#include "mega/values/service/root_config.hpp"

#include <boost/variant.hpp>
#include <boost/filesystem/path.hpp>

#include <string>

namespace mega::reports
{

// clang-format off
using Value = boost::variant
< 
    std::string, 
    boost::filesystem::path,

    // Database Types
    // mega::io::Directories,

    // Compilation Types
    interface::ObjectID, 
    interface::RelationID, 
    interface::SubObjectID, 
    interface::SymbolID, 
    interface::TypeID, 

    concrete::Instance,
    // concrete::ObjectID,
    // concrete::SubObjectIDInstance,
    // concrete::SubObjectID,
    concrete::TypeIDInstance,
    concrete::TypeID,

    //interface::TypeIDSequence,
    //ConcreteTypeIDSequence,
    // InvocationID, 
    SizeAlignment,
    MegastructureInstallation,
    IContextFlags,

    // Runtime Types
    // MP,
    // MPO,
    // Pointer,

    // Service Types
    mega::network::LogicalThreadID,
    mega::network::Node,
    mega::service::Project,
    mega::service::Program
    //mega::network::RootConfig

>;
// clang-format on

std::string toString( const Value& value );

} // namespace mega::reports

#endif // GUARD_2023_October_17_value
