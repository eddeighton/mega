
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

#include "mpo_conversation.hpp"

#include "service/network/network.hpp"

#include "service/protocol/model/python_leaf.hxx"

#include "service/protocol/common/conversation_base.hpp"

#include "common/assert_verify.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <boost/asio/io_context.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

namespace mega::service::python
{

class PythonModule
{
    struct LogConfig
    {
        LogConfig( const char* pszConsoleLogLevel, const char* pszFileLogLevel );
    };

public:
    using Ptr = std::shared_ptr< PythonModule >;

    static Ptr  makePlugin( short daemonPort, const char* pszConsoleLogLevel, const char* pszFileLogLevel );
    static void releasePlugin( Ptr& pPlugin );

    // PythonModule
    PythonModule( short daemonPort, const char* pszConsoleLogLevel, const char* pszFileLogLevel );
    ~PythonModule();

    PythonModule( const PythonModule& )            = delete;
    PythonModule( PythonModule&& )                 = delete;
    PythonModule& operator=( const PythonModule& ) = delete;
    PythonModule& operator=( PythonModule&& )      = delete;

    // Type System
    mega::TypeID getTypeID( const char* pszIdentifier );
    void         invoke( const mega::reference& ref, const PythonReference::TypePath& typePath );

    // Megastructure Execution
    void shutdown();
    void run_one();
    PythonRoot getRoot();

    template < typename Request >
    Request request()
    {
        return Request(
            [ mpoCon = m_mpoConversation,
              extCon = m_pExternalConversation ]( const network::Message& msg ) mutable -> network::Message
            {
                network::python_leaf::External_Request_Sender sender( *mpoCon, *extCon );
                return sender.PythonRoot( msg );
            },
            m_pExternalConversation->getID() );
    }

private:
    LogConfig                          m_logConfig;
    boost::asio::io_context            m_ioContext;
    Python                             m_python;
    network::ExternalConversation::Ptr m_pExternalConversation;
    network::ConversationBase::Ptr     m_mpoConversation;
    PythonReference::Registration      m_pythonReferenceRegistration;
};
} // namespace mega::service

#endif // GUARD_2023_March_12_module
