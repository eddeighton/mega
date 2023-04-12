
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

#include "mpo_conversation.hpp"

#include "python_reference.hpp"

#include "service/network/log.hpp"

#include "service/protocol/common/type_erase.hpp"
#include "service/protocol/common/jit_base.hpp"

#include "mega/reference_io.hpp"

#include <spdlog/async.h>

#include <pybind11/stl.h>

#include <sstream>

mega::service::python::PythonModule::Ptr getModule()
{
    static mega::service::python::PythonModule::Ptr g_pPythonModule;
    if( !g_pPythonModule )
    {
        g_pPythonModule = std::make_shared< mega::service::python::PythonModule >(
            mega::network::MegaDaemonPort(), "info", "trace" );
    }
    return g_pPythonModule;
}
namespace
{
} // namespace

namespace PYBIND11_NAMESPACE
{
namespace detail
{
template <>
struct type_caster< mega::reference >
{
public:
    /**
     * This macro establishes the name 'inty' in
     * function signatures and declares a local variable
     * 'value' of type inty
     */
    PYBIND11_TYPE_CASTER( mega::reference, const_name( "reference" ) );

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
        value = mega::service::python::PythonReference::cast( source );

        /* Ensure return code was OK (to avoid out-of-range errors etc) */
        return !( value != mega::reference{} && !PyErr_Occurred() );
    }

    /**
     * Conversion part 2 (C++ -> Python): convert an inty instance into
     * a Python object. The second and third arguments are used to
     * indicate the return value policy and parent object (for
     * ``return_value_policy::reference_internal``) and are generally
     * ignored by implicit casters.
     */
    static handle cast( mega::reference src, return_value_policy /* policy */, handle /* parent */ )
    {
        return mega::service::python::PythonReference::cast( *getModule(), src );
    }
};
} // namespace detail
} // namespace PYBIND11_NAMESPACE

PYBIND11_MODULE( megastructure, pythonModule )
{
    using namespace mega::service::python;

    pythonModule.doc() = "Python Module for Megastructure";

    pybind11::enum_< WriteOperation >( pythonModule, "WriteOperation" )
        .value( "DEFAULT", WriteOperation::DEFAULT )
        .value( "INSERT", WriteOperation::INSERT )
        .value( "REMOVE", WriteOperation::REMOVE )
        .value( "OVERWRITE", WriteOperation::OVERWRITE )
        .value( "RESET", WriteOperation::RESET );

    pybind11::class_< PythonRoot >( pythonModule, "Root" )
        .def( "getMachines", &PythonRoot::getMachines, "Get all machines connected to the Root" );

    pybind11::class_< PythonMachine >( pythonModule, "Machine" )
        .def( "getProcesses", &PythonMachine::getProcesses, "Get all processes for this machine" );

    pybind11::class_< PythonProcess >( pythonModule, "Process" )
        .def( "getMPOs", &PythonProcess::getMPOs, "Get all MPOs for this process" )
        .def( "createMPO", &PythonProcess::createMPO, "Create new MPO on this process" );

    pybind11::class_< PythonMPO >( pythonModule, "MPO" )
        .def( "getRoot", &PythonMPO::getRoot, "Get the MPO Root object" );

    pythonModule
        .def(
            "getRoot", [] { return getModule()->getRoot(); }, "Get the Megastructure Root" )
        .def(
            "getMachine", []( std::string strID ) { return getModule()->getMachine( strID ); },
            "Get Megastructure Machine", pybind11::arg( "strID" ) = "" )
        .def(
            "getProcess", []( std::string strID ) { return getModule()->getProcess( strID ); },
            "Get Megastructure Process", pybind11::arg( "strID" ) = "" )
        .def(
            "getMPO", []( std::string strID ) { return getModule()->getMPO( strID ); }, "Get Megastructure MPO",
            pybind11::arg( "strID" ) = "" )
        .def(
            "getThisMPO", []() { return getModule()->getMPO(); }, "Get the active MPO" )
            ;

    pythonModule.def(
        "run_one", [] { return getModule()->run_one(); }, "Run the Megastructure Message Queue for one message" );

    pythonModule.def(
        "cycle", [] { return getModule()->cycle(); },
        "Complete current cycle ( will commit all transactions to remote MPOs )" );
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
    m_pLogger = mega::network::configureLog(
        logFolder, "python", mega::network::fromStr( strConsoleLogLevel ), mega::network::fromStr( strLogFileLevel ) );

    m_pThreadPool = spdlog::thread_pool();
}

