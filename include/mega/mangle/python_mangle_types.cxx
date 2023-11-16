
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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>

#include "mega/values/runtime/reference.hpp"

#include "traits.hpp"
#include "python_mangle.hpp"

#include <unordered_map>
#include <string>

#include "service/python/reference.hpp"
#include "service/python/cast.hpp"

namespace mega::mangle
{
    namespace
    {
        static PyObject* cppToPy_unsignedchar( void* pCppObject )
        {
            unsigned char* p = reinterpret_cast< unsigned char* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_unsignedchar( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static unsigned char cppObject;
            cppObject = obj.cast< unsigned char >();
            return &cppObject;
        }
        static PyObject* cppToPy_signedchar( void* pCppObject )
        {
            signed char* p = reinterpret_cast< signed char* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_signedchar( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static signed char cppObject;
            cppObject = obj.cast< signed char >();
            return &cppObject;
        }
        static PyObject* cppToPy_unsignedshort( void* pCppObject )
        {
            unsigned short* p = reinterpret_cast< unsigned short* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_unsignedshort( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static unsigned short cppObject;
            cppObject = obj.cast< unsigned short >();
            return &cppObject;
        }
        static PyObject* cppToPy_short( void* pCppObject )
        {
            short* p = reinterpret_cast< short* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_short( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static short cppObject;
            cppObject = obj.cast< short >();
            return &cppObject;
        }
        static PyObject* cppToPy_unsignedint( void* pCppObject )
        {
            unsigned int* p = reinterpret_cast< unsigned int* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_unsignedint( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static unsigned int cppObject;
            cppObject = obj.cast< unsigned int >();
            return &cppObject;
        }
        static PyObject* cppToPy_int( void* pCppObject )
        {
            int* p = reinterpret_cast< int* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_int( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static int cppObject;
            cppObject = obj.cast< int >();
            return &cppObject;
        }
        static PyObject* cppToPy_unsignedlong( void* pCppObject )
        {
            unsigned long* p = reinterpret_cast< unsigned long* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_unsignedlong( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static unsigned long cppObject;
            cppObject = obj.cast< unsigned long >();
            return &cppObject;
        }
        static PyObject* cppToPy_unsignedlonglong( void* pCppObject )
        {
            unsigned long long* p = reinterpret_cast< unsigned long long* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_unsignedlonglong( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static unsigned long long cppObject;
            cppObject = obj.cast< unsigned long long >();
            return &cppObject;
        }
        static PyObject* cppToPy_long( void* pCppObject )
        {
            long* p = reinterpret_cast< long* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_long( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static long cppObject;
            cppObject = obj.cast< long >();
            return &cppObject;
        }
        static PyObject* cppToPy_longlong( void* pCppObject )
        {
            long long* p = reinterpret_cast< long long* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_longlong( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static long long cppObject;
            cppObject = obj.cast< long long >();
            return &cppObject;
        }
        static PyObject* cppToPy_float( void* pCppObject )
        {
            float* p = reinterpret_cast< float* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_float( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static float cppObject;
            cppObject = obj.cast< float >();
            return &cppObject;
        }
        static PyObject* cppToPy_double( void* pCppObject )
        {
            double* p = reinterpret_cast< double* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_double( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static double cppObject;
            cppObject = obj.cast< double >();
            return &cppObject;
        }
        static PyObject* cppToPy_bool( void* pCppObject )
        {
            bool* p = reinterpret_cast< bool* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_bool( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static bool cppObject;
            cppObject = obj.cast< bool >();
            return &cppObject;
        }
        static PyObject* cppToPy_classmega00TypeID( void* pCppObject )
        {
            class mega::TypeID* p = reinterpret_cast< class mega::TypeID* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_classmega00TypeID( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static class mega::TypeID cppObject;
            cppObject = obj.cast< class mega::TypeID >();
            return &cppObject;
        }
        static PyObject* cppToPy_classmega00reference( void* pCppObject )
        {
            class mega::reference* p = reinterpret_cast< class mega::reference* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_classmega00reference( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static class mega::reference cppObject;
            cppObject = obj.cast< class mega::reference >();
            return &cppObject;
        }
        static PyObject* cppToPy_classstd00vector3classmega00reference4( void* pCppObject )
        {
            class std::vector<class mega::reference>* p = reinterpret_cast< class std::vector<class mega::reference>* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_classstd00vector3classmega00reference4( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static class std::vector<class mega::reference> cppObject;
            cppObject = obj.cast< class std::vector<class mega::reference> >();
            return &cppObject;
        }
        static PyObject* cppToPy_classstd00vector3classmega00TypeID4( void* pCppObject )
        {
            class std::vector<class mega::TypeID>* p = reinterpret_cast< class std::vector<class mega::TypeID>* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_classstd00vector3classmega00TypeID4( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static class std::vector<class mega::TypeID> cppObject;
            cppObject = obj.cast< class std::vector<class mega::TypeID> >();
            return &cppObject;
        }
        static PyObject* cppToPy_classstd00basic_string3char4( void* pCppObject )
        {
            class std::basic_string<char>* p = reinterpret_cast< class std::basic_string<char>* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_classstd00basic_string3char4( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static class std::basic_string<char> cppObject;
            cppObject = obj.cast< class std::basic_string<char> >();
            return &cppObject;
        }
        static PyObject* cppToPy_classstd00vector3int4( void* pCppObject )
        {
            class std::vector<int>* p = reinterpret_cast< class std::vector<int>* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_classstd00vector3int4( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static class std::vector<int> cppObject;
            cppObject = obj.cast< class std::vector<int> >();
            return &cppObject;
        }
        static PyObject* cppToPy_classboost00dynamic_bitset3unsignedlonglong4( void* pCppObject )
        {
            class boost::dynamic_bitset<unsigned long long>* p = reinterpret_cast< class boost::dynamic_bitset<unsigned long long>* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_classboost00dynamic_bitset3unsignedlonglong4( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static class boost::dynamic_bitset<unsigned long long> cppObject;
            cppObject = obj.cast< class boost::dynamic_bitset<unsigned long long> >();
            return &cppObject;
        }
        static PyObject* cppToPy_structF2( void* pCppObject )
        {
            struct F2* p = reinterpret_cast< struct F2* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_structF2( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static struct F2 cppObject;
            cppObject = obj.cast< struct F2 >();
            return &cppObject;
        }
        static PyObject* cppToPy_structF3( void* pCppObject )
        {
            struct F3* p = reinterpret_cast< struct F3* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_structF3( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static struct F3 cppObject;
            cppObject = obj.cast< struct F3 >();
            return &cppObject;
        }
        static PyObject* cppToPy_structF4( void* pCppObject )
        {
            struct F4* p = reinterpret_cast< struct F4* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_structF4( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static struct F4 cppObject;
            cppObject = obj.cast< struct F4 >();
            return &cppObject;
        }
        static PyObject* cppToPy_structQuat( void* pCppObject )
        {
            struct Quat* p = reinterpret_cast< struct Quat* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_structQuat( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static struct Quat cppObject;
            cppObject = obj.cast< struct Quat >();
            return &cppObject;
        }
        static PyObject* cppToPy_structF33( void* pCppObject )
        {
            struct F33* p = reinterpret_cast< struct F33* >( pCppObject );
            auto pyObject = pybind11::cast( *p );
            pyObject.inc_ref();
            return pyObject.ptr();
        }
        static void* pyToCpp_structF33( PyObject* pyObject )
        {
            pybind11::object obj = pybind11::reinterpret_borrow< pybind11::object >( pyObject );
            static struct F33 cppObject;
            cppObject = obj.cast< struct F33 >();
            return &cppObject;
        }
    }

    void initialise_types( CppToPythonTable& cppToPython, PythonToCppTable& pythonToCpp )
    {
        cppToPython.insert( { "unsignedchar", &cppToPy_unsignedchar } );
        pythonToCpp.insert( { "unsignedchar", &pyToCpp_unsignedchar } );
        cppToPython.insert( { "signedchar", &cppToPy_signedchar } );
        pythonToCpp.insert( { "signedchar", &pyToCpp_signedchar } );
        cppToPython.insert( { "unsignedshort", &cppToPy_unsignedshort } );
        pythonToCpp.insert( { "unsignedshort", &pyToCpp_unsignedshort } );
        cppToPython.insert( { "short", &cppToPy_short } );
        pythonToCpp.insert( { "short", &pyToCpp_short } );
        cppToPython.insert( { "unsignedint", &cppToPy_unsignedint } );
        pythonToCpp.insert( { "unsignedint", &pyToCpp_unsignedint } );
        cppToPython.insert( { "int", &cppToPy_int } );
        pythonToCpp.insert( { "int", &pyToCpp_int } );
        cppToPython.insert( { "unsignedlong", &cppToPy_unsignedlong } );
        pythonToCpp.insert( { "unsignedlong", &pyToCpp_unsignedlong } );
        cppToPython.insert( { "unsignedlonglong", &cppToPy_unsignedlonglong } );
        pythonToCpp.insert( { "unsignedlonglong", &pyToCpp_unsignedlonglong } );
        cppToPython.insert( { "long", &cppToPy_long } );
        pythonToCpp.insert( { "long", &pyToCpp_long } );
        cppToPython.insert( { "longlong", &cppToPy_longlong } );
        pythonToCpp.insert( { "longlong", &pyToCpp_longlong } );
        cppToPython.insert( { "float", &cppToPy_float } );
        pythonToCpp.insert( { "float", &pyToCpp_float } );
        cppToPython.insert( { "double", &cppToPy_double } );
        pythonToCpp.insert( { "double", &pyToCpp_double } );
        cppToPython.insert( { "bool", &cppToPy_bool } );
        pythonToCpp.insert( { "bool", &pyToCpp_bool } );
        cppToPython.insert( { "classmega00TypeID", &cppToPy_classmega00TypeID } );
        pythonToCpp.insert( { "classmega00TypeID", &pyToCpp_classmega00TypeID } );
        cppToPython.insert( { "classmega00reference", &cppToPy_classmega00reference } );
        pythonToCpp.insert( { "classmega00reference", &pyToCpp_classmega00reference } );
        cppToPython.insert( { "classstd00vector3classmega00reference4", &cppToPy_classstd00vector3classmega00reference4 } );
        pythonToCpp.insert( { "classstd00vector3classmega00reference4", &pyToCpp_classstd00vector3classmega00reference4 } );
        cppToPython.insert( { "classstd00vector3classmega00TypeID4", &cppToPy_classstd00vector3classmega00TypeID4 } );
        pythonToCpp.insert( { "classstd00vector3classmega00TypeID4", &pyToCpp_classstd00vector3classmega00TypeID4 } );
        cppToPython.insert( { "classstd00basic_string3char4", &cppToPy_classstd00basic_string3char4 } );
        pythonToCpp.insert( { "classstd00basic_string3char4", &pyToCpp_classstd00basic_string3char4 } );
        cppToPython.insert( { "classstd00vector3int4", &cppToPy_classstd00vector3int4 } );
        pythonToCpp.insert( { "classstd00vector3int4", &pyToCpp_classstd00vector3int4 } );
        cppToPython.insert( { "classboost00dynamic_bitset3unsignedlonglong4", &cppToPy_classboost00dynamic_bitset3unsignedlonglong4 } );
        pythonToCpp.insert( { "classboost00dynamic_bitset3unsignedlonglong4", &pyToCpp_classboost00dynamic_bitset3unsignedlonglong4 } );
        cppToPython.insert( { "structF2", &cppToPy_structF2 } );
        pythonToCpp.insert( { "structF2", &pyToCpp_structF2 } );
        cppToPython.insert( { "structF3", &cppToPy_structF3 } );
        pythonToCpp.insert( { "structF3", &pyToCpp_structF3 } );
        cppToPython.insert( { "structF4", &cppToPy_structF4 } );
        pythonToCpp.insert( { "structF4", &pyToCpp_structF4 } );
        cppToPython.insert( { "structQuat", &cppToPy_structQuat } );
        pythonToCpp.insert( { "structQuat", &pyToCpp_structQuat } );
        cppToPython.insert( { "structF33", &cppToPy_structF33 } );
        pythonToCpp.insert( { "structF33", &pyToCpp_structF33 } );
    }

}
