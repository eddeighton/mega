
#include "pipeline/pipeline.hpp"
#include "pipeline/stash.hpp"

#include "common/assert_verify.hpp"

#include "boost/dll.hpp"
#include <boost/filesystem/operations.hpp>

namespace mega
{
namespace pipeline
{

TaskDescriptor::TaskDescriptor() {}

TaskDescriptor::TaskDescriptor( const std::string& strName, const Buffer& buffer )
    : m_strName( strName )
    , m_buffer( buffer )
{
}

Configuration::Configuration() {}

Configuration::Configuration( const Buffer& buffer )
    : m_buffer( buffer )
{
}

void Dependencies::add( const TaskDescriptor& newTask, const TaskDescriptor::Vector& dependencies )
{
    m_tasks.insert( newTask );
    for ( const TaskDescriptor& task : dependencies )
    {
        m_graph.insert( std::make_pair( newTask, task ) );
        m_tasks.insert( task );
    }
}

Schedule::Schedule( const Dependencies& dependencies )
    : m_dependencies( dependencies )
{
}

TaskDescriptor::Vector Schedule::getReady() const
{
    TaskDescriptor::Vector ready;
    {
        const Dependencies::Graph& graph = m_dependencies.getDependencies();
        for ( const TaskDescriptor& task : m_dependencies.getTasks() )
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


Stash::~Stash(){}

Progress::Progress() {}
Progress::~Progress() {}

Pipeline::Pipeline() {}
Pipeline::~Pipeline() {}

Pipeline::Ptr Registry::getPipeline( const Pipeline::ID& id, const Configuration& configuration )
{
    try
    {
        boost::filesystem::path cwd = boost::filesystem::current_path();

        boost::dll::fs::path pipelineLibrary( id );

        Pipeline::Ptr pPipeline = boost::dll::import_symbol< mega::pipeline::Pipeline >(
            pipelineLibrary, "mega_pipeline", boost::dll::load_mode::append_decorations );

        pPipeline->initialise( configuration );

        return pPipeline;
    }
    catch ( std::exception& ex )
    {
        THROW_RTE( "Failed to load pipeline: " << id << " exception: " << ex.what() );
    }
}

} // namespace pipeline
} // namespace mega
