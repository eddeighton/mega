
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

#include "service/python/python_type.hpp"
#include "service/python/python_type_system.hpp"
#include "service/python/python_reference.hpp"

#include "service/network/log.hpp"

namespace mega::service::python
{

namespace
{

typedef struct
{
    PyObject_HEAD PythonReference* pReference;
} PythonReferenceData;

PythonReference* fromPyObject( PyObject* pPyObject )
{
    PythonReferenceData* pLogicalObject = ( PythonReferenceData* )pPyObject;
    return pLogicalObject->pReference;
}

void type_dealloc( PyObject* pPyObject )
{
    if( PythonReference* pReference = fromPyObject( pPyObject ) )
    {
        delete pReference;
        Py_TYPE( pPyObject )->tp_free( ( PyObject* )pPyObject );
    }
    else
    {
        SPDLOG_ERROR( "PythonReference type_dealloc invalid python reference" );
    }
}

PyObject* type_get( PyObject* self, void* pClosure )
{
    if( PythonReference* pRef = fromPyObject( self ) )
    {
        return pRef->get( pClosure );
    }
    else
    {
        SPDLOG_ERROR( "PythonReference type_get invalid python reference" );
        return nullptr;
    }
}

int type_set( PyObject* self, PyObject* pValue, void* pClosure )
{
    if( PythonReference* pRef = fromPyObject( self ) )
    {
        return pRef->set( pClosure, pValue );
    }
    else
    {
        SPDLOG_ERROR( "PythonReference type_set invalid python reference" );
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
        SPDLOG_ERROR( "PythonReference type_str invalid python reference" );
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
        SPDLOG_ERROR( "PythonReference type_call invalid python reference" );
        return nullptr;
    }
}

PyObject* type_dump( PyObject* self )
{
    if( PythonReference* pRef = fromPyObject( self ) )
    {
        return pRef->dump();
    }
    else
    {
        SPDLOG_ERROR( "PythonReference type_dump invalid python reference" );
        return nullptr;
    }
}

static PyMethodDef type_methods[] = {
    { "dump", ( PyCFunction )type_dump, METH_VARARGS, "Dump object data" }, //
    { nullptr }                                                             // Sentinel
};
} // namespace

Type::Type( PythonModule& module, TypeSystem& typeSystem, Type::SymbolTablePtr pSymbolTable, SymbolTablePtr pLinkTable,
            Type::ConcreteObjectTypeSet&& concreteObjects )
    : m_module( module )
    , m_typeSystem( typeSystem )
    , m_pSymbolTable( pSymbolTable )
    , m_pLinkTable( pLinkTable )
    , m_concreteObjectTypes( std::move( concreteObjects ) )
{
    for( const auto& [ id, _ ] : *m_pSymbolTable )
    {
        char*       pszNonConst = const_cast< char* >( id.c_str() );
        PyGetSetDef data = { pszNonConst, ( getter )type_get, ( setter )type_set, pszNonConst, ( void* )pszNonConst };
        m_pythonAttributesData.push_back( data );
    }
    for( const auto& [ id, _ ] : *m_pLinkTable )
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

    static std::string strTypeName = "classmega00reference";

    PyType_Spec spec = {
        strTypeName.c_str(), sizeof( PythonReferenceData ), 0, Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, slots.data() };

    m_pTypeObject = ( PyTypeObject* )PyType_FromSpec( &spec );

    if( PyType_Ready( m_pTypeObject ) < 0 )
    {
        SPDLOG_ERROR( "Type failed to create python type" );
        // set exception
        THROW_RTE( "Failed to create python mega.reference type" );
    }
    else
    {
        Py_INCREF( m_pTypeObject );
        // successfully generated the dynamic type...
        // PyModule_AddObject( pPythonModule, "Host", (PyObject*)&m_type );
        SPDLOG_TRACE( "Successfully registered Python Reference Type" );
    }
}

Type::~Type()
{
    if( m_pTypeObject )
    {
        Py_DECREF( m_pTypeObject );
    }
}

PyObject* Type::createReference( const mega::reference& ref )
{
    PythonReferenceData* pRootObject   = PyObject_New( PythonReferenceData, m_pTypeObject );
    PyObject*            pPythonObject = PyObject_Init( ( PyObject* )pRootObject, m_pTypeObject );
    pRootObject->pReference            = new PythonReference( m_module, *this, ref );
    Py_INCREF( pPythonObject );
    return pPythonObject;
}

inline Type::TypeIDVector Type::append( const Type::TypeIDVector& from, mega::TypeID next )
{
    TypeIDVector newTypePath;
    {
        newTypePath.reserve( from.size() + 1U );
        newTypePath = from;
        newTypePath.push_back( next );
    }
    return newTypePath;
}

PyObject* Type::createReference( const mega::reference& ref, const Type::TypeIDVector& typePath, const char* symbol )
{
    if( auto iFind = m_pLinkTable->find( symbol ); iFind != m_pLinkTable->end() )
    {
        const TypeIDVector   newTypePath   = append( typePath, iFind->second );
        Type::Ptr            pLinkType     = m_typeSystem.getLinkType( ref.getType().getObjectID(), iFind->second );
        PythonReferenceData* pRootObject   = PyObject_New( PythonReferenceData, m_pTypeObject );
        PyObject*            pPythonObject = PyObject_Init( ( PyObject* )pRootObject, m_pTypeObject );
        pRootObject->pReference            = new PythonReference( m_module, *pLinkType, ref, newTypePath );
        Py_INCREF( pPythonObject );
        return pPythonObject;
    }
    else if( auto iFind = m_pSymbolTable->find( symbol ); iFind != m_pSymbolTable->end() )
    {
        // determine new type
        const TypeIDVector   newTypePath   = append( typePath, iFind->second );
        PythonReferenceData* pRootObject   = PyObject_New( PythonReferenceData, m_pTypeObject );
        PyObject*            pPythonObject = PyObject_Init( ( PyObject* )pRootObject, m_pTypeObject );
        pRootObject->pReference            = new PythonReference( m_module, *this, ref, newTypePath );
        Py_INCREF( pPythonObject );
        return pPythonObject;
    }
    else
    {
        std::ostringstream os;
        {
            using ::operator<<;
            os << "Invalid symbol for reference: " << ref;
            if( !typePath.empty() )
            {
                os << " Type Path: ";
                bool bFirst = true;
                for( auto& i : typePath )
                {
                    if( bFirst )
                        bFirst = false;
                    else
                        os << '.';
                    os << i;
                }
            }
            os << " Symbol: " << symbol;
        }

        SPDLOG_ERROR( os.str() );

        Py_INCREF( Py_None );
        return Py_None;
    }
}

mega::reference Type::cast( PyObject* pObject )
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

std::optional< mega::reference > Type::tryCast( PyObject* pObject )
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
