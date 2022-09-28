
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

#ifndef RUNTIME_18_JUNE_2022
#define RUNTIME_18_JUNE_2022

#include "runtime/runtime_functions.hpp"
#include "runtime/mpo_context.hpp"

#include "mega/common.hpp"
#include "mega/root.hpp"
#include "mega/invocation_id.hpp"

#include "service/protocol/common/header.hpp"
#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/project.hpp"

namespace mega
{
namespace runtime
{
void initialiseRuntime( const mega::network::MegastructureInstallation& megastructureInstallation,
                        const mega::network::Project&                   project );
bool isRuntimeInitialised();

class MPORoot
{
public:
    MPORoot( mega::MPO mpo );
    ~MPORoot();

    mega::reference root() const { return m_root; }

private:
    mega::reference m_root;
};
reference get_root( mega::MPO mpo );

void get_allocate( const char* pszUnitName, const mega::InvocationID& invocationID, AllocateFunction* ppFunction );
void get_read( const char* pszUnitName, const mega::InvocationID& invocationID, ReadFunction* ppFunction );
void get_write( const char* pszUnitName, const mega::InvocationID& invocationID, WriteFunction* ppFunction );
void get_call( const char* pszUnitName, const mega::InvocationID& invocationID, CallFunction* ppFunction );

} // namespace runtime
} // namespace mega

#endif // RUNTIME_18_JUNE_2022
