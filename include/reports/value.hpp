
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

#include "mega/type_id.hpp"
#include "mega/sub_type_instance.hpp"
#include "mega/type_instance.hpp"
#include "mega/invocation_id.hpp"
#include "mega/mpo.hpp"
#include "mega/reference.hpp"
#include "mega/any.hpp"
#include "mega/types/traits.hpp"
#include "mega/memory.hpp"
#include "mega/operation_id.hpp"
#include "mega/relation_id.hpp"

#include <variant>
#include <string>

namespace mega::reports
{

using Value
    = std::variant< std::string, Any, TypeID, MP, MPO, SubTypeInstance, TypeInstance, InvocationID, BitSet, reference,
                    ReferenceVector, LinkTypeVector, SizeAlignment, OperationID, ExplicitOperationID, RelationID >;

}

#endif // GUARD_2023_October_17_value
