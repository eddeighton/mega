
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

#ifndef GUARD_2023_December_06_factory
#define GUARD_2023_December_06_factory

#include "il/elements/elements.hpp"

#include "runtime/functor_id.hxx"

namespace mega::runtime
{
    class JITDatabase;
}

namespace mega::il
{
class Factory
{
public:
    FunctionDefinition generate_Object_constructor( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Object_destructor( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Object_binaryLoad( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Object_binarySave( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Object_getFunction( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Object_unParent( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Object_traverse( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Object_linkSize( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Object_linkGet( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Object_anyRead( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Object_anyWrite( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Object_enumerate( runtime::JITDatabase& db, const runtime::FunctorID& functorID );

    FunctionDefinition generate_Relation_Make( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Relation_Break( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Relation_Reset( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Relation_Size( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
    FunctionDefinition generate_Relation_Get( runtime::JITDatabase& db, const runtime::FunctorID& functorID );
};
} // namespace mega::il

#endif // GUARD_2023_December_06_factory
