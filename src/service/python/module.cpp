
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

#include "python_reference.hpp"

#include "service/network/log.hpp"

#include "service/protocol/common/jit_base.hpp"

#include "service/protocol/model/status.hxx"
#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/python.hxx"

#include <pybind11/stl.h>

namespace
{
mega::service::python::PythonModule::Ptr getModule()
{
    static mega::service::python::PythonModule::Ptr g_pPythonModule;
    if( !g_pPythonModule )
    {
        g_pPythonModule
            = mega::service::python::PythonModule::makePlugin( mega::network::MegaDaemonPort(), "info", "trace" );
    }
    return g_pPythonModule;
}
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

    pybind11::class_< PythonRoot >( pythonModule, "Root" )
        .def( "getMachines", &PythonRoot::getMachines, "Get all machines connected to the Root" );

    pybind11::class_< PythonMachine >( pythonModule, "Machine" )
        .def( "getProcesses", &PythonMachine::getProcesses, "Get all processes for this machine" );

    pybind11::class_< PythonProcess >( pythonModule, "Process" )
        .def( "getMPOs", &PythonProcess::getMPOs, "Get all MPOs for this process" );

    pybind11::class_< PythonMPO >( pythonModule, "MPO" )
        .def( "getRoot", &PythonMPO::getRoot, "Get the MPO Root object" );

    pythonModule.def(
        "getRoot", [] { return getModule()->getRoot(); }, "Get the Megastructure Root" );
    pythonModule.def(
        "run_one", [] { return getModule()->run_one(); }, "Run the Megastructure Message Queue for one message" );
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
    mega::network::configureLog(
        logFolder, "python", mega::network::fromStr( strConsoleLogLevel ), mega::network::fromStr( strLogFileLevel ) );
}

PythonModule::Ptr PythonModule::makePlugin( short daemonPort, const char* pszConsoleLogLevel,
                                            const char* pszFileLogLevel )
{
    auto pPlugin = std::make_shared< PythonModule >( daemonPort, pszConsoleLogLevel, pszFileLogLevel );
    // work around shared_from_this in constructor issues
    return pPlugin;
}

void PythonModule::releasePlugin( Ptr& pPlugin )
{
    // work around shared_from_this in constructor issues
    pPlugin->shutdown();
    pPlugin.reset();
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
    SPDLOG_TRACE( "PythonModule::dtor" );
    m_python.conversationCompleted( m_pExternalConversation );
}

mega::runtime::TypeErasedFunction PythonModule::invoke( const mega::InvocationID& invocationID )
{
    SPDLOG_TRACE( "PythonModule::invoke: {}", invocationID );

    auto iFind = m_functionTable.find( invocationID );
    if( iFind != m_functionTable.end() )
    {
        if( iFind->second != nullptr )
            return iFind->second;
    }
    else
    {
        iFind = m_functionTable.insert( std::make_pair( invocationID, nullptr ) ).first;
    }

    void**                    ppFunctionPtr = &iFind->second;
    mega::runtime::JITFunctor functor(
        [ &invocationID, &ppFunctionPtr ]( mega::runtime::JITBase& jit, void* pLLVMCompiler )
        { jit.compileInvocationFunction( pLLVMCompiler, "python", invocationID, ppFunctionPtr ); } );

    pythonRequest().PythonExecuteJIT( functor );

    return *ppFunctionPtr;
}

void PythonModule::shutdown()
{
    SPDLOG_TRACE( "PythonModule::shutdown" );

    // send shutdown to MPOConversation
}

void PythonModule::run_one()
{
    while( m_ioContext.poll_one() )
        ;
}

PythonRoot PythonModule::getRoot()
{
    return { *this };
}

} // namespace mega::service::python