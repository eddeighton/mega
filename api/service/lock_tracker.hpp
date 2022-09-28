
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

#ifndef LOCK_TRACKER_SEPT_19_2022
#define LOCK_TRACKER_SEPT_19_2022

#include "mega/reference.hpp"

#include <set>

namespace mega
{
namespace service
{
class LockTracker
{
    using MPOSet = std::set< MPO >;

public:
    void onRead( MPO mpo ) { m_reads.insert( mpo ); }

    void onWrite( MPO mpo )
    {
        m_reads.erase( mpo );
        m_writes.insert( mpo );
    }

    void onRelease( MPO mpo )
    {
        m_reads.erase( mpo );
        m_writes.erase( mpo );
    }

    const MPOSet& getReads() const { return m_reads; }
    const MPOSet& getWrites() const { return m_writes; }

    void reset()
    {
        m_reads.clear();
        m_writes.clear();
    }

private:
    MPOSet m_reads, m_writes;
};
} // namespace service
} // namespace mega

#endif // LOCK_TRACKER_SEPT_19_2022
