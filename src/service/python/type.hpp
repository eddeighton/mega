
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

#ifndef GUARD_2023_September_02_python_type
#define GUARD_2023_September_02_python_type

#include "mega/values/compilation/concrete/type_id.hpp"
#include "mega/values/compilation/interface/type_id.hpp"
#include "mega/values/runtime/pointer.hpp"

#include <pybind11/pybind11.h>

#include <memory>
#include <set>

namespace mega::service::python
{

class PythonModule;
class TypeSystem;

class Type
{
public:
    using Ptr                   = std::shared_ptr< Type >;
    using SymbolTable           = std::unordered_map< std::string, TypeID >;
    using SymbolTablePtr        = std::shared_ptr< SymbolTable >;
    using ConcreteObjectTypeSet = std::set< mega::SubType >;
    using TypeIDVector          = std::vector< mega::TypeID >;

    Type( PythonModule& module, TypeSystem& typeSystem, SymbolTablePtr pSymbolTable, SymbolTablePtr pLinkTable,
          ConcreteObjectTypeSet&& concreteObjects );
    ~Type();

    PyObject* createReference( const mega::runtime::Pointer& ref );
    PyObject* createReference( const mega::runtime::Pointer& ref, const TypeIDVector& typePath, const char* symbol );

private:
    static TypeIDVector append( const TypeIDVector& from, mega::TypeID next );

    PythonModule&              m_module;
    TypeSystem&                m_typeSystem;
    SymbolTablePtr             m_pSymbolTable;
    SymbolTablePtr             m_pLinkTable;
    ConcreteObjectTypeSet      m_concreteObjectTypes;
    std::vector< PyGetSetDef > m_pythonAttributesData;
    PyTypeObject*              m_pTypeObject;
};

} // namespace mega::service::python

#endif // GUARD_2023_September_02_python_type
