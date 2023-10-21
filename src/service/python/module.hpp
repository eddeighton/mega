
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

#ifndef GUARD_2023_March_12_module
#define GUARD_2023_March_12_module

#include "python_reference.hpp"
#include "python.hpp"
#include "python_root.hpp"
#include "python_machine.hpp"
#include "python_process.hpp"
#include "python_mpo.hpp"
#include "python_type_system.hpp"

#include "mega/values/compilation/invocation_id.hpp"
#include "mega/mangle/python_mangle.hpp"

#include "jit/jit_exception.hpp"

#include "service/protocol/model/mpo.hxx"
#include "service/protocol/model/python.hxx"
#include "service/protocol/model/python_leaf.hxx"

#include "service/protocol/common/logical_thread_base.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <boost/asio/io_context.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <map>

namespace mega::service::python
{

class PythonModule
{
    struct LogConfig
    {
        LogConfig( const char* pszConsoleLogLevel, const char* pszFileLogLevel );

    private:
        std::shared_ptr< spdlog::logger >               m_pLogger;
        std::shared_ptr< spdlog::details::thread_pool > m_pThreadPool;
    };

public:
    struct InvocationInfo
    {
        void*                                      pFunctionPtr = nullptr;
        mega::runtime::JITBase::InvocationTypeInfo typeInfo;
    };
    using OperatorFunction = std::pair< mega::runtime::operators::FunctionType, mega::TypeID >;

    struct WrapperInfo
    {
        PythonReference::PythonWrapperFunction pFunctionPtr = nullptr;
    };

    using Ptr = std::shared_ptr< PythonModule >;

    // PythonModule
    PythonModule( short daemonPort, const char* pszConsoleLogLevel, const char* pszFileLogLevel );
    ~PythonModule();

    PythonModule( const PythonModule& )            = delete;
    PythonModule( PythonModule&& )                 = delete;
    PythonModule& operator=( const PythonModule& ) = delete;
    PythonModule& operator=( PythonModule&& )      = delete;

    // operators
    mega::reference operatorNew( int typeID );
    void            operatorDelete( mega::reference ref );
    mega::reference operatorCast( mega::reference ref, int typeID );

    // Python Dynamic Invocations
    mega::TypeID                           getInterfaceTypeID( const mega::TypeID concreteTypeID );
    const InvocationInfo&                  invoke( const mega::InvocationID& invocationID );
    void*                                  getOperator( const OperatorFunction& operatorFunction );
    PythonReference::PythonWrapperFunction getPythonFunctionWrapper( TypeID interfaceTypeID );

    template < typename Functor >
    void invoke( Functor&& functor )
    {
        std::optional< std::exception_ptr > exceptionPtrOpt;

        auto lambda = [ functor = std::move( functor ), &exceptionPtrOpt ]()
        {
            try
            {
                functor();
            }
            catch( std::exception& )
            {
                exceptionPtrOpt = std::current_exception();
            }
            catch( mega::runtime::JITException& )
            {
                exceptionPtrOpt = std::current_exception();
            }
        };

        pythonRequest().PythonFunctor( runtime::Functor{ lambda } );

        if( exceptionPtrOpt.has_value() )
        {
            SPDLOG_ERROR( "PythonModule::invoke rethrowing exception" );
            std::rethrow_exception( exceptionPtrOpt.value() );
        }
    }

    // Megastructure Execution
    void          shutdown();
    void          run_one();
    TimeStamp     cycle();
    PythonRoot    getRoot();
    PythonMachine getMachine( std::string strID = "" );
    PythonProcess getProcess( std::string strID = "" );
    PythonMPO     getMPO( std::string strID = "" );

    template < typename Request >
    Request rootRequest()
    {
        return Request(
            [ mpoCon = m_mpoLogicalThread,
              extCon = m_pExternalLogicalThread ]( const network::Message& msg ) mutable -> network::Message
            {
                network::python_leaf::External_Request_Sender sender( *mpoCon, *extCon );
                return sender.PythonRoot( msg );
            },
            m_pExternalLogicalThread->getID() );
    }

    template < typename Request >
    Request mpRequest( MP mp )
    {
        return Request(
            [ mpoCon = m_mpoLogicalThread, extCon = m_pExternalLogicalThread, mp ](
                const network::Message& msg ) mutable -> network::Message
            {
                network::mpo::External_Request_Sender sender( *mpoCon, *extCon );
                return sender.MPUp( msg, mp );
            },
            m_pExternalLogicalThread->getID() );
    }

    network::python::External_Request_Sender pythonRequest()
    {
        return { *m_mpoLogicalThread, *m_pExternalLogicalThread };
    }

    const mega::mangle::PythonMangle& getPythonMangle() const { return m_pythonMangle; }
    TypeSystem&                       getTypeSystem();

private:
    using FunctionTable = std::map< mega::InvocationID, InvocationInfo >;
    using OperatorTable = std::map< OperatorFunction, void* >;
    using WrapperTable  = std::map< TypeID, WrapperInfo >;

    LogConfig                           m_logConfig;
    boost::asio::io_context             m_ioContext;
    Python                              m_python;
    network::ExternalLogicalThread::Ptr m_pExternalLogicalThread;
    network::LogicalThread::Ptr         m_mpoLogicalThread;
    OperatorTable                       m_operatorTable;
    FunctionTable                       m_functionTable;
    WrapperTable                        m_wrapperTable;
    mega::mangle::PythonMangle          m_pythonMangle;
    TypeSystem::Ptr                     m_pTypeSystem;
};
} // namespace mega::service::python

#endif // GUARD_2023_March_12_module
