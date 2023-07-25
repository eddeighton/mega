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

#include "mega/mpo.hpp"

#include <map>

namespace mega::service
{
class LockTracker
{
    using MPOLockMap = std::map< MPO, TimeStamp >;

public:
    TimeStamp isRead( MPO mpo ) const 
    { 
        auto iFind = m_reads.find( mpo );
        if( iFind != m_reads.end() )
        {
            return iFind->second;
        }
        return isWrite( mpo );
    }

    TimeStamp isWrite( MPO mpo ) const 
    { 
        auto iFind = m_writes.find( mpo );
        if( iFind != m_writes.end() )
        {
            return iFind->second;
        }
        return 0U;
    }

    TimeStamp getLockCycle( MPO mpo )
    {
        TimeStamp writeTime = isWrite( mpo );
        if( writeTime == 0U )
        {
            writeTime = isRead( mpo );
        }
        return writeTime;
    }

    void onRead( MPO mpo, TimeStamp lockCycle )
    {
        if( !isWrite( mpo ) )
        {
            m_reads.insert( { mpo, lockCycle } );
        }
    }

    void onWrite( MPO mpo, TimeStamp lockCycle )
    {
        m_reads.erase( mpo );
        m_writes.insert( { mpo, lockCycle } );
    }

    void onRelease( MPO mpo )
    {
        m_reads.erase( mpo );
        m_writes.erase( mpo );
    }

    const MPOLockMap& getReads() const { return m_reads; }
    const MPOLockMap& getWrites() const { return m_writes; }

    void reset()
    {
        m_reads.clear();
        m_writes.clear();
    }

private:
    MPOLockMap m_reads, m_writes;
};
} // namespace mega::service

#endif // LOCK_TRACKER_SEPT_19_2022
