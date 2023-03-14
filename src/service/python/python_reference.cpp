
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

#include "python_reference.hpp"

#include "module.hpp"

#include "jit/invocation_functions.hxx"
#include "jit/jit_exception.hpp"

#include "mega/reference_io.hpp"
#include "mega/invocation_id.hpp"

#include "mega/types/python_mangle.hpp"

#include "service/network/log.hpp"

#include "common/assert_verify.hpp"

#include <sstream>

namespace mega::service::python
{
namespace
{

typedef struct
{
    PyObject_HEAD PythonReference* pReference;
} PythonReferenceData;

static PythonReference* fromPyObject( PyObject* pPyObject )
{
    // if( pPyObject->ob_type == PythonReference::Registration::getTypeObject() )
    {
        PythonReferenceData* pLogicalObject = ( PythonReferenceData* )pPyObject;
        return pLogicalObject->pReference;
    }
    /*else
    {
        return nullptr;
    }*/
}

static void type_dealloc( PyObject* pPyObject )
{
    if( PythonReference* pReference = fromPyObject( pPyObject ) )
    {
        delete pReference;
        Py_TYPE( pPyObject )->tp_free( ( PyObject* )pPyObject );
    }
}

static PyObject* type_get( PyObject* self, void* pClosure )
{
    if( PythonReference* pRef = fromPyObject( self ) )
    {
        return pRef->get( pClosure );
    }
    else
    {
        // PYTHON_ERROR( "PythonEGReferenceFactory is out of date" );
        return nullptr;
    }
}

static int type_set( PyObject* self, PyObject* pValue, void* pClosure )
{
    if( PythonReference* pRef = fromPyObject( self ) )
    {
        return pRef->set( pClosure, pValue );
    }
    else
    {
        // PYTHON_ERROR( "PythonEGReferenceFactory is out of date" );
        return -1;
    }
}

PyObject* type_str( PyObject* self )
{
    if( PythonReference* pRef = fromPyObject( self ) )
    {
        return pRef->str();
    }
    else
    {
        // PYTHON_ERROR( "PythonEGReferenceFactory is out of date" );
        return nullptr;
    }
}

PyObject* type_call( PyObject* callable, PyObject* args, PyObject* kwargs )
{
    if( PythonReference* pRef = fromPyObject( callable ) )
    {
        return pRef->call( args, kwargs );
    }
    else
    {
        // PYTHON_ERROR( "PythonEGReferenceFactory is out of date" );
        return nullptr;
    }
}

static PyMethodDef type_methods[] = {
    //{"sample", (PyCFunction)type_sample, METH_VARARGS, "Sample the object and its subtree" },
    //{"assign", (PyCFunction)type_assign, METH_VARARGS, "Assign the object to a sample" },
    //{"update", (PyCFunction)type_update, METH_NOARGS, "Update objects associated gpu buffers" },
    { nullptr } /* Sentinel */
};
} // namespace

PythonReference::Registration::Registration( const SymbolTable& symbols )
{
    m_symbols = symbols;
    for( const auto& [ id, _ ] : m_symbols )
    {
        char*       pszNonConst = const_cast< char* >( id.c_str() );
        PyGetSetDef data = { pszNonConst, ( getter )type_get, ( setter )type_set, pszNonConst, ( void* )pszNonConst };
        m_pythonAttributesData.push_back( data );
    }
    m_pythonAttributesData.push_back( PyGetSetDef{ nullptr } );

    // generate heap allocated python type...
    std::vector< PyType_Slot > slots;
    {
        slots.push_back( PyType_Slot{ Py_tp_str, reinterpret_cast< void* >( &type_str ) } );
        slots.push_back( PyType_Slot{ Py_tp_repr, reinterpret_cast< void* >( &type_str ) } );
        slots.push_back( PyType_Slot{ Py_tp_dealloc, reinterpret_cast< void* >( &type_dealloc ) } );
        slots.push_back( PyType_Slot{ Py_tp_call, reinterpret_cast< void* >( &type_call ) } );
        slots.push_back( PyType_Slot{ Py_tp_methods, reinterpret_cast< void* >( &type_methods ) } );
        slots.push_back( PyType_Slot{ Py_tp_getset, m_pythonAttributesData.data() } );

        // Py_tp_getattr
        // Py_tp_getattro
    }

    slots.push_back( PyType_Slot{ 0 } );

    static std::string strTypeName = "megastructure.reference";

    PyType_Spec spec = {
        strTypeName.c_str(), sizeof( PythonReferenceData ), 0, Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, slots.data() };

    m_pTypeObject = ( PyTypeObject* )PyType_FromSpec( &spec );

    if( PyType_Ready( m_pTypeObject ) < 0 )
    {
        // set exception
        THROW_RTE( "Failed to create python egreference type" );
    }
    else
    {
        Py_INCREF( m_pTypeObject );
        // successfully generated the dynamic type...
        // PyModule_AddObject( pPythonModule, "Host", (PyObject*)&m_type );

        SPDLOG_INFO( "Successfully registered Python Reference Type" );
    }
}

PythonReference::Registration::~Registration()
{
    if( m_pTypeObject )
    {
        Py_DECREF( m_pTypeObject );
    }
}

mega::TypeID PythonReference::Registration::getTypeID( const char* pszIdentity ) const
{
    auto iFind = m_symbols.find( pszIdentity );
    if( iFind != m_symbols.end() )
    {
        return iFind->second;
    }
    else
    {
        return {};
    }
}

PythonReference::PythonReference( PythonModule& module, const mega::reference& ref )
    : m_module( module )
    , m_reference( ref )
{
}

PyObject* PythonReference::get( void* pClosure )
{
    const char* pszAttributeIdentity = reinterpret_cast< char* >( pClosure );
    SPDLOG_TRACE( "PythonReference::get: {}", pszAttributeIdentity );

    const mega::TypeID typeID = m_module.getPythonRegistration().getTypeID( pszAttributeIdentity );

    if( typeID == mega::TypeID{} )
    {
        SPDLOG_TRACE( "PythonReference::get invalid symbol: {}", pszAttributeIdentity );

        std::ostringstream os;
        os << "Invalid identity" << pszAttributeIdentity;
        // ERR( os.str() );

        Py_INCREF( Py_None );
        return Py_None;
    }
    else
    {
        PyObject* pResult = cast( m_module, m_reference );
        {
            PythonReference* pNewRef = fromPyObject( pResult );
            ASSERT( pNewRef );
            pNewRef->m_type_path.reserve( m_type_path.size() + 1U );
            pNewRef->m_type_path = m_type_path;
            pNewRef->m_type_path.push_back( typeID );
        }

        return pResult;
    }
}
int PythonReference::set( void* pClosure, PyObject* pValue )
{
    // const char* pszAttributeIdentity = reinterpret_cast< char* >( pClosure );
    return 0;
}
PyObject* PythonReference::str() const
{
    SPDLOG_TRACE( "PythonReference::str: {}", m_reference );

    std::ostringstream os;
    // m_module.print( m_reference, os );
    using ::operator<<;
    os << m_reference;
    /*for( std::vector< mega::TypeID >::const_iterator
        i = m_type_path.begin(), iEnd = m_type_path.end(); i!=iEnd; ++i )
    {
        if( i == m_type_path.begin() )
        {
            os << " type path: ";
        }
        os << *i << " ";
    }*/
    /*if( m_reference.type > 0 )
        os << " state: " << mega::getActionState( m_pythonReferenceFactory.getState( m_reference.type,
    m_reference.instance ) ); else os << " state: null";*/
    return Py_BuildValue( "s", os.str().c_str() );
}

PyObject* PythonReference::call( PyObject* args, PyObject* kwargs )
{
    SPDLOG_TRACE( "PythonReference::call" );
    if( m_reference.is_valid() )
    {
        try
        {
            pybind11::args pyArgs = pybind11::reinterpret_borrow< pybind11::args >( args );

            mega::OperationID operationID = pyArgs.size() ? mega::id_Imp_Params : mega::id_Imp_NoParams;
            {
                if( !m_type_path.empty() )
                {
                    mega::TypeID last = m_type_path.back();
                    if( mega::isOperationType( last ) )
                    {
                        operationID = static_cast< mega::OperationID >( static_cast< TypeID::ValueType >( last ) );
                    }
                }
            }

            const mega::InvocationID invocationID{ { m_reference.getType() }, m_type_path, operationID };

            SPDLOG_TRACE( "PythonReference::call: {}", invocationID );

            const PythonModule::FunctionInfo& functionInfo = m_module.invoke( invocationID );
            SPDLOG_TRACE( "PythonReference::call mangle: {}", functionInfo.typeInfo.mangledType );
            switch( functionInfo.typeInfo.operationType )
            {
                case id_exp_Read:
                {
                    auto pReadFunction
                        = reinterpret_cast< mega::runtime::invocation::Read::FunctionPtr >( functionInfo.pFunctionPtr );

                    SPDLOG_TRACE(
                        "PythonReference::call read calling function: {}", functionInfo.typeInfo.mangledType );
                    void* pResult = pReadFunction( m_reference );
                    SPDLOG_TRACE(
                        "PythonReference::call read converting result: {}", functionInfo.typeInfo.mangledType );
                    return m_module.getPythonMangle().cppToPython( functionInfo.typeInfo.mangledType, pResult );
                }
                case id_exp_Write:
                {
                    auto pWriteFunction = reinterpret_cast< mega::runtime::invocation::Write::FunctionPtr >(
                        functionInfo.pFunctionPtr );

                    pybind11::object firstArg = pyArgs[ 0 ];

                    void* pArg
                        = m_module.getPythonMangle().pythonToCpp( functionInfo.typeInfo.mangledType, firstArg.ptr() );

                    const mega::reference result = pWriteFunction( m_reference, pArg );

                    return cast( m_module, result );
                }
                case id_exp_Read_Link:
                case id_exp_Write_Link:
                case id_exp_Allocate:
                case id_exp_Call:
                case id_exp_Start:
                case id_exp_Stop:
                case id_exp_Save:
                case id_exp_Load:
                case id_exp_Files:
                case id_exp_GetAction:
                case id_exp_GetDimension:
                case id_exp_Done:
                case id_exp_Range:
                case id_exp_Raw:
                case HIGHEST_EXPLICIT_OPERATION_TYPE:
                    THROW_RTE( "Unsupported operation type: " << functionInfo.typeInfo.operationType );
                    break;
            }
        }
        catch( mega::runtime::JITException& ex )
        {
            SPDLOG_ERROR( "JIT Exception: {}", ex.what() );
        }
        catch( mega::io::DatabaseVersionException& ex )
        {
            SPDLOG_ERROR( "Database version exception: {}", ex.what() );
        }
        catch( std::exception& ex )
        {
            SPDLOG_ERROR( "Exception: {}", ex.what() );
        }

        SPDLOG_TRACE( "PythonReference::call returning Py_None" );

        Py_INCREF( Py_None );
        return Py_None;
    }
    else
    {
        SPDLOG_TRACE( "PythonReference::call reference invalid" );
        std::ostringstream os;
        os << "Invocation on null reference";
        // ERR( os.str() );

        Py_INCREF( Py_None );
        return Py_None;
    }
}

PyObject* PythonReference::cast( PythonModule& module, const mega::reference& ref )
{
    auto                 pTypeObject   = module.getPythonRegistration().getTypeObject();
    PythonReferenceData* pRootObject   = PyObject_New( PythonReferenceData, pTypeObject );
    PyObject*            pPythonObject = PyObject_Init( ( PyObject* )pRootObject, pTypeObject );
    pRootObject->pReference            = new PythonReference( module, ref );
    Py_INCREF( pPythonObject );
    return pPythonObject;
}

mega::reference PythonReference::cast( PyObject* pObject )
{
    if( PythonReference* pRef = fromPyObject( pObject ) )
    {
        return pRef->getReference();
    }
    else
    {
        return {};
    }
}

} // namespace mega::service::python