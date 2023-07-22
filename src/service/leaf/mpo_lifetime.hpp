
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

#ifndef GUARD_2022_October_22_mpo_lifetime
#define GUARD_2022_October_22_mpo_lifetime

#include "service/leaf.hpp"

#include "mega/types/traits.hpp"

namespace mega::service
{

class LeafRequestLogicalThread;

class MPOLifetime
{
public:
    MPOLifetime( Leaf& leaf, LeafRequestLogicalThread& logicalthread, const MPO& mpo,
                 boost::asio::yield_context& yield_ctx );
    ~MPOLifetime();

    const MPO& getMPO() const { return m_mpo; }

private:
    Leaf&                         m_leaf;
    LeafRequestLogicalThread&      m_logicalthread;
    boost::asio::yield_context&   m_yield_ctx;
    MPO                           m_mpo;
};

} // namespace mega::service

#endif // GUARD_2022_October_22_mpo_lifetime
