


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
