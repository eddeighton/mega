#ifndef TASK_UTILS_05_JUNE_2022
#define TASK_UTILS_05_JUNE_2022

#include "common/assert_verify.hpp"

#include <memory>

namespace mega
{
namespace compiler
{

class BaseTask;

#define TASK( taskName, sourceFileType, argumentType ) \
    extern std::unique_ptr< BaseTask > create_Task_##taskName( const TaskArguments&, argumentType );
#include "tasks.hxx"
#undef TASK

enum TaskType
{
#define TASK( taskName, sourceFileType, argumentType ) eTask_##taskName,
#include "tasks.hxx"
#undef TASK
    TOTAL_TASK_TYPES
};

const char* g_taskNames[] = {
#define TASK( taskName, sourceFileType, argumentType ) "Task_" #taskName,
#include "tasks.hxx"
#undef TASK
};

inline const char* taskTypeToName( TaskType type ) { return g_taskNames[ type ]; }

inline TaskType nameToTaskType( const char* pszName )
{
    for ( int i = 0; i != TOTAL_TASK_TYPES; ++i )
    {
        if ( strcmp( pszName, g_taskNames[ i ] ) == 0 )
            return static_cast< TaskType >( i );
    }
    THROW_RTE( "Failed to find task type for: " << pszName );
}

} // namespace compiler
} // namespace mega

#endif // TASK_UTILS_05_JUNE_2022
