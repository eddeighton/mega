
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
#include "python_type_system.hpp"

#include "module.hpp"

#include "jit/invocation_functions.hxx"
#include "jit/jit_exception.hpp"

#include "service/mpo_visitor.hpp"

#include "mega/reference_io.hpp"
#include "mega/invocation_id.hpp"
#include "mega/types/traits.hpp"

#include "mega/types/python_mangle.hpp"
#include "mega/iterator.hpp"
#include "mega/logical_tree.hpp"
#include "mega/printer.hpp"

#include "service/network/log.hpp"

#include "common/assert_verify.hpp"

#include <sstream>

namespace mega::service::python
{

PythonReference::PythonReference( PythonModule& module, Type& type, const mega::reference& ref )
    : m_module( module )
    , m_type( type )
    , m_reference( ref )
{
}

PythonReference::PythonReference( PythonModule&          module,
                                  Type&                  type,
                                  const mega::reference& ref,
                                  const TypePath&        typePath )
    : m_module( module )
    , m_type( type )
    , m_reference( ref )
    , m_type_path( typePath )
{
}

PyObject* PythonReference::get( void* pClosure )
{
    return m_type.createReference( m_reference, m_type_path, reinterpret_cast< char* >( pClosure ) );
}

PyObject* PythonReference::cast( PythonModule& module, const mega::reference& ref )
{
    return module.getTypeSystem().cast( ref );
}

mega::reference PythonReference::cast( PyObject* pObject )
{
    return Type::cast( pObject );
}

std::optional< mega::reference > PythonReference::tryCast( PyObject* pObject )
{
    return Type::tryCast( pObject );
}

int PythonReference::set( void* pClosure, PyObject* pValue )
{
    // const char* pszAttributeIdentity = reinterpret_cast< char* >( pClosure );
    return 0;
}

PyObject* PythonReference::str() const
{
    std::ostringstream os;
    using ::           operator<<;
    os << m_reference.getNetworkAddress();
    return Py_BuildValue( "s", os.str().c_str() );
}

PyObject* PythonReference::dump() const
{
    std::ostringstream os;

    m_module.invoke(
        [ &m_reference = m_reference, &os ]()
        {
            MPORealToLogicalVisitor mpoRealInstantiation( m_reference );
            LogicalTreePrinter      printer( os );
            LogicalTreeTraversal    objectTraversal( mpoRealInstantiation, printer );
            traverse( objectTraversal );
        } );

    return Py_BuildValue( "s", os.str().c_str() );
}

PyObject* PythonReference::call( PyObject* args, PyObject* kwargs )
{
    SPDLOG_TRACE( "PythonReference::call" );
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

            SPDLOG_TRACE( "PythonReference::call: {}", invocationID );

            const PythonModule::FunctionInfo& functionInfo = m_module.invoke( invocationID );
            switch( functionInfo.typeInfo.operationType )
            {
                case id_exp_Read:
                {
                    void* pResult = nullptr;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &pResult ]()
                        {
                            auto pReadFunction = reinterpret_cast< mega::runtime::invocation::Read::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            pResult = pReadFunction( m_reference );
                        } );
                    if( pResult )
                    {
                        return m_module.getPythonMangle().cppToPython( functionInfo.typeInfo.mangledType, pResult );
                    }
                }
                case id_exp_Write:
                {
                    VERIFY_RTE_MSG( pyArgs.size() > 0, "Write requires atleast one argument" );
                    pybind11::object firstArg = pyArgs[ 0 ];
                    void*            pArg
                        = m_module.getPythonMangle().pythonToCpp( functionInfo.typeInfo.mangledType, firstArg.ptr() );

                    mega::reference result;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &pArg, &result ]()
                        {
                            auto pWriteFunction = reinterpret_cast< mega::runtime::invocation::Write::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            result = pWriteFunction( m_reference, pArg );
                        } );
                    return cast( m_module, result );
                }
                case id_exp_Read_Link:
                {
                    void* pResult = nullptr;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &pResult ]()
                        {
                            auto pReadFunction = reinterpret_cast< mega::runtime::invocation::ReadLink::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            pResult = pReadFunction( m_reference );
                        } );
                    if( pResult )
                    {
                        return m_module.getPythonMangle().cppToPython( functionInfo.typeInfo.mangledType, pResult );
                    }
                }
                break;
                case id_exp_Write_Link:
                {
                    VERIFY_RTE_MSG( pyArgs.size() > 0, "Write Link requires atleast one argument" );

                    std::optional< mega::reference >       argRefOpt;
                    std::optional< mega::ReferenceVector > argRefVectorOpt;
                    std::optional< WriteOperation >        argWriteOpOpt;
                    {
                        if( pyArgs.size() == 2 )
                        {
                            argWriteOpOpt = pybind11::cast< WriteOperation >( pyArgs[ 0 ] );
                            VERIFY_RTE_MSG( argWriteOpOpt.has_value(), "Invalid arguments to link write" );

                            pybind11::object arg = pyArgs[ 1 ];
                            argRefOpt            = tryCast( arg.ptr() );
                            if( !argRefOpt.has_value() )
                            {
                                argRefVectorOpt = pybind11::cast< mega::ReferenceVector >( arg );
                            }
                        }
                        else if( pyArgs.size() == 1 )
                        {
                            pybind11::object arg = pyArgs[ 0 ];
                            try
                            {
                                argRefOpt = tryCast( arg.ptr() );
                                if( !argRefOpt.has_value() )
                                {
                                    argRefVectorOpt = pybind11::cast< mega::ReferenceVector >( arg );
                                }
                            }
                            catch( pybind11::cast_error& )
                            {
                                argWriteOpOpt = pybind11::cast< WriteOperation >( arg );
                            }
                        }
                        else
                        {
                            THROW_RTE( "Invalid number of arguments to link write" );
                        }
                    }

                    if( !argWriteOpOpt.has_value() )
                    {
                        argWriteOpOpt = ::WriteOperation::DEFAULT;
                    }

                    mega::reference result;

                    if( !argRefOpt.has_value() && !argRefVectorOpt.has_value() )
                    {
                        VERIFY_RTE_MSG(
                            argWriteOpOpt.value() == WriteOperation::RESET,
                            "Invalid write operation type with no other parameters to write link invocation" );
                        m_module.invoke(
                            [ &functionInfo, &m_reference = m_reference, &argWriteOpOpt, &result ]()
                            {
                                auto pWriteLinkFunction
                                    = reinterpret_cast< mega::runtime::invocation::WriteLink::FunctionPtr >(
                                        functionInfo.pFunctionPtr );
                                result = pWriteLinkFunction( m_reference, argWriteOpOpt.value(), {} );
                            } );
                    }
                    else if( argRefOpt.has_value() )
                    {
                        m_module.invoke(
                            [ &functionInfo, &m_reference = m_reference, &argWriteOpOpt, &argRefOpt, &result ]()
                            {
                                auto pWriteLinkFunction
                                    = reinterpret_cast< mega::runtime::invocation::WriteLink::FunctionPtr >(
                                        functionInfo.pFunctionPtr );
                                result = pWriteLinkFunction( m_reference, argWriteOpOpt.value(), argRefOpt.value() );
                            } );
                    }
                    else if( argRefVectorOpt.has_value() )
                    {
                        THROW_RTE( "WriteLinkRange not implemented in python yet" );
                        m_module.invoke(
                            [ &functionInfo, &m_reference = m_reference, &argWriteOpOpt, &argRefVectorOpt, &result ]()
                            {
                                auto pWriteLinkFunction
                                    = reinterpret_cast< mega::runtime::invocation::WriteLinkRange::FunctionPtr >(
                                        functionInfo.pFunctionPtr );
                                result = pWriteLinkFunction(
                                    m_reference, argWriteOpOpt.value(), &argRefVectorOpt.value() );
                            } );
                    }

                    return cast( m_module, result );
                }
                break;
                case id_exp_Allocate:
                {
                    mega::reference result;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &m_module = m_module, &result ]()
                        {
                            auto pAllocateFunction
                                = reinterpret_cast< mega::runtime::invocation::Allocate::FunctionPtr >(
                                    functionInfo.pFunctionPtr );
                            result = pAllocateFunction( m_reference );
                        } );
                    return cast( m_module, result );
                }
                break;
                case id_exp_Call:
                {
                    runtime::CallResult result;

                    // obtain the CallResult from the invocation function - executing in the MPO context
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &m_module = m_module, &result ]()
                        {
                            auto pCall = reinterpret_cast< runtime::invocation::Call::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            result = pCall( m_reference );
                        } );

                    // given the result can now obtain the corresponding python wrapper function
                    auto pPythonWrapperFunction = m_module.getPythonFunctionWrapper( result.interfaceTypeID );

                    // execute the wrapper function in the MPO context passing in the CallResult and args
                    PyObject* pPyObject = nullptr;
                    m_module.invoke( [ &pPythonWrapperFunction, &result, &pyArgs, &pPyObject ]()
                                     { pPyObject = pPythonWrapperFunction( result, pyArgs ); } );

                    if( pPyObject )
                    {
                        return pPyObject;
                    }
                }
                break;
                case id_exp_Start:
                {
                    mega::reference result;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &m_module = m_module, &result ]()
                        {
                            auto pStartFunction = reinterpret_cast< mega::runtime::invocation::Start::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            result = pStartFunction( m_reference );
                        } );
                    return cast( m_module, result );
                }
                break;
                case id_exp_Stop:
                {
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &m_module = m_module ]()
                        {
                            auto pStopFunction = reinterpret_cast< mega::runtime::invocation::Stop::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            pStopFunction( m_reference );
                        } );
                    Py_INCREF( Py_None );
                    return Py_None;
                }
                break;
                case id_exp_GetAction:
                case id_exp_GetDimension:
                {
                    mega::reference result;
                    m_module.invoke(
                        [ &functionInfo, &m_reference = m_reference, &result ]()
                        {
                            auto pGetFunction = reinterpret_cast< mega::runtime::invocation::Get::FunctionPtr >(
                                functionInfo.pFunctionPtr );
                            result = pGetFunction( m_reference );
                        } );
                    return cast( m_module, result );
                }
                break;
                case id_exp_Move:
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

} // namespace mega::service::python