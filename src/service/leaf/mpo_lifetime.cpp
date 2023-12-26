
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

#include "mpo_lifetime.hpp"

#include "request.hpp"

#include "log/log.hpp"

#include "service/protocol/model/memory.hxx"

namespace mega::service
{

MPOLifetime::MPOLifetime( Leaf& leaf, LeafRequestLogicalThread& logicalthread, const MPO& mpo,
                          boost::asio::yield_context& yield_ctx )
    : m_leaf( leaf )
    , m_logicalthread( logicalthread )
    , m_yield_ctx( yield_ctx )
    , m_mpo( mpo )
{
    m_leaf.getMPOs().insert( m_mpo );
}

MPOLifetime::~MPOLifetime()
{
    m_leaf.getMPOs().erase( m_mpo );
}

} // namespace mega::service
