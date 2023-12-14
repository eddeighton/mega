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

#ifndef TASK_UTILS_05_JUNE_2022
#define TASK_UTILS_05_JUNE_2022

#include "common/assert_verify.hpp"

#include <memory>

namespace mega::compiler
{

class BaseTask;

#define TASK_PROJECT( taskName ) \
    extern std::unique_ptr< BaseTask > create_Task_##taskName( const TaskArguments& );
#define TASK_SRC( taskName, sourceFileType, argumentType ) \
    extern std::unique_ptr< BaseTask > create_Task_##taskName( const TaskArguments&, argumentType );
#include "tasks.xmc"
#undef TASK_PROJECT
#undef TASK_SRC

enum TaskType
{
#define TASK_PROJECT( taskName ) eTask_##taskName,
#define TASK_SRC( taskName, sourceFileType, argumentType ) eTask_##taskName,
#include "tasks.xmc"
#undef TASK_PROJECT
#undef TASK_SRC
    TOTAL_TASK_TYPES
};

const char* g_taskNames[] = {
#define TASK_PROJECT( taskName ) "Task_" #taskName,
#define TASK_SRC( taskName, sourceFileType, argumentType ) "Task_" #taskName,
#include "tasks.xmc"
#undef TASK_PROJECT
#undef TASK_SRC
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

} // namespace mega::compiler

#endif // TASK_UTILS_05_JUNE_2022
