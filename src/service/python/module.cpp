
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

#include "module.hpp"

#include "mpo_logical_thread.hpp"

#include "pointer.hpp"
#include "cast.hpp"

#include "service/network/log.hpp"

#include "service/protocol/common/type_erase.hpp"
#include "runtime/function_provider.hpp"

#include "mega/values/compilation/invocation_id.hpp"

#include "mega/values/runtime/pointer.hpp"
#include "mega/values/runtime/maths_types.hpp"

#include <boost/dynamic_bitset.hpp>

#include <spdlog/async.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <sstream>

namespace mega::service::python
{

PythonModule::Ptr getModule()
{
    static PythonModule::Ptr g_pPythonModule;
    if( !g_pPythonModule )
    {
        g_pPythonModule = std::make_shared< PythonModule >( mega::network::MegaDaemonPort(), "info", "trace" );
    }
    return g_pPythonModule;
}

struct CasterImpl : IPythonModuleCast
{
    PyObject* cast( const mega::Pointer& ref ) override { return getModule()->getTypeSystem().cast( ref ); }
} g_casterImpl;

PyObject* cast( const mega::Pointer& ref )
{
    return getModule()->getTypeSystem().cast( ref );
}

} // namespace mega::service::python

PYBIND11_MODULE( megastructure, pythonModule )
{
    using namespace mega::service::python;

    mega::service::python::PythonModule::Ptr pMegaModule = getModule();

    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    // add ALL object interface type IDs
    const TypeSystem& typeSystem   = pMegaModule->getTypeSystem();
    auto              pTypesModule = pythonModule.def_submodule( "Type" );
    for( const auto& [ strName, id ] : typeSystem.getObjectTypes() )
    {
        pTypesModule.attr( strName.c_str() ) = id;
    }

    pythonModule.doc() = "Python Module for Megastructure";

    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    // Operators
    pythonModule.def(
        "new", []( mega::SubType interfaceTypeID ) { return getModule()->operatorNew( interfaceTypeID ); },
        "Allocate a Megastructure Object" );
    pythonModule.def(
        "delete", []( mega::Pointer ref ) { getModule()->operatorDelete( ref ); },
        "Disconnect a Megastructure Object" );
    pythonModule.def(
        "cast",
        []( mega::Pointer ref, mega::SubType interfaceTypeID )
        { return getModule()->operatorCast( ref, interfaceTypeID ); },
        "Attempt to case a Megastructure Pointer" );

    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    // Root
    pybind11::class_< PythonRoot >( pythonModule, "Root" )
        .def( "machines", &PythonRoot::getMachines, "Get all machines connected to the Root" )

        ;

    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    // Machine
    pybind11::class_< PythonMachine >( pythonModule, "Machine" )
        .def( "processes", &PythonMachine::getProcesses, "Get all processes for this machine" )
        .def( "createExecutor", &PythonMachine::createExecutor, "Create a new executor" )

        ;

    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    // Process
    pybind11::class_< PythonProcess >( pythonModule, "Process" )
        .def( "mpos", &PythonProcess::getMPOs, "Get all MPOs for this process" )
        .def( "createMPO", &PythonProcess::createMPO, "Create new MPO on this process" )
        .def( "destroy", &PythonProcess::destroy, "Shutdown the process" )

        ;

    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    // MPO
    pybind11::class_< PythonMPO >( pythonModule, "MPO" )
        .def( "root", &PythonMPO::getRoot, "Get the MPO Root object" )
        .def( "new", &PythonMPO::new_, "Allocate object on the mpo" )
        .def( "destroy", &PythonMPO::destroy, "Destroy the mpo" )

        ;

    /////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    // Free Functions
    pythonModule.def(
        "root", [] { return getModule()->getRoot(); }, "Get the Megastructure Root" );

    pythonModule.def(
        "machine", []( std::string strID ) { return getModule()->getMachine( strID ); }, "Get Megastructure Machine",
        pybind11::arg( "strID" ) = "" );

    pythonModule.def(
        "process", []( std::string strID ) { return getModule()->getProcess( strID ); }, "Get Megastructure Process",
        pybind11::arg( "strID" ) = "" );

    pythonModule.def(
        "mpo", []( std::string strID ) { return getModule()->getMPO( strID ); }, "Get Megastructure MPO",
        pybind11::arg( "strID" ) = "" );

    // execution
    pythonModule.def(
        "run_one", [] { return getModule()->run_one(); }, "Run the Megastructure Message Queue for one message" );

    pythonModule.def(
        "cycle", [] { return getModule()->cycle(); },
        "Complete current cycle ( will commit all transactions to remote MPOs )" );

    // boost dynamic_bitset

#include "qvm_bindings.hpp"
}

