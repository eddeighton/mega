
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

#ifndef GUARD_2023_March_14_python_mangle
#define GUARD_2023_March_14_python_mangle

#include "common/assert_verify.hpp"

#include <pybind11/pybind11.h>

#include <string>
#include <unordered_map>

namespace mega::mangle
{
    using CppToPythonFunction = PyObject* (*)( void* );
    using PythonToCppFunction = void* (*)( PyObject* );

    using CppToPythonTable = std::unordered_map< std::string, CppToPythonFunction >;
    using PythonToCppTable = std::unordered_map< std::string, PythonToCppFunction >;

    extern void initialise_types( CppToPythonTable& cppToPython, PythonToCppTable& pythonToCpp );

    class PythonMangle
    {
    public:
        PythonMangle()
        {
            initialise_types( m_cppToPython, m_pythonToCpp );
        }

        PyObject* cppToPython( const std::string& strMangle, void* pCppObject ) const
        {
            auto iFind = m_cppToPython.find( strMangle );
            VERIFY_RTE_MSG( iFind != m_cppToPython.end(), "Failed to locate cpp to python conversion for type: " << strMangle );
            return iFind->second( pCppObject );
        }

        void* pythonToCpp( const std::string& strMangle, PyObject* pyObject ) const
        {
            auto iFind = m_pythonToCpp.find( strMangle );
            VERIFY_RTE_MSG( iFind != m_pythonToCpp.end(), "Failed to locate python to cpp conversion for type: " << strMangle );
            return iFind->second( pyObject );
        }

    private:
        CppToPythonTable m_cppToPython;
        PythonToCppTable m_pythonToCpp;
    };
}

#endif //GUARD_2023_March_14_python_mangle
