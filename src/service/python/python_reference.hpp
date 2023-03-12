
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

#include "mega/reference.hpp"

#include <pybind11/pybind11.h>

#include <vector>

namespace mega::service::python
{

class PythonModule;

class PythonReference
{
public:
    using TypePath = std::vector< mega::TypeID >;

    class Registration
    {
    public:
        Registration();
        ~Registration();

        static PyTypeObject* getTypeObject() { return m_pTypeObject; }

    private:
        static std::vector< PyGetSetDef > m_pythonAttributesData;
        static PyTypeObject*              m_pTypeObject;
    };

    PythonReference( PythonModule& module, const mega::reference& ref );

    PyObject* get( void* pClosure );
    int       set( void* pClosure, PyObject* pValue );
    PyObject* str() const;
    PyObject* call( PyObject* args, PyObject* kwargs );

    const mega::reference getReference() const { return m_reference; }

    static PyObject*       cast( PythonModule& module, const mega::reference& ref );
    static mega::reference cast( PyObject* pObject );

private:
    PythonModule&   m_module;
    mega::reference m_reference;
    TypePath        m_type_path;
};

} // namespace mega::service::python

#endif // GUARD_2023_March_12_python_reference
