
#include "python_reference.hpp"
#include "python_reference_factory.hpp"

#include "mega/reference_io.hpp"

#include "common/assert_verify.hpp"

//#include "eg/macros.hpp"

#include <pybind11/pybind11.h>

#include <sstream>
#include <exception>

namespace megastructure
{


PythonEGReference::PythonEGReference( PythonEGReferenceFactory& pythonReferenceFactory, const mega::reference& ref ) 
    :   m_pythonReferenceFactory( pythonReferenceFactory ),
        m_reference( ref ) 
{
}
    
PyObject* PythonEGReference::get( void* pClosure )
{
    const char* pszAttributeIdentity = reinterpret_cast< char* >( pClosure );
    const mega::TypeID typeID = m_pythonReferenceFactory.getTypeID( pszAttributeIdentity );
    if( typeID == 0 )
    {
        std::ostringstream os;
        os << "Invalid identity" << pszAttributeIdentity;
        // ERR( os.str() );
        
        Py_INCREF( Py_None );
        return Py_None;
    }
    else
    {
        PyObject* pResult = m_pythonReferenceFactory.create( m_reference );
        {
            PythonEGReference* pNewRef = PythonEGReferenceFactory::getReference( pResult );
            pNewRef->m_type_path.reserve( m_type_path.size() + 1U );
            pNewRef->m_type_path = m_type_path;
            pNewRef->m_type_path.push_back( typeID );
        }
        
        return pResult;
    }
}

int PythonEGReference::set( void* pClosure, PyObject* pValue )
{
    //const char* pszAttributeIdentity = reinterpret_cast< char* >( pClosure );
    return 0;
}

PyObject* PythonEGReference::str() const
{
    std::ostringstream os;
    os << "type: ";
    m_pythonReferenceFactory.printType( m_reference.getType(), os );
    
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
        os << " state: " << mega::getActionState( m_pythonReferenceFactory.getState( m_reference.type, m_reference.instance ) );
    else
        os << " state: null";*/
    return Py_BuildValue( "s", os.str().c_str() );
}

PyObject* PythonEGReference::call( PyObject *args, PyObject *kwargs )
{
    //test the state
    //switch( m_pythonReferenceFactory.getState( m_reference.type, m_reference.instance ) )
    //{
    //    case action_stopped:
    //    case action_running:
    //    case action_paused:
    //    case TOTAL_ACTION_STATES:
    //    default:
    //    break;
    //}
    if( m_reference.is_valid() )
    {
        return m_pythonReferenceFactory.invoke( m_reference, m_type_path, args, kwargs );
    }
    else
    {
        std::ostringstream os;
        os << "Invocation on null reference";
        // ERR( os.str() );
        
        Py_INCREF( Py_None );
        return Py_None;
    }
}



} //namespace megastructure