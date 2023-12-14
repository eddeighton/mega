
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

#include "pointer.hpp"
#include "type_system.hpp"
#include "cast.hpp"

#include "module.hpp"

// #include "jit/invocation_functions.hxx"
#include "runtime/function_ptr.hpp"
#include "runtime/exception.hpp"

#include "service/mpo_visitor.hpp"

#include "mega/values/runtime/pointer_io.hpp"
#include "mega/values/compilation/invocation_id.hpp"
#include "mega/mangle/traits.hpp"

#include "mega/mangle/python_mangle.hpp"
#include "mega/iterator.hpp"
#include "mega/logical_tree.hpp"
#include "mega/printer.hpp"

#include "service/network/log.hpp"

#include "common/assert_verify.hpp"

#include <sstream>

namespace mega::service::python
{

PythonPointer::PythonPointer( PythonModule& module, Type& type, const mega::Pointer& ref )
    : m_module( module )
    , m_type( type )
    , m_reference( ref )
{
}

PythonPointer::PythonPointer( PythonModule&          module,
                                  Type&                  type,
                                  const mega::Pointer& ref,
                                  const TypePath&        typePath )
    : m_module( module )
    , m_type( type )
    , m_reference( ref )
    , m_type_path( typePath )
{
}

PyObject* PythonPointer::get( void* pClosure )
{
    return m_type.createReference( m_reference, m_type_path, reinterpret_cast< char* >( pClosure ) );
}

int PythonPointer::set( void* pClosure, PyObject* pValue )
{
    const char* pszAttributeIdentity = reinterpret_cast< char* >( pClosure );
    SPDLOG_ERROR( "PythonPointer::set called with symbol: {}", pszAttributeIdentity );
    return 0;
}

PyObject* PythonPointer::str() const
{
    std::ostringstream os;
    using ::           operator<<;
    os << m_reference.getNetworkAddress();
    return Py_BuildValue( "s", os.str().c_str() );
}

PyObject* PythonPointer::dump() const
{
    std::ostringstream os;

    m_module.invoke(
        [ &m_reference = m_reference, &os ]( MPOContext& )
        {
            MPORealToLogicalVisitor mpoRealInstantiation( m_reference );
            LogicalTreePrinter      printer( os );
            LogicalTreeTraversal    objectTraversal( mpoRealInstantiation, printer );
            traverse( objectTraversal );
        } );

    return Py_BuildValue( "s", os.str().c_str() );
}

PyObject* PythonPointer::call( PyObject* args, PyObject* kwargs )
{
    if( m_reference.valid() )
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

            const mega::TypeID interfaceTypeID = m_module.getInterfaceTypeID( m_reference.getType() );

            const mega::InvocationID invocationID{ { interfaceTypeID }, m_type_path, operationID };

            const PythonModule::InvocationInfo& functionInfo = m_module.invoke( invocationID );

            //SPDLOG_TRACE( "PythonPointer::call: {} {}", invocationID,
            //              mega::getExplicitOperationString( functionInfo.typeInfo.operationType ) );
            THROW_TODO;
            /*switch( functionInfo.typeInfo.operationType )
            {
                case id_exp_Read:
                {
                    void* pResult = nullptr;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &pResult ]( MPOContext& )
                        {
                            auto pFunction = reinterpret_cast< mega::runtime::invocation::Read::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            pResult = pFunction( m_reference );
                        } );
                    if( pResult )
                    {
                        return m_module.getPythonMangle().cppToPython( functionInfo.typeInfo.mangledType, pResult );
                    }
                }
                break;
                case id_exp_Write:
                {
                    VERIFY_RTE_MSG( pyArgs.size() > 0, "Write requires atleast one argument" );
                    pybind11::object firstArg = pyArgs[ 0 ];
                    void*            pArg
                        = m_module.getPythonMangle().pythonToCpp( functionInfo.typeInfo.mangledType, firstArg.ptr() );

                    mega::Pointer result;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &pArg, &result ]( MPOContext& )
                        {
                            auto pWriteFunction = reinterpret_cast< mega::runtime::invocation::Write::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            result = pWriteFunction( m_reference, pArg );
                        } );
                    return m_module.getTypeSystem().cast( result );
                }
                break;
                case id_exp_Link_Read:
                {
                    void* pResult = nullptr;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &pResult ]( MPOContext& )
                        {
                            auto pFunction = reinterpret_cast< mega::runtime::invocation::LinkRead::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            pResult = pFunction( m_reference );
                        } );
                    if( pResult )
                    {
                        return m_module.getPythonMangle().cppToPython( functionInfo.typeInfo.mangledType, pResult );
                    }
                }
                break;
                case id_exp_Link_Add:
                {
                    VERIFY_RTE_MSG( pyArgs.size() > 0, "Link Add requires one argument" );
                    pybind11::object firstArg = pyArgs[ 0 ];
                    auto             refOpt   = tryCast( firstArg.ptr() );
                    VERIFY_RTE_MSG( refOpt.has_value(), "Link Add requires a Pointer parameter" );

                    mega::Pointer refArg = refOpt.value();
                    mega::Pointer result;

                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &refArg, &result ]( MPOContext& )
                        {
                            auto pFunction = reinterpret_cast< mega::runtime::invocation::LinkAdd::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            result = pFunction( m_reference, refArg );
                        } );

                    return m_module.getTypeSystem().cast( result );
                }
                break;
                case id_exp_Link_Remove:
                {
                    VERIFY_RTE_MSG( pyArgs.size() > 0, "Link Remove requires one argument" );
                    pybind11::object firstArg = pyArgs[ 0 ];
                    auto             refOpt   = tryCast( firstArg.ptr() );
                    VERIFY_RTE_MSG( refOpt.has_value(), "Link Remove requires a Pointer parameter" );

                    mega::Pointer refArg = refOpt.value();
                    mega::Pointer result;

                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &refArg, &result ]( MPOContext& )
                        {
                            auto pFunction = reinterpret_cast< mega::runtime::invocation::LinkRemove::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            result = pFunction( m_reference, refArg );
                        } );

                    return m_module.getTypeSystem().cast( result );
                }
                break;
                case id_exp_Link_Clear:
                {
                    mega::Pointer result;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &result ]( MPOContext& )
                        {
                            auto pFunction = reinterpret_cast< mega::runtime::invocation::LinkClear::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            result = pFunction( m_reference );
                        } );

                    return m_module.getTypeSystem().cast( result );
                }
                break;
                case id_exp_Signal:
                {
                    THROW_TODO;
                }
                break;
                case id_exp_Call:
                {
                    runtime::CallResult result;

                    // obtain the CallResult from the invocation function - executing in the MPO context
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &m_module = m_module, &result ]( MPOContext& )
                        {
                            auto pCall = reinterpret_cast< runtime::invocation::Call::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            result = pCall( m_reference );
                        } );

                    // given the result can now obtain the corresponding python wrapper function
                    auto pPythonWrapperFunction = m_module.getPythonFunctionWrapper( result.interfaceTypeID );

                    // execute the wrapper function in the MPO context passing in the CallResult and args
                    PyObject* pPyObject = nullptr;
                    m_module.invoke( [ &pPythonWrapperFunction, &result, &pyArgs, &pPyObject ]( MPOContext& )
                                     { pPyObject = pPythonWrapperFunction( result, pyArgs ); } );

                    if( pPyObject )
                    {
                        return pPyObject;
                    }
                }
                break;
                case id_exp_Start:
                {
                    mega::Pointer result;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &m_module = m_module, &result ]( MPOContext& )
                        {
                            auto pStartFunction = reinterpret_cast< mega::runtime::invocation::Start::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            result = pStartFunction( m_reference );
                        } );
                    return m_module.getTypeSystem().cast( result );
                }
                break;
                case id_exp_GetContext:
                {
                    mega::Pointer result;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &result ]( MPOContext& )
                        {
                            auto pGetFunction = reinterpret_cast< mega::runtime::invocation::Get::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            result = pGetFunction( m_reference );
                        } );
                    return m_module.getTypeSystem().cast( result );
                }
                break;
                case id_exp_Move:
                case id_exp_Range:
                case HIGHEST_EXPLICIT_OPERATION_TYPE:
                    THROW_RTE( "Unsupported operation type: " << functionInfo.typeInfo.operationType );
                    break;
            }*/
        }
        catch( mega::runtime::RuntimeException& ex )
        {
            SPDLOG_ERROR( "JIT Exception: {}", ex.what() );
        }
        /*catch( mega::io::DatabaseVersionException& ex )
        {
            SPDLOG_ERROR( "Database version exception: {}", ex.what() );
        }*/
        catch( std::exception& ex )
        {
            SPDLOG_ERROR( "Exception: {}", ex.what() );
        }

        SPDLOG_TRACE( "PythonPointer::call returning Py_None" );

        Py_INCREF( Py_None );
        return Py_None;
    }
    else
    {
        SPDLOG_ERROR( "PythonPointer::call on invalid Pointer" );
        Py_INCREF( Py_None );
        return Py_None;
    }
}

} // namespace mega::service::python