PythonModule::PythonModule( short daemonPort, const char* pszConsoleLogLevel, const char* pszFileLogLevel )
    : m_logConfig( pszConsoleLogLevel, pszFileLogLevel )
    , m_python( m_ioContext, daemonPort )
{
    {
        m_pExternalConversation = std::make_shared< network::ExternalConversation >(
            m_python, m_python.createConversationID( m_python.getLeafSender().getConnectionID() ), m_ioContext );
        m_python.externalConversationInitiated( m_pExternalConversation );
    }
    {
        m_mpoConversation = std::make_shared< MPOConversation >(
            m_python, m_python.createConversationID( m_python.getLeafSender().getConnectionID() ) );
        m_python.conversationInitiated( m_mpoConversation, m_python.getLeafSender() );
    }

    {
        SPDLOG_TRACE( "PythonModule::ctor getting identities" );
        PythonReference::Registration::SymbolTable symbols = pythonRequest().PythonGetIdentities();
        m_pRegistration = std::make_unique< PythonReference::Registration >( symbols );
    }

    SPDLOG_TRACE( "PythonModule::ctor" );
}

PythonModule::~PythonModule()
{
    shutdown();
}

const PythonModule::FunctionInfo& PythonModule::invoke( const mega::InvocationID& invocationID )
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
        iFind = m_functionTable.insert( { invocationID, PythonModule::FunctionInfo{} } ).first;
    }

    PythonModule::FunctionInfo& functionInfo = iFind->second;
    std::optional< std::exception_ptr > exceptionPtrOpt;
    mega::runtime::JITFunctor   functor(
        [ &invocationID, &functionInfo, &exceptionPtrOpt ]( mega::runtime::JITBase& jit, void* pLLVMCompiler )
        {
            try
            {
                functionInfo.typeInfo
                    = jit.compileInvocationFunction( pLLVMCompiler, "python", invocationID, &functionInfo.pFunctionPtr );
            }
            catch( std::exception& )
            {
                exceptionPtrOpt = std::current_exception();
            }
            catch( mega::runtime::JITException& )
            {
                exceptionPtrOpt = std::current_exception();
            }
        } );

    pythonRequest().PythonExecuteJIT( functor );
    if( exceptionPtrOpt.has_value() )
    {
        SPDLOG_ERROR( "PythonModule::invoke rethrowing exception" );
        std::rethrow_exception(exceptionPtrOpt.value() );
    }

    return functionInfo;
}

PythonReference::PythonWrapperFunction PythonModule::getPythonWrapper( TypeID interfaceTypeID )
{
    SPDLOG_TRACE( "PythonModule::getPythonWrapper: {}", interfaceTypeID );

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

    PythonModule::WrapperInfo& wrapperInfo = iFind->second;
    mega::runtime::JITFunctor  functor(
        [ &interfaceTypeID, &wrapperInfo ]( mega::runtime::JITBase& jit, void* )
        {
            void** ppFunction = reinterpret_cast< void** >( &wrapperInfo.pFunctionPtr );
            jit.getPythonFunction( interfaceTypeID, ppFunction );
        } );

    pythonRequest().PythonExecuteJIT( functor );

    return wrapperInfo.pFunctionPtr;
}

void PythonModule::shutdown()
{
    SPDLOG_TRACE( "PythonModule::shutdown" );
    if( m_pExternalConversation )
    {
        pythonRequest().PythonShutdown();
        m_python.conversationCompleted( m_pExternalConversation );
        m_pExternalConversation.reset();
    }
    if( std::shared_ptr< MPOConversation > pMPOCon = std::dynamic_pointer_cast< MPOConversation >( m_mpoConversation ) )
    {
        while( !pMPOCon->isRunComplete() )
        {
            run_one();
        }
    }
    m_mpoConversation.reset();
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
        return PythonProcess( *this, mpo );
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