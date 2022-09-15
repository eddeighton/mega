#ifndef EXECUTION_CONTEXT_AUG_30_2022
#define EXECUTION_CONTEXT_AUG_30_2022

#include "mega/common.hpp"

#include <string>

namespace mega
{

class ExecutionContext
{
public:
    static void              resume( ExecutionContext* pExecutionContext );
    static void              suspend();
    static ExecutionContext* get();

public:
    virtual MPE             getThisMPE()                                                       = 0;
    virtual mega::reference getRoot()                                                          = 0;
    virtual std::string     acquireMemory( MPE mpe )                                           = 0;
    virtual NetworkAddress  allocateNetworkAddress( MPE mpe, TypeID objectTypeID )             = 0;
    virtual void            deAllocateNetworkAddress( MPE mpe, NetworkAddress networkAddress ) = 0;
    virtual void            stash( const std::string& filePath, std::size_t determinant )      = 0;
    virtual bool            restore( const std::string& filePath, std::size_t determinant )    = 0;
    virtual void            readLock( MPE mpe )                                                = 0;
    virtual void            writeLock( MPE mpe )                                               = 0;
    virtual void            releaseLock( MPE mpe )                                             = 0;
};

#define SUSPEND_EXECUTION_CONTEXT()                                  \
    ExecutionContext* _pExecutionContext_ = ExecutionContext::get(); \
    ExecutionContext::suspend()

#define RESUME_EXECUTION_CONTEXT() ExecutionContext::resume( _pExecutionContext_ )

} // namespace mega

#endif // EXECUTION_CONTEXT_AUG_30_2022
