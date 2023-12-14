
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

#ifndef GUARD_2023_September_14_python_cast
#define GUARD_2023_September_14_python_cast

#include "mega/values/runtime/pointer.hpp"
#include "service/python/pointer.hpp"

#include <pybind11/pybind11.h>

#include <optional>

namespace mega::service::python
{

typedef struct
{
    PyObject_HEAD PythonPointer* pReference;
} PythonReferenceData;

inline PythonPointer* fromPyObject( PyObject* pPyObject )
{
    PythonReferenceData* pLogicalObject = ( PythonReferenceData* )pPyObject;
    return pLogicalObject->pReference;
}

inline const mega::Pointer& cast( PyObject* pObject )
{
    return fromPyObject( pObject )->getReference();
}

inline std::optional< Pointer > tryCast( PyObject* pObject )
{
    if( PythonPointer* pRef = fromPyObject( pObject ) )
    {
        return pRef->getReference();
    }
    else
    {
        return {};
    }
}

struct IPythonModuleCast
{
    virtual PyObject* cast( const mega::Pointer& ref ) = 0;
};
PyObject* cast( const mega::Pointer& ref );

} // namespace mega::service::python

template < typename T >
concept IsReferenceType = std::is_base_of< mega::Pointer, T >::value;

namespace PYBIND11_NAMESPACE
{
namespace detail
{
template <>
struct type_caster< mega::Pointer >
{
public:
    /**
     * This macro establishes the name 'inty' in
     * function signatures and declares a local variable
     * 'value' of type inty
     */
    PYBIND11_TYPE_CASTER( mega::Pointer, const_name( "classmega00reference" ) );

    /**
     * Conversion part 1 (Python->C++): convert a PyObject into a inty
     * instance or return false upon failure. The second argument
     * indicates whether implicit conversions should be applied.
     */
    bool load( handle src, bool )
    {
        /* Extract PyObject from handle */
        PyObject* source = src.ptr();
        if( !source )
            return false;

        /* Now try to convert into a C++ int */
        value = mega::service::python::cast( source );

        /* Ensure return code was OK (to avoid out-of-range errors etc) */
        // return !( value != mega::Pointer{} && !PyErr_Occurred() );
        return !PyErr_Occurred();
    }

    /**
     * Conversion part 2 (C++ -> Python): convert an inty instance into
     * a Python object. The second and third arguments are used to
     * indicate the return value policy and parent object (for
     * ``return_value_policy::reference_internal``) and are generally
     * ignored by implicit casters.
     */
    static handle cast( mega::Pointer src, return_value_policy /* policy */, handle /* parent */ )
    {
        return mega::service::python::cast( src );
    }
};

template < IsReferenceType TReferenceType >
struct type_caster< TReferenceType >
{
public:
    /**
     * This macro establishes the name 'inty' in
     * function signatures and declares a local variable
     * 'value' of type inty
     */
    PYBIND11_TYPE_CASTER( TReferenceType, const_name( "classmega00reference" ) );

    /**
     * Conversion part 1 (Python->C++): convert a PyObject into a inty
     * instance or return false upon failure. The second argument
     * indicates whether implicit conversions should be applied.
     */
    bool load( handle src, bool )
    {
        /* Extract PyObject from handle */
        PyObject* source = src.ptr();
        if( !source )
            return false;

        /* Now try to convert into a C++ int */
        value = mega::service::python::cast( source );

        /* Ensure return code was OK (to avoid out-of-range errors etc) */
        return !PyErr_Occurred();
    }

    /**
     * Conversion part 2 (C++ -> Python): convert an inty instance into
     * a Python object. The second and third arguments are used to
     * indicate the return value policy and parent object (for
     * ``return_value_policy::reference_internal``) and are generally
     * ignored by implicit casters.
     */
    static handle cast( TReferenceType src, return_value_policy /* policy */, handle /* parent */ )
    {
        return mega::service::python::cast( src );
    }
};

} // namespace detail
} // namespace PYBIND11_NAMESPACE

#endif // GUARD_2023_September_14_python_cast
