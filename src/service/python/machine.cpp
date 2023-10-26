
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

#include "machine.hpp"
#include "module.hpp"

#include "service/protocol/model/enrole.hxx"

namespace mega::service::python
{

PythonMachine::PythonMachine( PythonModule& module, mega::MachineID machineID )
    : m_module( module )
    , m_machineID( machineID )
{
}

std::vector< PythonProcess > PythonMachine::getProcesses() const
{
    SPDLOG_TRACE( "PythonMachine::getMPOs" );

    std::vector< PythonProcess > result;
    {
        auto processes = m_module.rootRequest< network::enrole::Request_Encoder >().EnroleGetProcesses( m_machineID );
        for( mega::MP mp : processes )
        {
            result.emplace_back( PythonProcess( m_module, mp ) );
        }
    }
    return result;
}

} // namespace mega::service::python