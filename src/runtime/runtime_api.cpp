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

#include "runtime/runtime_api.hpp"

#include "runtime.hpp"

namespace mega
{
namespace runtime
{
namespace
{

static std::unique_ptr< Runtime > g_pRuntime;

void initialiseStaticRuntime( const mega::network::MegastructureInstallation& megastructureInstallation,
                              const mega::network::Project&                   project,
                              const AddressSpace::Names&                      addressSpaceNames )
{
    try
    {
        g_pRuntime.reset();

        if ( !project.isEmpty() )
        {
            if ( !boost::filesystem::exists( project.getProjectDatabase() ) )
            {
                SPDLOG_ERROR( "Database missing at: {}", project.getProjectDatabase().string() );
            }
            else
            {
                g_pRuntime = std::make_unique< Runtime >( megastructureInstallation, project, addressSpaceNames );
            }
        }
    }
    catch ( mega::io::DatabaseVersionException& ex )
    {
        SPDLOG_ERROR( "Database version exception: {}", project.getProjectInstallPath().string(), ex.what() );
    }
    catch ( std::exception& ex )
    {
        SPDLOG_ERROR( "ComponentManager failed to initialise project: {} error: {}",
                      project.getProjectInstallPath().string(), ex.what() );
        throw;
    }
}

bool isStaticRuntimeInitialised() { return g_pRuntime.get(); }

} // namespace
} // namespace runtime
} // namespace mega

namespace mega
{
namespace runtime
{

// routines used by jit compiled functions
NetworkAddress allocateNetworkAddress( mega::MPO mpo, mega::TypeID objectTypeID )
{
    return g_pRuntime->allocateNetworkAddress( mpo, objectTypeID );
}

reference allocateMachineAddress( mega::MPO mpo, mega::TypeID objectTypeID, mega::NetworkAddress networkAddress )
{
    return g_pRuntime->allocateMachineAddress( mpo, objectTypeID, networkAddress );
}
reference networkToMachine( TypeID objectTypeID, NetworkAddress networkAddress )
{
    return g_pRuntime->networkToMachine( objectTypeID, networkAddress );
}

void get_getter_shared( const char* pszUnitName, mega::TypeID objectTypeID, GetSharedFunction* ppFunction )
{
    g_pRuntime->get_getter_shared( pszUnitName, objectTypeID, ppFunction );
}

void get_getter_heap( const char* pszUnitName, mega::TypeID objectTypeID, GetSharedFunction* ppFunction )
{
    g_pRuntime->get_getter_heap( pszUnitName, objectTypeID, ppFunction );
}

void get_getter_call( const char* pszUnitName, mega::TypeID objectTypeID, TypeErasedFunction* ppFunction )
{
    g_pRuntime->get_getter_call( pszUnitName, objectTypeID, ppFunction );
}

// public facing runtime interface
void initialiseRuntime( const mega::network::MegastructureInstallation& megastructureInstallation,
                        const mega::network::Project&                   project,
                        const std::string&                              addressSpaceMemory,
                        const std::string&                              addressSpaceMutex,
                        const std::string&                              addressSpaceMap )
{
    initialiseStaticRuntime(
        megastructureInstallation, project, { addressSpaceMemory, addressSpaceMutex, addressSpaceMap } );
}
bool isRuntimeInitialised() { return isStaticRuntimeInitialised(); }

MPORoot::MPORoot( mega::MPO mpo )
    : m_root( g_pRuntime->getRoot( mpo ) )
{
}

MPORoot::~MPORoot() { g_pRuntime->deAllocateRoot( m_root ); }

reference get_root( mega::MPO mpo ) { return g_pRuntime->getRoot( mpo ); }

void get_allocate( const char* pszUnitName, const mega::InvocationID& invocationID, AllocateFunction* ppFunction )
{
    g_pRuntime->get_allocate( pszUnitName, invocationID, ppFunction );
}

void get_read( const char* pszUnitName, const mega::InvocationID& invocationID, ReadFunction* ppFunction )
{
    g_pRuntime->get_read( pszUnitName, invocationID, ppFunction );
}

void get_write( const char* pszUnitName, const mega::InvocationID& invocationID, WriteFunction* ppFunction )
{
    g_pRuntime->get_write( pszUnitName, invocationID, ppFunction );
}

void get_call( const char* pszUnitName, const mega::InvocationID& invocationID, CallFunction* ppFunction )
{
    g_pRuntime->get_call( pszUnitName, invocationID, ppFunction );
}

} // namespace runtime
} // namespace mega