namespace mega::service::python
{
PythonModule::LogConfig::LogConfig( const char* pszConsoleLogLevel, const char* pszFileLogLevel )
{
    boost::filesystem::path logFolder          = boost::filesystem::current_path() / "log";
    std::string             strConsoleLogLevel = "info";
    std::string             strLogFileLevel    = "info";
    if( pszConsoleLogLevel )
        strConsoleLogLevel = pszConsoleLogLevel;
    if( pszFileLogLevel )
        strLogFileLevel = pszFileLogLevel;
    m_log = mega::network::configureLog( network::Log::Config{ logFolder, "python",
                                                               mega::network::fromStr( strConsoleLogLevel ),
                                                               mega::network::fromStr( strLogFileLevel ) } );

    m_pThreadPool = spdlog::thread_pool();
}

PythonModule::PythonModule( short daemonPort, const char* pszConsoleLogLevel, const char* pszFileLogLevel )
    : m_logConfig( pszConsoleLogLevel, pszFileLogLevel )
    , m_python( m_ioContext, m_logConfig.m_log, daemonPort )
{
    SPDLOG_TRACE( "PythonModule::ctor" );

    {
        m_pExternalLogicalThread = std::make_shared< network::ExternalLogicalThread >(
            m_python, m_python.createLogicalThreadID(), m_ioContext );
        m_python.externalLogicalThreadInitiated( m_pExternalLogicalThread );
    }
    {
        m_mpoLogicalThread = std::make_shared< MPOLogicalThread >( m_python, m_python.createLogicalThreadID() );
        m_python.logicalthreadInitiated( m_mpoLogicalThread );
    }

    // wait for the MPO conversation to have started up and been specified its MPO
    while( !m_python.isMPOInitialised() )
    {
        run_one();
    }

    // initialise type system
    THROW_TODO;
    // m_python.getProject();
    //  VERIFY_RTE_MSG( project.has_value(), "Could not find mega structure project" );
    //  m_pTypeSystem = std::make_unique< TypeSystem >( *this, project.value() );
}

PythonModule::~PythonModule()
{
    shutdown();
}

mega::TypeID PythonModule::getInterfaceTypeID( const mega::TypeID concreteTypeID )
{
    return pythonRequest().PythonGetInterfaceTypeID( concreteTypeID );
}

mega::Pointer PythonModule::operatorNew( mega::SubType interfaceTypeID )
{
    SPDLOG_TRACE( "PythonModule::operatorNew: {}", interfaceTypeID );

    THROW_TODO;
    // auto pNewFunction = ( mega::runtime::operators::New::FunctionPtr )getOperator(
    //     OperatorFunction{ mega::runtime::operators::eNew, TypeID::make_object_from_objectID( interfaceTypeID ) } );

    mega::Pointer result;
    // invoke( [ &pNewFunction, &result ]( MPOContext& ) { result = pNewFunction(); } );
    return result;
}

mega::Pointer PythonModule::operatorRemoteNew( mega::SubType interfaceTypeID, MPO mpo )
{
    SPDLOG_TRACE( "PythonModule::operatorRemoteNew: {} {}", interfaceTypeID, mpo );

    THROW_TODO;
    // auto pNewFunction = ( mega::runtime::operators::RemoteNew::FunctionPtr )getOperator(
    //     OperatorFunction{ mega::runtime::operators::eRemoteNew, TypeID::make_object_from_objectID( interfaceTypeID )
    //     } );

    mega::Pointer result;
    // invoke( [ &pNewFunction, &result, mpo ]( MPOContext& ) { result = pNewFunction( mpo ); } );
    return result;
}

void PythonModule::operatorDelete( mega::Pointer ref )
{
    SPDLOG_TRACE( "PythonModule::operatorDelete: {}", ref );

    const mega::TypeID interfaceTypeID = getInterfaceTypeID( ref.getType() );

    THROW_TODO;
    // auto pDeleteFunction = ( mega::runtime::operators::Delete::FunctionPtr )getOperator(
    //     OperatorFunction{ mega::runtime::operators::eDelete, TypeID::make_object_from_typeID( interfaceTypeID ) } );

    // invoke( [ &pDeleteFunction, &ref ]( MPOContext& ) { pDeleteFunction( ref ); } );
}

mega::Pointer PythonModule::operatorCast( mega::Pointer ref, mega::SubType interfaceTypeID )
{
    SPDLOG_TRACE( "PythonModule::operatorCast: {} {}", ref, interfaceTypeID );

    THROW_TODO;
    // auto pCastFunction = ( mega::runtime::operators::Cast::FunctionPtr )getOperator(
    //     OperatorFunction{ mega::runtime::operators::eCast, TypeID::make_object_from_objectID( interfaceTypeID ) } );

    mega::Pointer result;
    // invoke( [ &pCastFunction, &ref, &result ]( MPOContext& ) { result = pCastFunction( ref ); } );
    return result;
}
/*
void* PythonModule::getOperator( const OperatorFunction& operatorFunction )
{
    THROW_TODO;
    void** ppFunction = nullptr;
    {
        attempt to acquire from cache
        auto iFind = m_operatorTable.find( operatorFunction );
        if( iFind != m_operatorTable.end() )
        {
            ppFunction = &iFind->second;
        }
        else
        {
            iFind      = m_operatorTable.insert( { operatorFunction, nullptr } ).first;
            ppFunction = &iFind->second;
        }
    }

    if( nullptr == *ppFunction )
    {
        std::optional< std::exception_ptr > exceptionPtrOpt;
        mega::runtime::RuntimeFunctor           functor(
            [ &operatorFunction, &ppFunction, &exceptionPtrOpt ]( mega::runtime::FunctionProvider& jit, void*
pLLVMCompiler )
            {
                try
                {
                    jit.getOperatorFunction(
                        pLLVMCompiler, "python", operatorFunction.second, operatorFunction.first, ppFunction );
                }
                catch( std::exception& )
                {
                    exceptionPtrOpt = std::current_exception();
                }
                catch( mega::runtime::RuntimeException& )
                {
                    exceptionPtrOpt = std::current_exception();
                }
            } );

        pythonRequest().PythonExecuteJIT( functor );
        if( exceptionPtrOpt.has_value() )
        {
            std::ostringstream osError;
            osError << "Exception compiling operator: " << operatorFunction.second;
            SPDLOG_ERROR( "PythonModule::invoke rethrowing exception for invocation: {}", osError.str() );
            std::rethrow_exception( exceptionPtrOpt.value() );
        }
    }

    VERIFY_RTE_MSG( *ppFunction, "Failed to jit compile operator for interfaceTypeID: " << operatorFunction.second );
    return *ppFunction;
}*/

const PythonModule::InvocationInfo& PythonModule::invoke( const mega::InvocationID& invocationID )
{
    SPDLOG_TRACE( "PythonModule::invoke: {}", invocationID );

    auto iFind = m_functionTable.find( invocationID );
    if( iFind != m_functionTable.end() )
    {
        if( iFind->second.pFunctionPtr != nullptr )
            return iFind->second;
    }
    else
    {
        iFind = m_functionTable.insert( { invocationID, PythonModule::InvocationInfo{} } ).first;
    }

    PythonModule::InvocationInfo&       functionInfo = iFind->second;
    std::optional< std::exception_ptr > exceptionPtrOpt;
    mega::runtime::RuntimeFunctor       functor(
        [ &invocationID, &functionInfo, &exceptionPtrOpt ](
            mega::runtime::FunctionProvider& jit, service::StashProvider& stashProvider )
        {
            try
            {
                THROW_TODO;
                // functionInfo.typeInfo =
                // jit.compileInvocationFunction(
                //     pLLVMCompiler, "python", invocationID, &functionInfo.pFunctionPtr );
            }
            catch( std::exception& )
            {
                exceptionPtrOpt = std::current_exception();
            }
            catch( mega::runtime::RuntimeException& )
            {
                exceptionPtrOpt = std::current_exception();
            }
        } );

    pythonRequest().PythonExecuteJIT( functor );
    if( exceptionPtrOpt.has_value() )
    {
        std::ostringstream osError;
        using ::           operator<<;
        osError << "Exception compiling invocation: " << invocationID;
        SPDLOG_ERROR( "PythonModule::invoke rethrowing exception for invocation: {}", osError.str() );
        std::rethrow_exception( exceptionPtrOpt.value() );
    }

    return functionInfo;
}

PythonPointer::PythonWrapperFunction PythonModule::getPythonFunctionWrapper( TypeID interfaceTypeID )
{
    SPDLOG_TRACE( "PythonModule::getPythonFunctionWrapper: {}", interfaceTypeID );

    auto iFind = m_wrapperTable.find( interfaceTypeID );
    if( iFind != m_wrapperTable.end() )
    {
        if( iFind->second.pFunctionPtr != nullptr )
            return iFind->second.pFunctionPtr;
    }
    else
    {
        iFind = m_wrapperTable.insert( { interfaceTypeID, PythonModule::WrapperInfo{} } ).first;
    }

    PythonModule::WrapperInfo&    wrapperInfo = iFind->second;
    mega::runtime::RuntimeFunctor functor(
        [ &interfaceTypeID, &wrapperInfo, pCaster = &g_casterImpl ](
            mega::runtime::FunctionProvider& jit, service::StashProvider& stashProvider )
        {
            THROW_TODO;
            // void** ppFunction = reinterpret_cast< void** >( &wrapperInfo.pFunctionPtr );
            // jit.getPythonFunction( interfaceTypeID, ppFunction, pCaster );
        } );

    pythonRequest().PythonExecuteJIT( functor );

    return wrapperInfo.pFunctionPtr;
}

void PythonModule::shutdown()
{
    SPDLOG_TRACE( "PythonModule::shutdown" );
    if( m_pExternalLogicalThread )
    {
        pythonRequest().PythonShutdown();
        m_python.logicalthreadCompleted( m_pExternalLogicalThread );
        m_pExternalLogicalThread.reset();
    }
    if( std::shared_ptr< MPOLogicalThread > pMPOCon
        = std::dynamic_pointer_cast< MPOLogicalThread >( m_mpoLogicalThread ) )
    {
        while( !pMPOCon->isRunComplete() )
        {
            run_one();
        }
    }
    m_mpoLogicalThread.reset();
}

TypeSystem& PythonModule::getTypeSystem()
{
    VERIFY_RTE_MSG( m_pTypeSystem, "Megastructure type system not initialised" );
    return *m_pTypeSystem;
}

void PythonModule::run_one()
{
    while( m_ioContext.poll_one() )
        ;
}

TimeStamp PythonModule::cycle()
{
    return pythonRequest().PythonCycle();
}

PythonRoot PythonModule::getRoot()
{
    return { *this };
}

PythonMachine PythonModule::getMachine( std::string strID /*= ""*/ )
{
    auto mpo = m_python.getMPO();
    if( strID.empty() )
    {
        return PythonMachine( *this, mpo.getMachineID() );
    }
    else
    {
        std::istringstream is( strID );
        MachineID          id;
        is >> id;
        return PythonMachine( *this, id );
    }
}

PythonProcess PythonModule::getProcess( std::string strID /*= ""*/ )
{
    auto mpo = m_python.getMPO();
    if( strID.empty() )
    {
        return PythonProcess( *this, mpo.getMP() );
    }
    else
    {
        std::istringstream is( strID );
        MP                 id;
        is >> id;
        return PythonProcess( *this, id );
    }
}

PythonMPO PythonModule::getMPO( std::string strID /*= ""*/ )
{
    auto mpo = m_python.getMPO();
    if( strID.empty() )
    {
        return PythonMPO( *this, mpo );
    }
    else
    {
        std::istringstream is( strID );
        MPO                id;
        is >> id;
        return PythonMPO( *this, id );
    }
}

} // namespace mega::service::python