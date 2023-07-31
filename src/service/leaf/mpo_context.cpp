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

#include <memory>

#include "jit/program_functions.hxx"
#include "jit/jit_exception.hpp"

#include "service/mpo_context.hpp"
#include "service/cycle.hpp"

#include "service/protocol/common/type_erase.hpp"

#include "common/assert_verify.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace runtime
{
MPO getThisMPO()
{
    return getMPOContext()->getThisMPO();
}
void networkToHeap( reference& ref )
{
    getMPOContext()->networkToHeap( ref );
}
void readLock( reference& ref )
{
    getMPOContext()->readLock( ref );
}
void writeLock( reference& ref )
{
    getMPOContext()->writeLock( ref );
}
reference allocate( const reference& parent, TypeID typeID )
{
    return getMPOContext()->allocate( parent, typeID );
}
void* log()
{
    return &getMPOContext()->getLog();
}
} // namespace runtime

Cycle::~Cycle()
{
    try
    {
        if( auto pContext = getMPOContext() )
        {
            pContext->cycleComplete();
        }
    }
    catch( mega::runtime::JITException& ex )
    {
        SPDLOG_ERROR( "Cycle::~Cycle JIT exception: {}", ex.what() );
    }
    catch( std::exception& ex )
    {
        SPDLOG_ERROR( "Cycle::~Cycle exception: {}", ex.what() );
    }
}

reference MPOContext::getRoot( MPO mpo )
{
    if( m_root.getMPO() == mpo )
    {
        return m_root;
    }
    else
    {
        return reference::make_root( mpo );
    }
}

MPO MPOContext::constructMPO( MP machineProcess )
{
    network::sim::Request_Encoder request(
        [ mpoRequest = getMPRequest(), machineProcess ]( const network::Message& msg ) mutable
        { return mpoRequest.MPUp( msg, machineProcess ); },
        m_logicalthreadIDRef );
    return request.SimCreate();
}

MP MPOContext::constructExecutor( MachineID daemonMachineID )
{
    network::enrole::Request_Encoder request( [ mpoRequest = getMPRequest() ]( const network::Message& msg ) mutable
                                              { return mpoRequest.MPRoot( msg, MP{} ); },
                                              m_logicalthreadIDRef );
    return request.EnroleCreateExecutor( daemonMachineID );
}

void MPOContext::destroyExecutor( MP mp )
{
    network::enrole::Request_Encoder request( [ mpoRequest = getMPRequest(), mp ]( const network::Message& msg ) mutable
                                              { return mpoRequest.MPUp( msg, mp ); },
                                              m_logicalthreadIDRef );
    return request.EnroleDestroy();
}

reference MPOContext::allocate( const reference& parent, TypeID objectTypeID )
{
    reference allocated;
    if( parent.getMPO() == getThisMPO() )
    {
        allocated = m_pMemoryManager->New( objectTypeID );
        m_pLog->record( mega::log::Structure::Write( parent, allocated, 0, mega::log::Structure::eConstruct ) );
    }
    else
    {
        MPO       targetMPO = parent.getMPO();
        TimeStamp lockCycle = m_lockTracker.isWrite( targetMPO );
        if( lockCycle == 0U )
        {
            lockCycle = getMPOSimRequest( targetMPO ).SimLockWrite( getThisMPO(), targetMPO );
            VERIFY_RTE_MSG( lockCycle != 0U, "Failed to acquire write lock on: " << targetMPO );
            m_lockTracker.onWrite( targetMPO, lockCycle );
        }
        allocated = getLeafMemoryRequest().NetworkAllocate( targetMPO, objectTypeID, lockCycle );
    }

    return allocated;
}

// networkToHeap ONLY called when MPO matches
void MPOContext::networkToHeap( reference& ref )
{
    SPDLOG_TRACE( "MPOContext::networkToHeap: {}", ref );

    VERIFY_RTE_MSG( ref.getMPO() == getThisMPO(), "networkToHeap used when not matching MPO" );

    if( ref.isNetworkAddress() )
    {
        ASSERT( m_pMemoryManager );
        ref = m_pMemoryManager->networkToHeap( ref );
    }
}

