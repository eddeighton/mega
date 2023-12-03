
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

#ifndef GUARD_2023_March_12_python_reference
#define GUARD_2023_March_12_python_reference

#include "mega/values/runtime/reference.hpp"

#include "runtime/function_ptr.hpp"

#include <pybind11/pybind11.h>

#include <vector>
#include <string>
#include <unordered_map>

namespace mega::service::python
{

class PythonModule;
class Type;

class PythonReference
{
public:
    using TypePath              = std::vector< mega::TypeID >;
    using PythonWrapperFunction = PyObject* ( * )( mega::runtime::CallResult&, const pybind11::args& );

    PythonReference( PythonModule& module, Type& type, const mega::reference& ref );
    PythonReference( PythonModule& module, Type& type, const mega::reference& ref, const TypePath& typePath );

    PyObject* get( void* pClosure );
    int       set( void* pClosure, PyObject* pValue );
    PyObject* str() const;
    PyObject* dump() const;
    PyObject* call( PyObject* args, PyObject* kwargs );

    inline const mega::reference& getReference() const { return m_reference; }

private:
    PythonModule&   m_module;
    Type&           m_type;
    mega::reference m_reference;
    TypePath        m_type_path;
};

} // namespace mega::service::python

#endif // GUARD_2023_March_12_python_reference
