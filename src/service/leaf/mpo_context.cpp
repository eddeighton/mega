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

// #include "jit/program_functions.hxx"
#include "runtime/exception.hpp"

#include "report/url.hpp"

#include "service/mpo_context.hpp"
#include "service/cycle.hpp"
#include "service/reporters.hpp"
#include "service/network/network.hpp"

#include "service/protocol/common/type_erase.hpp"

#include "common/assert_verify.hpp"

#include "log/log.hpp"

namespace mega::runtime
{
/*
namespace runtime
{
MPO getThisMPO()
{
return getMPOContext()->getThisMPO();
}
void networkToHeap( Pointer& ref )
{
getMPOContext()->networkToHeap( ref );
}
void readLock( Pointer& ref )
{
getMPOContext()->readLock( ref );
}
void writeLock( Pointer& ref )
{
getMPOContext()->writeLock( ref );
}
Pointer allocate( mega::TypeID objectType )
{
return getMPOContext()->allocate( objectType );
}
Pointer allocateRemote( const MPO& remote, mega::TypeID objectType )
{
return getMPOContext()->allocateRemote( remote, objectType );
}
void* log()
{
return &getMPOContext()->getLog();
}
} // namespace runtime
*/

Cycle::~Cycle()
{
    try
    {
        THROW_TODO;
        /*if( auto pContext = getMPOContext() )
        {
            pContext->cycleComplete();
        }*/
    }
    catch( mega::runtime::RuntimeException& ex )
    {
        SPDLOG_ERROR( "Cycle::~Cycle JIT exception: {}", ex.what() );
    }
    catch( std::exception& ex )
    {
        SPDLOG_ERROR( "Cycle::~Cycle exception: {}", ex.what() );
    }
}

runtime::PointerNet MPOContext::getRoot( runtime::MPO mpo )
{
    if( m_root.getMPO() == mpo )
    {
        return m_root.getPointerNet();
    }
    else
    {
        THROW_TODO;
        // return Pointer::make_root( mpo );
    }
}

runtime::MPO MPOContext::constructMPO( runtime::MP machineProcess )
{
    network::sim::Request_Encoder request(
        [ mpoRequest = getMPRequest(), machineProcess ]( const network::Message& msg ) mutable
        { return mpoRequest.MPUp( msg, machineProcess ); },
        m_logicalthreadIDRef );
    return request.SimCreate();
}

runtime::MP MPOContext::constructExecutor( runtime::MachineID daemonMachineID )
{
    network::enrole::Request_Encoder request( [ mpoRequest = getMPRequest() ]( const network::Message& msg ) mutable
                                              { return mpoRequest.MPRoot( msg, runtime::MP{} ); },
                                              m_logicalthreadIDRef );
    return request.EnroleCreateExecutor( daemonMachineID );
}

void MPOContext::destroyExecutor( runtime::MP mp )
{
    network::enrole::Request_Encoder request( [ mpoRequest = getMPRequest(), mp ]( const network::Message& msg ) mutable
                                              { return mpoRequest.MPUp( msg, mp ); },
                                              m_logicalthreadIDRef );
    return request.EnroleDestroy();
}

runtime::PointerHeap MPOContext::allocate( concrete::ObjectID  )
{
    THROW_TODO;
    /*VERIFY_RTE_MSG( m_pMemoryManager, "Memory manager not instantiated" );

    Pointer allocated = m_pMemoryManager->New( objectTypeID );
    m_pLog->record(
        mega::event::Structure::Write( allocated, allocated.getPointerNet(), 0, mega::event::Structure::eConstruct )
    );

    return allocated;*/
}

runtime::PointerNet MPOContext::allocateRemote( const runtime::MPO& remote, concrete::ObjectID objectTypeID )
{
    runtime::PointerNet allocated;
    if( remote == getThisMPO() )
    {
        auto local = allocate( objectTypeID );
        allocated  = local.getPointerNet();
    }
    else
    {
        runtime::TimeStamp lockCycle = m_lockTracker.isWrite( remote );
        if( lockCycle == 0U )
        {
            lockCycle = getMPOSimRequest( remote ).SimLockWrite( getThisMPO(), remote );
            VERIFY_RTE_MSG( lockCycle != 0U, "Failed to acquire write lock on: " << remote );
            m_lockTracker.onWrite( remote, lockCycle );
        }
        allocated = getLeafMemoryRequest().NetworkAllocate( remote, objectTypeID, lockCycle );
    }

    return allocated;
}

// networkToHeap ONLY called when MPO matches
runtime::PointerHeap MPOContext::networkToHeap( const runtime::PointerNet&  )
{
    THROW_TODO;
    /*SPDLOG_TRACE( "MPOContext::networkToHeap: {}", ref );

    VERIFY_RTE_MSG( ref.getMPO() == getThisMPO(), "networkToHeap used when not matching MPO" );

    if( ref.isNetworkAddress() )
    {
        VERIFY_RTE_MSG( m_pMemoryManager, "MPOContext::networkToHeap: Memory manager not allocated" );
        ref = m_pMemoryManager->networkToHeap( ref );
    }*/
}
/*
void MPOContext::readLock( runtime::Pointer& ref )
{
    SPDLOG_TRACE( "MPOContext::readLock: {}", ref );

    VERIFY_RTE_MSG( ref.getMPO() != getThisMPO(), "readLock used when matching MPO" );

    // acquire lock if required and get lock cycle
    runtime::TimeStamp lockCycle = m_lockTracker.isRead( ref.getMPO() );
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

void MPOContext::writeLock( runtime::Pointer& ref )
{
    SPDLOG_TRACE( "MPOContext::writeLock: {}", ref );

    VERIFY_RTE_MSG( ref.getMPO() != getThisMPO(), "writeLock used when matching MPO" );

    runtime::TimeStamp lockCycle = m_lockTracker.isWrite( ref.getMPO() );
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
*/
void MPOContext::createRoot( const runtime::MPO& mpo )
{
    // initialise event log
    try
    {
        boost::filesystem::path logFolder;
        {
            if( auto cfgType = network::MegaType(); cfgType.has_value() )
            {
                std::ostringstream os;
                os << "/home/foobar/test_" << cfgType.value();
                logFolder = os.str();
            }
            else
            {
                logFolder = boost::filesystem::current_path();
            }
        }
        std::ostringstream os;
        {
            os << "events/ev_" << mpo.getMachineID() << '_' << mpo.getProcessID() << '_' << mpo.getOwnerID() << '_'
               << m_logicalthreadIDRef << "/";
        }

        const boost::filesystem::path eventLogFolder = logFolder / os.str();
        SPDLOG_TRACE( "MPOContext::createRoot: mpo: {} event log: {}", mpo, eventLogFolder.string() );

        m_pLog                 = std::make_unique< event::FileStorage >( eventLogFolder, false );
        m_pTransactionProducer = std::make_unique< network::TransactionProducer >( *m_pLog );
    }
    catch( std::exception& ex )
    {
        std::ostringstream os;
        os << mpo;
        THROW_RTE( "Exception attempting to create event log for mpo: " << os.str() << " error: " << ex.what() );
    }

    m_mpo = mpo;

    // m_pDatabase.reset();
    // THROW_TODO;
    // m_pMemoryManager.reset();

    // THROW_TODO;
    /*if( boost::filesystem::exists( project.getProjectDatabase() ) )
    {
        m_pDatabase = std::make_unique< runtime::MPODatabase >( project.getProjectDatabase() );

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
        //m_root = m_pMemoryManager->New( ROOT_TYPE_ID );
        VERIFY_RTE_MSG( m_root.valid(), "Root allocation failed" );
        SPDLOG_INFO( "Created Root: {} net: {}", m_root.getObjectAddress(), m_root.getPointerNet() );
        m_pLog->record(
            mega::event::Structure::Write( m_root, m_root.getPointerNet(), 0, mega::event::Structure::eConstruct )
    );
    }
    else
    {
        SPDLOG_WARN( "Could not create root - no database found at: {}", project.getProjectDatabase().string() );
    }*/
}

void MPOContext::jit( runtime::RuntimeFunctor func )
{
    getLeafJITRequest().ExecuteJIT( func );
}
void MPOContext::yield()
{
    boost::asio::post( *m_pYieldContext );
}

void MPOContext::applyTransaction( const network::Transaction&  )
{
    // NOTE: can context switch when call get_load_record
    THROW_TODO;
    // static thread_local mega::runtime::program::RecordLoadBin recordLoadBin;
    // static thread_local mega::runtime::program::RecordMake    recordMake;
    // static thread_local mega::runtime::program::RecordBreak   recordBreak;

    /*const network::Transaction::In& data = transaction.m_in.value();
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
                    recordMake( structure.m_data.m_Source, structure.m_data.m_Target, structure.m_data.m_Relation );
                    break;
                case log::Structure::eBreak:
                    recordBreak( structure.m_data.m_Source, structure.m_data.m_Target, structure.m_data.m_Relation );
                    break;
                case log::Structure::eMove:
                {
                    THROW_TODO;
                    // m_movedObjects.insert( { structure.m_data.m_Target.getMPO(),
                    //                          { structure.m_data.m_Source, structure.m_data.m_Target } } );
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
        for( const log::Event::DataIO& event : data.m_event )
        {
            SPDLOG_TRACE( "SIM::SimLockRelease Got event record: {} {}", event.m_data.m_Ref,
                          log::Event::toString( event.m_data.m_Type ) );
            m_pLog->record( log::Event::Write( event.m_data.m_Ref, event.m_data.m_Type ) );
        }
    }
    {
        for( const log::Transition::DataIO& transition : data.m_transition )
        {
            // SPDLOG_TRACE( "SIM::SimLockRelease Got transition record: {} {}", transition.m_data.m_Ref,
            //     transition.m_data.m_Transition );
            m_pLog->record( log::Transition::Write( transition.m_data.m_Ref ) );
        }
    }
    {
        for( const log::Memory::DataIO& memory : data.m_memory )
        {
            SPDLOG_TRACE( "SIM::SimLockRelease Got memory record: {} {}", memory.m_data.m_Ref, memory.m_Data );
            recordLoadBin( memory.m_data.m_Ref, ( void* )memory.m_Data.data(), memory.m_Data.size() );
        }
    }*/
}

void MPOContext::cycleComplete()
{
    m_pLog->cycle();

    network::TransactionProducer::MPOTransactions transactions;
    network::TransactionProducer::UnparentedSet   unparentedObjects;
    m_pTransactionProducer->generate( transactions, unparentedObjects, m_movedObjects );

    // THROW_TODO;
    /*if( m_pMemoryManager )
    {
        m_pMemoryManager->Garbage();

        for( const auto& unparentedObject : unparentedObjects )
        {
            // DONT DESTOY THE ROOT!
            VERIFY_RTE( unparentedObject.getType() != ROOT_TYPE_ID );

            // destroy the object
            SPDLOG_TRACE( "MPOContext: cycleComplete: {} unparented: {}", m_mpo.value(), unparentedObject );
            if( unparentedObject.getMPO() == m_mpo.value() )
            {
                // delete the heap address
                Pointer deleteRef = unparentedObject;
                if( deleteRef.isNetworkAddress() )
                {
                    deleteRef = m_pMemoryManager->networkToHeap( deleteRef );
                }
                m_pMemoryManager->Delete( deleteRef );
                m_pLog->record( mega::event::Structure::Write(
                    deleteRef, deleteRef.getPointerNet(), 0, mega::event::Structure::eDestruct ) );
            }
        }
    }*/

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

/*
void MPOContest::getDump()
{
    os << "<html><body><H1>";
    os << "Hello from HTTPLogicalThread: " << getThisMPO() << "</h1><P>";
    {
        MPORealToLogicalVisitor mpoRealInstantiation( getThisRoot() );
        LogicalTreePrinter      printer( os );
        LogicalTreeTraversal    objectTraversal( mpoRealInstantiation, printer );
        traverse( objectTraversal );
    }
    os << "</P></body></html>";
}
*/

void MPOContext::getBasicReport( const URL& url, Table& table )
{
    SPDLOG_TRACE( "MPOContext::GetBasicReport: mpo: {} {}", m_mpo.value(), url.c_str() );

    using namespace std::string_literals;

    // general stuff

    // clang-format off
    table.m_rows.push_back( { Line{ "         MPO: "s }, Line{ m_mpo.value() } } );
    // table.m_rows.push_back( { Line{ "   Root Heap: "s }, Line{ m_root } } );
    // if( m_root.valid() )
    // {
    //     table.m_rows.push_back( { Line{ "    Root Net: "s }, Line{ m_root.getPointerNet() } } );
    // }

    table.m_rows.push_back( { Line{ "        Tick: "s }, Line{ std::to_string( getLog().getTimeStamp() ) } } );
    table.m_rows.push_back( { Line{ "  Start Time: "s }, Line{ common::printTimeStamp( m_systemStartTime ) } } );
    table.m_rows.push_back( { Line{ "Elapsed Time: "s }, Line{ common::printDuration( getElapsedTime() ) } } );
    table.m_rows.push_back( { Line{ "     Log Dir: "s }, Line{ getLog().getLogFolderPath().string(), 
        report::makeFileURL( url, getLog().getLogFolderPath().string() ) } } );
    // clang-format on

    auto reportType     = report::getReportType( url );
    bool bDoBasicReport = true;

    if( reportType.has_value() )
    {
        if( reportType.value() == "memory" )
        {
            // do service request...
            // THROW_TODO;
            // MemoryReporter memoryReporter( *m_pMemoryManager, *m_pDatabase );
            // table.m_rows.push_back( { Line{ "     Memory: "s }, memoryReporter.generate( url ) } );
        }
    }

    if( bDoBasicReport )
    {
        {
            Table                     logTable;
            const event::IndexRecord& iter = getLog().getIterator();
            for( auto i = 0; i != event::toInt( event::TrackID::TOTAL ); ++i )
            {
                if( auto amt = iter.get( event::TrackID( i ) ).get(); amt > 0 )
                {
                    logTable.m_rows.push_back( { Line{ event::toName( event::TrackID( i ) ) },
                                                 Line{ std::to_string( iter.get( event::TrackID( i ) ).get() ) } } );
                }
            }
            table.m_rows.push_back( { Line{ "         Log: "s }, logTable } );
        }

        {
            Table locks{ { "Lock Type"s, "MPO"s, "Timestamp"s } };
            for( auto& [ mpo, timestamp ] : m_lockTracker.getReads() )
            {
                locks.m_rows.push_back( { Line{ "READ"s }, Line{ mpo }, Line{ std::to_string( timestamp ) } } );
            }
            for( auto& [ mpo, timestamp ] : m_lockTracker.getWrites() )
            {
                locks.m_rows.push_back( { Line{ "WRITE"s }, Line{ mpo }, Line{ std::to_string( timestamp ) } } );
            }
            table.m_rows.push_back( { Line{ "    In Locks: "s }, locks } );
        }

        // THROW_TODO;
        // if( m_pMemoryManager )
        // {
        //     const network::MemoryStatus memStatus = m_pMemoryManager->getStatus();
        //     table.m_rows.push_back( { Line{ "         Mem: "s }, Line{ std::to_string( memStatus.m_heap ) } } );
        //     table.m_rows.push_back( { Line{ "         Obj: "s }, Line{ std::to_string( memStatus.m_object ) } } );
        // }
    }
}

} // namespace mega::runtime
