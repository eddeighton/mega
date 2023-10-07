
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

#include "python_reference.hpp"
#include "python_cast.hpp"

#include "service/network/log.hpp"

#include "service/protocol/common/type_erase.hpp"
#include "service/protocol/common/jit_base.hpp"

#include "mega/reference_io.hpp"
#include "mega/invocation_io.hpp"
#include "mega/maths_types.hpp"

#include <spdlog/async.h>

#include <pybind11/numpy.h>
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

PYBIND11_MODULE( megastructure, pythonModule )
{
    using namespace mega::service::python;

    pythonModule.doc() = "Python Module for Megastructure";

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
            "getThisMPO", []() { return getModule()->getMPO(); }, "Get the active MPO" );

    pythonModule.def(
        "run_one", [] { return getModule()->run_one(); }, "Run the Megastructure Message Queue for one message" );

    pythonModule.def(
        "cycle", [] { return getModule()->cycle(); },
        "Complete current cycle ( will commit all transactions to remote MPOs )" );

    using namespace boost::qvm;
    // clang-format off
#define QVM_PYBIND11_OPERATORS( T ) \
        .def( "__add__",      []( const T& v, const T& o )      { using namespace boost::qvm; return v + o; },      pybind11::is_operator() ) \
        .def( "__sub__",      []( const T& v, const T& o )      { using namespace boost::qvm; return v - o; },      pybind11::is_operator() ) \
        .def( "__mul__",      []( const T& v, const float& f )  { using namespace boost::qvm; return v * f; },      pybind11::is_operator() ) \
        .def( "__truediv__",  []( const T& v, const float& f )  { using namespace boost::qvm; return v / f; },      pybind11::is_operator() ) \
        .def( "__iadd__",     []( T& v, const T& o )            { using namespace boost::qvm; v += o; return v; },  pybind11::is_operator() ) \
        .def( "__isub__",     []( T& v, const T& o )            { using namespace boost::qvm; v -= o; return v; },  pybind11::is_operator() ) \
        .def( "__imul__",     []( T& v, const float& f )        { using namespace boost::qvm; v *= f; return v; },  pybind11::is_operator() ) \
        .def( "__itruediv__", []( T& v, const float& f )        { using namespace boost::qvm; v /= f; return v; },  pybind11::is_operator() ) \
        .def( "__repr__",     []( const T& v ) { return boost::qvm::to_string( v ); } )

