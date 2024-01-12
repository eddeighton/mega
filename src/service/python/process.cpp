
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

#include "process.hpp"

#include "module.hpp"

#include "service/mpo_context.hpp"

#include "service/protocol/model/enrole.hxx"
#include "service/protocol/model/sim.hxx"

namespace mega::service::python
{

PythonProcess::PythonProcess( PythonModule& module, mega::runtime::MP mp )
    : m_module( module )
    , m_mp( mp )
{
}

std::vector< PythonMPO > PythonProcess::getMPOs() const
{
    SPDLOG_TRACE( "PythonProcess::getMPOs" );

    std::vector< PythonMPO > result;
    {
        auto mpos = m_module.rootRequest< network::enrole::Request_Encoder >().EnroleGetMPO( m_mp );
        for( mega::runtime::MPO mpo : mpos )
        {
            result.emplace_back( PythonMPO( m_module, mpo ) );
        }
    }
    return result;
}

PythonMPO PythonProcess::createMPO() const
{
    MPO newMPO = m_module.mpRequest< network::sim::Request_Encoder >( m_mp ).SimCreate();
    return PythonMPO{ m_module, newMPO };
}

void PythonProcess::destroy() const
{
    m_module.invoke( [ mp = m_mp ]( mega::MPOContext& mpoContext ) { mpoContext.destroyExecutor( mp ); } );
}
} // namespace mega::service::python