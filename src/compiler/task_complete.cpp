
#include "base_task.hpp"

namespace mega
{
namespace compiler
{

class Task_Complete : public BaseTask
{
    const mega::io::manifestFilePath& m_manifest;

public:
    Task_Complete( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifest )
        : BaseTask( taskArguments )
        , m_manifest( manifest )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        start( taskProgress, "Task_Complete", m_manifest.path(), m_manifest.path() );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Complete( const TaskArguments&              taskArguments,
                                    const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_Complete >( taskArguments, manifestFilePath );
}

} // namespace compiler
} // namespace mega