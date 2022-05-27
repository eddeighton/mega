
#include "pipeline/pipeline.hpp"

#include "boost/dll.hpp"

namespace mega
{
namespace pipeline
{

Task::Task() {}

Task::Task( const std::string& taskName )
    : m_taskName( taskName )
{
}

void Dependencies::add( const Task& newTask, const Task::Vector& dependencies )
{
    m_tasks.insert( newTask );
    for ( const Task& task : dependencies )
    {
        m_graph.insert( std::make_pair( newTask, task ) );
        m_tasks.insert( task );
    }
}

Schedule::Schedule( const Dependencies& dependencies )
    : m_dependencies( dependencies )
{
}

Task::Vector Schedule::getReady() const
{
    Task::Vector ready;
    {
        const Dependencies::Graph& graph = m_dependencies.getDependencies();
        for ( const Task& task : m_dependencies.getTasks() )
        {
            if ( m_complete.count( task ) )
                continue;
            bool bWaiting = false;
            for ( Dependencies::Graph::const_iterator i = graph.lower_bound( task ), iEnd = graph.upper_bound( task );
                  i != iEnd; ++i )
            {
                if ( !m_complete.count( i->second ) )
                {
                    bWaiting = true;
                    break;
                }
            }
            if ( !bWaiting )
            {
                ready.push_back( task );
            }
        }
    }
    return ready;
}

Pipeline::Pipeline() {}
Pipeline::~Pipeline() {}

Registry::Registry() {}
Registry::~Registry() {}

Pipeline::Ptr Registry::getPipeline( const Pipeline::ID& id )
{
    PipelinePtrMap::const_iterator iFind = m_pipelines.find( id );
    if ( iFind != m_pipelines.end() )
        return iFind->second;

    Pipeline::Ptr pPipeline;
    try
    {
        boost::dll::fs::path pipelineLibrary( id );

        pPipeline = boost::dll::import_symbol< Pipeline >(
            pipelineLibrary / "pipeline", "pipeline", boost::dll::load_mode::append_decorations );

        m_pipelines.insert( std::make_pair( id, pPipeline ) );
    }
    catch ( std::exception& ex )
    {
        //...
    }

    return pPipeline;
}

} // namespace pipeline
} // namespace mega