#define QVM_PYBIND11_FUNCTIONS( T ) \
        .def( "norm",   []( T& v )          { boost::qvm::normalize( v ); },            "Normalize" )       \
        .def( "mag",    []( const T& v )    { return boost::qvm::mag( v ); },           "Magnitude" )       \
        .def( "dot",    []( T& v1, T& v2 )  { return boost::qvm::dot( v1, v2 ); },      "Dot Product" )

    // clang-format on

    pybind11::class_< F2 >( pythonModule, "F2" )
        .def( pybind11::init<>() )
        .def( pybind11::init< float, float >() )

        .def_property(
            "x", []( F2& v ) { return v.x(); }, []( F2& v, float f ) { return v.x( f ); }, "X coordinate" )
        .def_property(
            "y", []( F2& v ) { return v.y(); }, []( F2& v, float f ) { return v.y( f ); }, "Y coordinate" )

            QVM_PYBIND11_OPERATORS( F2 ) QVM_PYBIND11_FUNCTIONS( F2 );

    pybind11::class_< F3 >( pythonModule, "F3" )
        .def( pybind11::init<>() )
        .def( pybind11::init< float, float, float >() )
        .def_property(
            "x", []( F3& v ) { return v.x(); }, []( F3& v, float f ) { return v.x( f ); }, "X coordinate" )
        .def_property(
            "y", []( F3& v ) { return v.y(); }, []( F3& v, float f ) { return v.y( f ); }, "Y coordinate" )
        .def_property(
            "z", []( F3& v ) { return v.z(); }, []( F3& v, float f ) { return v.z( f ); }, "Z coordinate" )

        .def(
            "cross", []( F3& v1, F3& v2 ) { return boost::qvm::cross( v1, v2 ); }, "Cross Product" )

            QVM_PYBIND11_OPERATORS( F3 ) QVM_PYBIND11_FUNCTIONS( F3 );

    pybind11::class_< F4 >( pythonModule, "F4" )
        .def( pybind11::init<>() )
        .def( pybind11::init< float, float, float, float >() )
        .def_property(
            "x", []( F4& v ) { return v.x(); }, []( F4& v, float f ) { return v.x( f ); }, "X coordinate" )
        .def_property(
            "y", []( F4& v ) { return v.y(); }, []( F4& v, float f ) { return v.y( f ); }, "Y coordinate" )
        .def_property(
            "z", []( F4& v ) { return v.z(); }, []( F4& v, float f ) { return v.z( f ); }, "Z coordinate" )
        .def_property(
            "w", []( F4& v ) { return v.w(); }, []( F4& v, float f ) { return v.w( f ); }, "W coordinate" )

            QVM_PYBIND11_OPERATORS( F4 ) QVM_PYBIND11_FUNCTIONS( F4 );

    pybind11::class_< Quat >( pythonModule, "Quat" ).def( pybind11::init<>() )
        /*.def( pybind11::init< float, float, float, float >() )
        .def( pybind11::init( []( const F3& axis, float angle ) { return boost::qvm::rot_quat( axis, angle ); } ) )
        .def_property(
            "x", []( Quat& v ) { return v.x(); }, []( Quat& v, float f ) { return v.x( f ); }, "X coordinate" )
        .def_property(
            "y", []( Quat& v ) { return v.y(); }, []( Quat& v, float f ) { return v.y( f ); }, "Y coordinate" )
        .def_property(
            "z", []( Quat& v ) { return v.z(); }, []( Quat& v, float f ) { return v.z( f ); }, "Z coordinate" )
        .def_property(
            "w", []( Quat& v ) { return v.w(); }, []( Quat& v, float f ) { return v.w( f ); }, "W coordinate" )

        .def(
            "conjugate", []( Quat& v ) { return boost::qvm::conjugate( v ); }, "Conjugate" )
        .def(
            "inverse", []( Quat& v ) { return boost::qvm::inverse( v ); }, "Inverse" )
        .def(
            "slerp", []( Quat& v1, Quat& v2, float t ) { return boost::qvm::slerp( v1, v2, t ); }, "Slerp" )
        .def(
            "identity", []( Quat& v ) { return boost::qvm::set_identity( v ); }, "Set to indentity" )
        .def(
            "set_rot", []( Quat& v, const F3& axis, float angle ) { boost::qvm::set_rot( v, axis, angle ); },
            "Set rotation" )
        .def(
            "rotate", []( Quat& v, const F3& axis, float angle ) { boost::qvm::rotate( v, axis, angle ); },
            "Rotate" )
        .def(
            "set_rotx", []( Quat& v, float angle ) { boost::qvm::set_rotx( v, angle ); },
            "Set rotation in x axis" )
        .def(
            "set_roty", []( Quat& v, float angle ) { boost::qvm::set_roty( v, angle ); },
            "Set rotation in y axis" )
        .def(
            "set_rotz", []( Quat& v, float angle ) { boost::qvm::set_rotz( v, angle ); },
            "Set rotation in z axis" )
        .def(
            "rotate_x", []( Quat& v, float angle ) { boost::qvm::rotate_x( v, angle ); },
            "Rotate in x axis" )
        .def(
            "rotate_y", []( Quat& v, float angle ) { boost::qvm::rotate_y( v, angle ); },
            "Rotate in y axis" )
        .def(
            "rotate_z", []( Quat& v, float angle ) { boost::qvm::rotate_z( v, angle ); },
            "Rotate in z axis" )

            QVM_PYBIND11_OPERATORS( Quat ) QVM_PYBIND11_FUNCTIONS( Quat )*/
        ;

    pybind11::class_< F33 >( pythonModule, "F33", pybind11::buffer_protocol() )
        .def( pybind11::init<>() )
        .def_buffer(
            []( F33& m ) -> pybind11::buffer_info
            {
                return pybind11::buffer_info(
                    m.data.data(),                                  /* Pointer to buffer */
                    sizeof( float ),                                /* Size of one scalar */
                    pybind11::format_descriptor< float >::format(), /* Python struct-style format descriptor */
                    2,                                              /* Number of dimensions */
                    { 3, 3 },                                       /* Buffer dimensions */
                    { sizeof( float ) * 3,                          /* Strides (in bytes) for each index */
                      sizeof( float ) } );
            } );
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
        m_pExternalLogicalThread = std::make_shared< network::ExternalLogicalThread >(
            m_python, m_python.createLogicalThreadID(), m_ioContext );
        m_python.externalLogicalThreadInitiated( m_pExternalLogicalThread );
    }
    {
        m_mpoLogicalThread = std::make_shared< MPOLogicalThread >( m_python, m_python.createLogicalThreadID() );
        m_python.logicalthreadInitiated( m_mpoLogicalThread );
    }

    SPDLOG_TRACE( "PythonModule::ctor" );
}

PythonModule::~PythonModule()
{
    shutdown();
}

mega::TypeID PythonModule::getInterfaceTypeID( const mega::TypeID concreteTypeID )
{
    return pythonRequest().PythonGetInterfaceTypeID( concreteTypeID );
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

    PythonModule::FunctionInfo&         functionInfo = iFind->second;
    std::optional< std::exception_ptr > exceptionPtrOpt;
    mega::runtime::JITFunctor           functor(
        [ &invocationID, &functionInfo, &exceptionPtrOpt ]( mega::runtime::JITBase& jit, void* pLLVMCompiler )
        {
            try
            {
                functionInfo.typeInfo = jit.compileInvocationFunction(
                    pLLVMCompiler, "python", invocationID, &functionInfo.pFunctionPtr );
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
        std::ostringstream osError;
        using ::           operator<<;
        osError << "Exception compiling invocation: " << invocationID;
        SPDLOG_ERROR( "PythonModule::invoke rethrowing exception for invocation: {}", osError.str() );
        std::rethrow_exception( exceptionPtrOpt.value() );
    }

    return functionInfo;
}

PythonReference::PythonWrapperFunction PythonModule::getPythonFunctionWrapper( TypeID interfaceTypeID )
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
    if( const auto& project = m_python.getProject(); project.has_value() )
    {
        if( !m_pTypeSystem )
        {
            m_pTypeSystem = std::make_unique< TypeSystem >( *this, project.value() );
        }
        else
        {
            m_pTypeSystem->reload( project.value() );
        }
    }
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