void MPOContext::readLock( reference& ref )
{
    SPDLOG_TRACE( "MPOContext::readLock: {}", ref );

    VERIFY_RTE_MSG( ref.getMPO() != getThisMPO(), "readLock used when matching MPO" );

    // acquire lock if required and get lock cycle
    TimeStamp lockCycle = m_lockTracker.isRead( ref.getMPO() );
    if( lockCycle == 0U )
    {
        lockCycle = getMPOSimRequest( ref.getMPO() ).SimLockRead( getThisMPO(), ref.getMPO() );
        VERIFY_RTE_MSG( lockCycle != 0U, "Failed to acquire write lock on: " << ref );
        m_lockTracker.onRead( ref.getMPO(), lockCycle );
    }

    if( ref.isNetworkAddress() || ( ref.getLockCycle() != lockCycle ) )
    {
        ref = getLeafMemoryRequest().NetworkToHeap( ref, lockCycle );
    }
}

void MPOContext::writeLock( reference& ref )
{
    SPDLOG_TRACE( "MPOContext::writeLock: {}", ref );

    VERIFY_RTE_MSG( ref.getMPO() != getThisMPO(), "writeLock used when matching MPO" );

    TimeStamp lockCycle = m_lockTracker.isWrite( ref.getMPO() );
    if( lockCycle == 0U )
    {
        lockCycle = getMPOSimRequest( ref.getMPO() ).SimLockWrite( getThisMPO(), ref.getMPO() );
        VERIFY_RTE_MSG( lockCycle != 0U, "Failed to acquire write lock on: " << ref );
        m_lockTracker.onWrite( ref.getMPO(), lockCycle );
    }

    if( ref.isNetworkAddress() || ( ref.getLockCycle() != lockCycle ) )
    {
        ref = getLeafMemoryRequest().NetworkToHeap( ref, lockCycle );
    }
}

void MPOContext::createRoot( const Project& project, const mega::MPO& mpo )
{
    // initialise event log
    {
        boost::filesystem::path logFolder;
        {
            const char* pszCFG_TYPE = std::getenv( network::ENV_CFG_TYPE );
            if( pszCFG_TYPE != nullptr )
            {
                std::ostringstream os;
                os << "/home/foobar/test_" << pszCFG_TYPE;
                logFolder = os.str();
            }
            else
            {
                logFolder = boost::filesystem::current_path();
            }
        }
        std::ostringstream os;
        {
            using ::operator<<;
            os << "events/ev_" << mpo.getMachineID() << "-" << mpo.getProcessID() << "-"
               << static_cast< mega::U32 >( mpo.getOwnerID() ) << "_" << m_logicalthreadIDRef << "/";
        }

        const boost::filesystem::path eventLogFolder = logFolder / os.str();
        SPDLOG_TRACE( "MPOContext::createRoot: mpo: {} project: {} event log: {}", mpo,
                      project.getProjectInstallPath().string(), eventLogFolder.string() );

        m_pLog                 = std::make_unique< log::FileStorage >( eventLogFolder, false );
        m_pTransactionProducer = std::make_unique< network::TransactionProducer >( *m_pLog );
    }

    m_mpo = mpo;

    m_pDatabase.reset();
    m_pDatabase = std::make_unique< runtime::MPODatabase >( project.getProjectDatabase() );

    m_pMemoryManager.reset();

    m_pMemoryManager = std::make_unique< runtime::MemoryManager >(
        *m_pDatabase,
        mpo,
        [ jitRequest = getLeafJITRequest() ]( TypeID typeID ) mutable -> runtime::Allocator::Ptr
        {
            runtime::Allocator::Ptr pAllocator;
            jitRequest.GetAllocator( typeID, type_erase( &pAllocator ) );
            return pAllocator;
        } );

    // instantiate the root
    m_root = m_pMemoryManager->New( ROOT_TYPE_ID );
    VERIFY_RTE_MSG( m_root.is_valid(), "Root allocation failed" );
    m_pLog->record( mega::log::Structure::Write( reference{}, m_root, 0, mega::log::Structure::eConstruct ) );
}

void MPOContext::jit( runtime::JITFunctor func )
{
    getLeafJITRequest().ExecuteJIT( func );
}
void MPOContext::yield()
{
    boost::asio::post( *m_pYieldContext );
}

