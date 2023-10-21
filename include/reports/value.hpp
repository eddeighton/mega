
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

// #include "mega/values/compilation/type_id.hpp"
// #include "mega/sub_type_instance.hpp"
// #include "mega/values/compilation/type_instance.hpp"
// #include "mega/values/compilation/invocation_id.hpp"
// #include "mega/values/runtime/mpo.hpp"
// #include "mega/values/runtime/reference.hpp"
// #include "mega/values/compilation/operation_id.hpp"
// #include "mega/values/compilation/relation_id.hpp"
// #include "mega/mangle/traits.hpp"
// 
// #include "mega/values/compilation/type_id.hpp"
// #include "mega/values/runtime/reference_io.hpp"
// #include "mega/values/compilation/invocation_id.hpp"
// #include "mega/values/native_types_io.hpp"
// #include "mega/values/compilation/relation_id.hpp"
// 
// #include <boost/variant.hpp>
// #include <variant>

#include <any>

namespace mega::reports
{
using Value = std::any;
/*
// clang-format off
using CompileTimeIdentities= std::variant
< 
    TypeID, 
    SubTypeInstance, 
    TypeInstance, 
    InvocationID, 
    OperationID, 
    ExplicitOperationID, 
    RelationID 
>;

using RuntimeValue = std::variant
< 
    BitSet, 
    reference, 
    ReferenceVector, 
    LinkTypeVector,
    MP, 
    MPO, 
    AllocationID 
>;
// clang-format on

using Value = std::variant< CompileTimeIdentities, RuntimeValue >;

inline std::string toString( const Value& value )
{
    using ::operator<<;

    std::ostringstream osValue;

    // clang-format off
    struct CompilerTimeVisitor
    {
        std::ostream& os;
        void operator()( const mega::TypeID                 & value ) const { os << value; }
        void operator()( const mega::SubTypeInstance        & value ) const { os << value; }
        void operator()( const mega::TypeInstance           & value ) const { os << value; }
        void operator()( const mega::InvocationID           & value ) const { os << value; }
        void operator()( const mega::OperationID            & value ) const { os << value; }
        void operator()( const mega::ExplicitOperationID    & value ) const { os << value; }
        void operator()( const mega::RelationID             & value ) const { os << value; }
    } compilerTimeVisitor{ osValue };

    struct RuntimeVisitor
    {
        std::ostream& os;
        void operator()( const mega::MP                     & value ) const { os << value; }
        void operator()( const mega::MPO                    & value ) const { os << value; }
        void operator()( const mega::reference              & value ) const { os << value; }
        void operator()( const mega::ReferenceVector        & value ) const { os << value; }
        void operator()( const mega::LinkTypeVector         & value ) const { os << value; }
        void operator()( const mega::BitSet                 & value ) const { os << value; }
        void operator()( const mega::AllocationID           & value ) const { os << value; }
        
    } runtimeVisitor{ osValue };

    struct Visitor
    {
        CompilerTimeVisitor& compilerTimeVisitor;
        RuntimeVisitor&      runtimeVisitor;
        void operator()( const CompileTimeIdentities& value )   const { std::visit( compilerTimeVisitor, value ); }
        void operator()( const RuntimeValue& value )            const { std::visit( runtimeVisitor, value ); }
    } visitor{ compilerTimeVisitor, runtimeVisitor };
    std::visit( visitor, value );
    // clang-format on

    return osValue.str();
}*/

} // namespace mega::reports

#endif // GUARD_2023_October_17_value
