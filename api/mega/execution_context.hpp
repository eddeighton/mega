#ifndef EXECUTION_CONTEXT_AUG_30_2022
#define EXECUTION_CONTEXT_AUG_30_2022

#include "mega/common.hpp"

namespace mega
{

class ExecutionContext
{
public:
    static void              resume( ExecutionContext* pExecutionContext );
    static void              suspend();
    static ExecutionContext* get();

public:
    virtual ExecutionIndex  getThisExecutionIndex()                                                           = 0;
    virtual mega::reference getRoot()                                                                         = 0;
    virtual std::string     acquireMemory( ExecutionIndex executionIndex )                                    = 0;
    virtual LogicalAddress  allocateLogical( ExecutionIndex executionIndex, TypeID objectTypeID )             = 0;
    virtual void            deAllocateLogical( ExecutionIndex executionIndex, LogicalAddress logicalAddress ) = 0;
    virtual void            stash( const std::string& filePath, std::size_t determinant )                     = 0;
    virtual bool            restore( const std::string& filePath, std::size_t determinant )                   = 0;
    virtual void            readLock( ExecutionIndex executionIndex )                                         = 0;
    virtual void            writeLock( ExecutionIndex executionIndex )                                        = 0;
    virtual void            releaseLock( ExecutionIndex executionIndex )                                      = 0;
};

#define SUSPEND_EXECUTION_CONTEXT()                                  \
    ExecutionContext* _pExecutionContext_ = ExecutionContext::get(); \
    ExecutionContext::suspend()

#define RESUME_EXECUTION_CONTEXT() ExecutionContext::resume( _pExecutionContext_ )

} // namespace mega

#endif // EXECUTION_CONTEXT_AUG_30_2022
