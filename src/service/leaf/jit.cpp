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

#include "request.hpp"

#include "service/protocol/common/type_erase.hpp"

#include "service/stash_provider.hpp"

#include "service/protocol/model/stash.hxx"

namespace mega::service
{
// network::jit::Impl

void LeafRequestLogicalThread::ExecuteJIT( const runtime::RuntimeFunctor& func, boost::asio::yield_context& yield_ctx )
{
    struct StashProviderImpl : StashProvider
    {
        LogicalThread&              m_logicalthread;
        network::Sender::Ptr        m_pSender;
        boost::asio::yield_context& m_yield_ctx;

        StashProviderImpl( LogicalThread&              m_logicalthread,
                           network::Sender::Ptr        m_pSender,
                           boost::asio::yield_context& m_yield_ctx )
            : m_logicalthread( m_logicalthread )
            , m_pSender( m_pSender )
            , m_yield_ctx( m_yield_ctx )
        {
        }

        virtual void stash( const std::string& filePath, mega::U64 determinant ) const override
        {
            // LogicalThread
            network::leaf_daemon::Request_Sender router( m_logicalthread, m_pSender, m_yield_ctx );
            network::stash::Request_Encoder      rq( [ &router ]( const network::Message& msg )
                                                { return router.LeafRoot( msg ); },
                                                m_logicalthread.getID() );
            rq.StashStash( filePath, determinant );
        }

        virtual bool restore( const std::string& filePath, mega::U64 determinant ) const override
        {
            network::leaf_daemon::Request_Sender router( m_logicalthread, m_pSender, m_yield_ctx );
            network::stash::Request_Encoder      rq( [ &router ]( const network::Message& msg )
                                                { return router.LeafRoot( msg ); },
                                                m_logicalthread.getID() );
            return rq.StashRestore( filePath, determinant );
        }
    };

    StashProviderImpl stashProvider( *this, m_leaf.getDaemonSender(), yield_ctx );

    func( m_leaf.getRuntime(), stashProvider );
}

TypeID LeafRequestLogicalThread::GetInterfaceTypeID( const mega::TypeID& concreteTypeID, boost::asio::yield_context& )
{
    THROW_TODO;
    // VERIFY_RTE_MSG( m_leaf.m_pJIT, "JIT not initialised" );
    // return m_leaf.m_pJIT->getInterfaceTypeID( concreteTypeID );
}

} // namespace mega::service