void MPOContext::applyTransaction( const network::Transaction& transaction )
{
    // NOTE: can context switch when call get_load_record
    static thread_local mega::runtime::program::RecordLoadBin recordLoadBin;
    static thread_local mega::runtime::program::RecordMake    recordMake;
    static thread_local mega::runtime::program::RecordBreak   recordBreak;

    const network::Transaction::In& data = transaction.m_in.value();
    {
        for( const log::Structure::DataIO& structure : data.m_structure )
        {
            SPDLOG_TRACE( "SIM::SimLockRelease Got structure record: {} {} {}", structure.m_data.m_Source,
                          structure.m_data.m_Target, log::Structure::toString( structure.m_data.m_Type ) );

            switch( structure.m_data.m_Type )
            {
                case log::Structure::eConstruct:
                case log::Structure::eDestruct:
                    break;
                case log::Structure::eMake:
                case log::Structure::eMakeSource:
                case log::Structure::eMakeTarget:
                    recordMake( structure.m_data.m_Source, structure.m_data.m_Target );
                    break;
                case log::Structure::eBreak:
                case log::Structure::eBreakSource:
                case log::Structure::eBreakTarget:
                    recordBreak( structure.m_data.m_Source, structure.m_data.m_Target );
                    break;
                case log::Structure::eMove:
                {
                    m_movedObjects.insert( { structure.m_data.m_Target.getMPO(),
                                             { structure.m_data.m_Source, structure.m_data.m_Target } } );
                }
                break;
                default:
                    THROW_RTE(
                        "Unsupported structure record type: " << log::Structure::toString( structure.m_data.m_Type ) );
                    break;
            }
        }
    }
    {
        for( const log::Scheduling::DataIO& scheduling : data.m_scheduling )
        {
            SPDLOG_TRACE( "SIM::SimLockRelease Got scheduling record: {} {}", scheduling.m_data.m_Ref,
                          log::Scheduling::toString( scheduling.m_data.m_Type ) );
            m_pLog->record( log::Scheduling::Write( scheduling.m_data.m_Ref, scheduling.m_data.m_Type ) );
        }
    }
    {
        for( const log::Memory::DataIO& memory : data.m_memory )
        {
            SPDLOG_TRACE( "SIM::SimLockRelease Got memory record: {} {}", memory.m_data.m_Ref, memory.m_Data );
            recordLoadBin( memory.m_data.m_Ref, ( void* )memory.m_Data.data(), memory.m_Data.size() );
        }
    }
}

void MPOContext::cycleComplete()
{
    m_pLog->cycle();

    network::TransactionProducer::MPOTransactions transactions;
    network::TransactionProducer::UnparentedSet   unparentedObjects;
    m_pTransactionProducer->generate( transactions, unparentedObjects, m_movedObjects );

    m_pMemoryManager->Garbage();

    for( const auto& unparentedObject : unparentedObjects )
    {
        // destroy the object
        SPDLOG_TRACE( "MPOContext: cycleComplete: {} unparented: {}", m_mpo.value(), unparentedObject );
        if( unparentedObject.getMPO() == m_mpo.value() )
        {
            // delete the heap address
            reference deleteRef = unparentedObject;
            if( deleteRef.isNetworkAddress() )
            {
                deleteRef = m_pMemoryManager->networkToHeap( deleteRef );
            }
            m_pMemoryManager->Delete( deleteRef );
            m_pLog->record( mega::log::Structure::Write( reference{}, deleteRef, 0, mega::log::Structure::eDestruct ) );
        }
    }

    for( const auto& [ writeLockMPO, lockCycle ] : m_lockTracker.getWrites() )
    {
        SPDLOG_TRACE( "MPOContext: cycleComplete: {} sending: write release to: {}", m_mpo.value(), writeLockMPO );
        getMPOSimRequest( writeLockMPO )
            .SimLockRelease( m_mpo.value(), writeLockMPO, network::Transaction{ transactions[ writeLockMPO ] } );
    }

    for( const auto& [ readLockMPO, lockCycle ] : m_lockTracker.getReads() )
    {
        SPDLOG_TRACE( "MPOContext: cycleComplete: {} sending: read release to: {}", m_mpo.value(), readLockMPO );
        getMPOSimRequest( readLockMPO ).SimLockRelease( m_mpo.value(), readLockMPO, network::Transaction{} );
    }

    m_lockTracker.reset();
}

} // namespace mega
