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


#include "pipeline/pipeline.hpp"
#include "pipeline/stash.hpp"

#include "common/assert_verify.hpp"

#include "boost/dll.hpp"
#include "boost/dll/shared_library.hpp"
#include <boost/dll/shared_library_load_mode.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include "common/string.hpp"
#include "common/file.hpp"

namespace mega
{
namespace pipeline
{

TaskDescriptor::TaskDescriptor() {}

TaskDescriptor::TaskDescriptor( const std::string& strName, const std::string& strSourceFileName, const Buffer& buffer )
    : m_strName( strName )
    , m_strSourceFile( strSourceFileName )
    , m_buffer( buffer )
{
}

Configuration::Configuration() {}

Configuration::Configuration( const Buffer& buffer )
    : m_buffer( buffer )
{
}

PipelineID Configuration::getPipelineID() const
{
    std::istringstream           is( m_buffer );
    boost::archive::xml_iarchive ia( is );
    ConfigurationHeader          header;
    ia&                          boost::serialization::make_nvp( "pipeline_header", header );
    return header.pipelineID;
}

Version Configuration::getVersion() const
{
    std::istringstream           is( m_buffer );
    boost::archive::xml_iarchive ia( is );
    ConfigurationHeader          header;
    ia&                          boost::serialization::make_nvp( "pipeline_header", header );
    return header.version;
}

void Dependencies::add( const TaskDescriptor& newTask, const TaskDescriptor::Vector& dependencies )
{
    VERIFY_RTE( newTask != TaskDescriptor() );
    m_tasks.insert( newTask );
    for ( const TaskDescriptor& task : dependencies )
    {
        VERIFY_RTE( task != TaskDescriptor() );
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

std::optional< TaskDescriptor > Schedule::getTask( const std::string& strTaskName,
                                                   const std::string& strSourceFile ) const
{
    std::optional< TaskDescriptor > result;

    for ( const TaskDescriptor& task : m_dependencies.getTasks() )
    {
        if ( task.getName() == strTaskName && task.getSourceFile() == strSourceFile )
        {
            return task;
        }
    }

    return result;
}

Stash::~Stash() {}

Progress::Progress() {}
Progress::~Progress() {}

Pipeline::Pipeline() {}
Pipeline::~Pipeline() {}

DependencyProvider::~DependencyProvider() {}

Pipeline::Ptr Registry::getPipeline( const mega::utilities::ToolChain& toolChain, const Configuration& configuration,
                                     std::ostream& osLog )
{
    try
    {
        boost::filesystem::path tempDir = boost::filesystem::temp_directory_path() / "mega_registry";
        boost::filesystem::ensureFoldersExist( tempDir / "test" );
        VERIFY_RTE_MSG(
            boost::filesystem::exists( tempDir ), "Failed to create temporary folder: " << tempDir.string() );

        std::ostringstream osTempFileName;
        {
            const boost::filesystem::path actualFile = configuration.getPipelineID();
            osTempFileName << common::uuid() << "_" << actualFile.filename().string();
        }

        const boost::filesystem::path tempDllPath = tempDir / osTempFileName.str();
        boost::filesystem::copy( configuration.getPipelineID(), tempDllPath );

        boost::dll::fs::path pipelineLibrary( tempDllPath );

        Pipeline::Ptr pPipeline
            = boost::dll::import_symbol< mega::pipeline::Pipeline >( pipelineLibrary, "mega_pipeline" );

        pPipeline->initialise( toolChain, configuration, osLog );

        return pPipeline;
    }
    catch ( std::exception& ex )
    {
        THROW_RTE( "Failed to load pipeline: " << configuration.get() << " exception: " << ex.what() );
    }
}

} // namespace pipeline
} // namespace mega
