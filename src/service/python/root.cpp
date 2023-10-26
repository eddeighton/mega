
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

#include "root.hpp"

#include "module.hpp"

#include "service/protocol/model/enrole.hxx"

namespace mega::service::python
{

PythonRoot::PythonRoot( PythonModule& module )
    : m_module( module )
{
}

std::vector< PythonMachine > PythonRoot::getMachines() const
{
    SPDLOG_TRACE( "PythonRoot::getMachines" );

    std::vector< PythonMachine > result;
    {
        auto machineIDs = m_module.rootRequest< network::enrole::Request_Encoder >().EnroleGetDaemons();
        for( mega::MachineID machineID : machineIDs )
        {
            result.emplace_back( PythonMachine( m_module, machineID ) );
        }
    }
    return result;
}

} // namespace mega::service::python