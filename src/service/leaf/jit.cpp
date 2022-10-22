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

#include "service/network/log.hpp"

namespace mega::service
{

// network::jit::Impl

mega::network::SizeAlignment LeafRequestConversation::GetRootSize( boost::asio::yield_context& yield_ctx ) 
{
    return m_leaf.m_pJIT->getRootSize();
}

void LeafRequestConversation::GetObjectSharedAlloc( const network::JITModuleName&  unitName,
                                                    const mega::TypeID&            objectTypeID,
                                                    const network::JITFunctionPtr& jitFunctionPtr,
                                                    boost::asio::yield_context&    yield_ctx )
{
    m_leaf.m_pJIT->getObjectSharedAlloc( network::convert( unitName ),
                                         objectTypeID,
                                         network::convert< mega::runtime::SharedCtorFunction >( jitFunctionPtr ) );
}

void LeafRequestConversation::GetObjectSharedDel( const network::JITModuleName&  unitName,
                                                  const mega::TypeID&            objectTypeID,
                                                  const network::JITFunctionPtr& jitFunctionPtr,
                                                  boost::asio::yield_context&    yield_ctx )
{
    m_leaf.m_pJIT->getObjectSharedDel( network::convert( unitName ),
                                       objectTypeID,
                                       network::convert< mega::runtime::SharedDtorFunction >( jitFunctionPtr ) );
}

void LeafRequestConversation::GetObjectHeapAlloc( const network::JITModuleName&  unitName,
                                                  const mega::TypeID&            objectTypeID,
                                                  const network::JITFunctionPtr& jitFunctionPtr,
                                                  boost::asio::yield_context&    yield_ctx )
{
    m_leaf.m_pJIT->getObjectHeapAlloc( network::convert( unitName ),
                                       objectTypeID,
                                       network::convert< mega::runtime::HeapCtorFunction >( jitFunctionPtr ) );
}

void LeafRequestConversation::GetObjectHeapDel( const network::JITModuleName&  unitName,
                                                const mega::TypeID&            objectTypeID,
                                                const network::JITFunctionPtr& jitFunctionPtr,
                                                boost::asio::yield_context&    yield_ctx )
{
    m_leaf.m_pJIT->getObjectHeapDel( network::convert( unitName ),
                                     objectTypeID,
                                     network::convert< mega::runtime::HeapDtorFunction >( jitFunctionPtr ) );
}

void LeafRequestConversation::GetCallGetter( const network::JITModuleName&  unitName,
                                             const mega::TypeID&            objectTypeID,
                                             const network::JITFunctionPtr& jitFunctionPtr,
                                             boost::asio::yield_context&    yield_ctx )
{
    m_leaf.m_pJIT->get_call_getter( network::convert( unitName ),
                                    objectTypeID,
                                    network::convert< mega::runtime::TypeErasedFunction >( jitFunctionPtr ) );
}

void LeafRequestConversation::GetAllocate( const network::JITModuleName&  unitName,
                                           const mega::InvocationID&      invocationID,
                                           const network::JITFunctionPtr& jitFunctionPtr,
                                           boost::asio::yield_context&    yield_ctx )
{
    m_leaf.m_pJIT->get_allocate( network::convert( unitName ),
                                 invocationID,
                                 network::convert< mega::runtime::AllocateFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetRead( const network::JITModuleName& unitName, const mega::InvocationID& invocationID,
                                       const network::JITFunctionPtr& jitFunctionPtr,
                                       boost::asio::yield_context&    yield_ctx )
{
    m_leaf.m_pJIT->get_read(
        network::convert( unitName ), invocationID, network::convert< mega::runtime::ReadFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetWrite( const network::JITModuleName& unitName, const mega::InvocationID& invocationID,
                                        const network::JITFunctionPtr& jitFunctionPtr,
                                        boost::asio::yield_context&    yield_ctx )
{
    m_leaf.m_pJIT->get_write( network::convert( unitName ),
                              invocationID,
                              network::convert< mega::runtime::WriteFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetCall( const network::JITModuleName& unitName, const mega::InvocationID& invocationID,
                                       const network::JITFunctionPtr& jitFunctionPtr,
                                       boost::asio::yield_context&    yield_ctx )
{
    m_leaf.m_pJIT->get_call(
        network::convert( unitName ), invocationID, network::convert< mega::runtime::CallFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetStart( const network::JITModuleName& unitName, const mega::InvocationID& invocationID,
                                        const network::JITFunctionPtr& jitFunctionPtr,
                                        boost::asio::yield_context&    yield_ctx )
{
    m_leaf.m_pJIT->get_start( network::convert( unitName ),
                              invocationID,
                              network::convert< mega::runtime::StartFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetStop( const network::JITModuleName& unitName, const mega::InvocationID& invocationID,
                                       const network::JITFunctionPtr& jitFunctionPtr,
                                       boost::asio::yield_context&    yield_ctx )
{
    m_leaf.m_pJIT->get_stop(
        network::convert( unitName ), invocationID, network::convert< mega::runtime::StopFunction >( jitFunctionPtr ) );
}
} // namespace mega::service
