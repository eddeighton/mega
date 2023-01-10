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

#include "llvm_compiler.hpp"

namespace mega::service
{

// network::jit::Impl

void LeafRequestConversation::GetAllocator( const mega::TypeID&         objectTypeID,
                                            const mega::U64&            jitAllocatorPtr,
                                            boost::asio::yield_context& yield_ctx )
{
    runtime::Allocator::Ptr* pAllocatorPtr = network::type_reify< runtime::Allocator::Ptr >( jitAllocatorPtr );
    *pAllocatorPtr                         = m_leaf.m_pJIT->getAllocator( getLLVMCompiler( yield_ctx ), objectTypeID );
}

void LeafRequestConversation::GetLoadRecord( const mega::U64& ppFunction, boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getLoadRecord(
        getLLVMCompiler( yield_ctx ), network::type_reify< mega::runtime::LoadRecordFunction >( ppFunction ) );
}

void LeafRequestConversation::GetLoadObjectRecord( const mega::U64&            unitName,
                                                   const mega::TypeID&         objectTypeID,
                                                   const mega::U64&            ppFunction,
                                                   boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getLoadObjectRecord( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ),
                                        objectTypeID,
                                        network::type_reify< mega::runtime::LoadObjectRecordFunction >( ppFunction ) );
}

void LeafRequestConversation::GetSaveXMLObject( const mega::U64&            unitName,
                                                const mega::TypeID&         objectTypeID,
                                                const mega::U64&            jitFunctionPtr,
                                                boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getObjectSaveXML( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ),
                                     objectTypeID,
                                     network::type_reify< mega::runtime::SaveObjectFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetLoadXMLObject( const mega::U64&            unitName,
                                                const mega::TypeID&         objectTypeID,
                                                const mega::U64&            jitFunctionPtr,
                                                boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getObjectLoadXML( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ),
                                     objectTypeID,
                                     network::type_reify< mega::runtime::LoadObjectFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetSaveBinObject( const mega::U64&            unitName,
                                                const mega::TypeID&         objectTypeID,
                                                const mega::U64&            jitFunctionPtr,
                                                boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getObjectSaveBin( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ),
                                     objectTypeID,
                                     network::type_reify< mega::runtime::SaveObjectFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetLoadBinObject( const mega::U64&            unitName,
                                                const mega::TypeID&         objectTypeID,
                                                const mega::U64&            jitFunctionPtr,
                                                boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getObjectLoadBin( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ),
                                     objectTypeID,
                                     network::type_reify< mega::runtime::LoadObjectFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetCallGetter( const mega::U64&            unitName,
                                             const mega::TypeID&         objectTypeID,
                                             const mega::U64&            jitFunctionPtr,
                                             boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getCallGetter( network::type_reify< const char >( unitName ), objectTypeID,
                                  network::type_reify< mega::runtime::TypeErasedFunction >( jitFunctionPtr ) );
}

void LeafRequestConversation::GetAllocate( const mega::U64&            unitName,
                                           const mega::InvocationID&   invocationID,
                                           const mega::U64&            jitFunctionPtr,
                                           boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getAllocate( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ),
                                invocationID,
                                network::type_reify< mega::runtime::AllocateFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetRead( const mega::U64& unitName, const mega::InvocationID& invocationID,
                                       const mega::U64& jitFunctionPtr, boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getRead( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ), invocationID,
                            network::type_reify< mega::runtime::ReadFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetWrite( const mega::U64& unitName, const mega::InvocationID& invocationID,
                                        const mega::U64& jitFunctionPtr, boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getWrite( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ), invocationID,
                             network::type_reify< mega::runtime::WriteFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetCall( const mega::U64& unitName, const mega::InvocationID& invocationID,
                                       const mega::U64& jitFunctionPtr, boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getCall( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ), invocationID,
                            network::type_reify< mega::runtime::CallFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetStart( const mega::U64& unitName, const mega::InvocationID& invocationID,
                                        const mega::U64& jitFunctionPtr, boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getStart( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ), invocationID,
                             network::type_reify< mega::runtime::StartFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetStop( const mega::U64& unitName, const mega::InvocationID& invocationID,
                                       const mega::U64& jitFunctionPtr, boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getStop( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ), invocationID,
                            network::type_reify< mega::runtime::StopFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetSave( const mega::U64& unitName, const mega::InvocationID& invocationID,
                                       const mega::U64& jitFunctionPtr, boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getSave( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ), invocationID,
                            network::type_reify< mega::runtime::SaveFunction >( jitFunctionPtr ) );
}
void LeafRequestConversation::GetLoad( const mega::U64& unitName, const mega::InvocationID& invocationID,
                                       const mega::U64& jitFunctionPtr, boost::asio::yield_context& yield_ctx )
{
    m_leaf.m_pJIT->getLoad( getLLVMCompiler( yield_ctx ), network::type_reify< const char >( unitName ), invocationID,
                            network::type_reify< mega::runtime::LoadFunction >( jitFunctionPtr ) );
}
} // namespace mega::service
