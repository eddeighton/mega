#ifndef MPO_CONTEXT_SEPT_18_2022
#define MPO_CONTEXT_SEPT_18_2022

#include "mega/common.hpp"

#include <string>

namespace mega
{

class MPOContext
{
public:
    static void        resume( MPOContext* pMPOContext );
    static void        suspend();
    static MPOContext* get();

public:
    virtual MPO             getThisMPO()                                                       = 0;
    virtual mega::reference getRoot()                                                          = 0;
    virtual std::string     acquireMemory( MPO mpo )                                           = 0;
    virtual NetworkAddress  allocateNetworkAddress( MPO mpo, TypeID objectTypeID )             = 0;
    virtual void            deAllocateNetworkAddress( MPO mpo, NetworkAddress networkAddress ) = 0;
    virtual void            stash( const std::string& filePath, mega::U64 determinant )        = 0;
    virtual bool            restore( const std::string& filePath, mega::U64 determinant )      = 0;
    virtual bool            readLock( MPO mpo )                                                = 0;
    virtual bool            writeLock( MPO mpo )                                               = 0;
    virtual void            releaseLock( MPO mpo )                                             = 0;
};

#define SUSPEND_MPO_CONTEXT()                      \
    MPOContext* _pMPOContext_ = MPOContext::get(); \
    MPOContext::suspend()

#define RESUME_MPO_CONTEXT() MPOContext::resume( _pMPOContext_ )

} // namespace mega

#endif // MPO_CONTEXT_SEPT_18_2022
