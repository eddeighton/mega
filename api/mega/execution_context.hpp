#ifndef EXECUTION_CONTEXT_AUG_30_2022
#define EXECUTION_CONTEXT_AUG_30_2022

#include "mega/common.hpp"

namespace mega
{

class ExecutionContext
{
public:
    static void              execution_resume( ExecutionContext* pExecutionContext );
    static void              execution_suspend();
    static ExecutionContext* execution_get();

public:
    virtual ExecutionIndex getThisExecutionIndex() = 0;

    virtual std::string acquireMemory( ExecutionIndex executionIndex ) = 0;

    virtual LogicalAddress allocateLogical( ExecutionIndex executionIndex, TypeID objectTypeID ) = 0;
    virtual void deAllocateLogical( ExecutionIndex executionIndex, LogicalAddress logicalAddress ) = 0;

};

#define SUSPEND_EXECUTION_CONTEXT()                                            \
    ExecutionContext* _pExecutionContext_ = ExecutionContext::execution_get(); \
    ExecutionContext::execution_suspend()

#define RESUME_EXECUTION_CONTEXT() ExecutionContext::execution_resume( _pExecutionContext_ )

} // namespace mega

#endif // EXECUTION_CONTEXT_AUG_30_2022
