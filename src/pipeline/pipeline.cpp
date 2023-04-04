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

#include <boost/dll.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/dll/shared_library_load_mode.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include "common/string.hpp"
#include "common/file.hpp"

namespace mega::pipeline
{

TaskDescriptor::TaskDescriptor()
{
}

TaskDescriptor::TaskDescriptor( const std::string& strName, const std::string& strSourceFileName, const Buffer& buffer )
    : m_strName( strName )
    , m_strSourceFile( strSourceFileName )
    , m_buffer( buffer )
{
}

Configuration::Configuration()
{
}

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

Dependencies::Dependencies( const Dependencies& other, const std::vector< TaskDescriptor >& targets )
{
    const Graph& graph = other.getDependencies();

    TaskSet open, closed;
    {
        for( auto t : targets )
        {
            open.insert( t );
            m_tasks.insert( t );
        }
    }

    while( !open.empty() )
    {
        // get the first one and move it to closed
        TaskDescriptor t = *open.begin();
        open.erase( open.begin() );
        closed.insert( t );

        // find all dependent tasks
        auto i    = graph.lower_bound( t );
        auto iEnd = graph.upper_bound( t );
        for( ; i != iEnd; ++i )
        {
            auto dependency = i->second;
            m_graph.insert( { t, dependency } );

            // add each task if NOT already closed
            auto iFind = closed.find( dependency );
            if( iFind == closed.end() )
            {
                open.insert( dependency );
                m_tasks.insert( dependency );
            }
        }
    }
}

void Dependencies::add( const TaskDescriptor& newTask, const TaskDescriptor::Vector& dependencies )
{
    VERIFY_RTE( newTask != TaskDescriptor() );
    m_tasks.insert( newTask );
    for( const TaskDescriptor& task : dependencies )
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
        for( const TaskDescriptor& task : m_dependencies.getTasks() )
        {
            if( m_complete.count( task ) )
                continue;
            bool bWaiting = false;
            for( Dependencies::Graph::const_iterator i = graph.lower_bound( task ), iEnd = graph.upper_bound( task );
                 i != iEnd; ++i )
            {
                if( !m_complete.count( i->second ) )
                {
                    bWaiting = true;
                    break;
                }
            }
            if( !bWaiting )
            {
                ready.push_back( task );
            }
        }
    }
    return ready;
}

std::vector< TaskDescriptor > Schedule::getTasks( const std::string& strTaskName ) const
{
    std::vector< TaskDescriptor > tasks;
    for( const TaskDescriptor& task : m_dependencies.getTasks() )
    {
        if( task.getName() == strTaskName )
        {
            tasks.push_back( task );
        }
    }
    return tasks;
}

std::optional< TaskDescriptor > Schedule::getTask( const std::string& strTaskName,
                                                   const std::string& strSourceFile ) const
{
    std::optional< TaskDescriptor > result;

    for( const TaskDescriptor& task : m_dependencies.getTasks() )
    {
        if( task.getName() == strTaskName && task.getSourceFile() == strSourceFile )
        {
            return task;
        }
    }

    return result;
}

Schedule Schedule::getUpTo( const std::string& strTaskName ) const
{
    auto result = getTasks( strTaskName );
    VERIFY_RTE_MSG( !result.empty(), "Failed to locate tasks: " << strTaskName );
    return { Dependencies( m_dependencies, result ) };
}

Schedule Schedule::getUpTo( const std::string& strTaskName, const std::string& strSourceFile ) const
{
    std::optional< TaskDescriptor > result = getTask( strTaskName, strSourceFile );
    VERIFY_RTE_MSG( result.has_value(), "Failed to locate task: " << strTaskName << " with source: " << strSourceFile );
    return { Dependencies( m_dependencies, { result.value() } ) };
}

Stash::~Stash() = default;

Progress::Progress()  = default;
Progress::~Progress() = default;

Pipeline::Pipeline()  = default;
Pipeline::~Pipeline() = default;

DependencyProvider::~DependencyProvider() = default;

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
        boost::filesystem::copy_file(
            configuration.getPipelineID(), tempDllPath, boost::filesystem::copy_options::synchronize );

        boost::dll::fs::path pipelineLibrary( tempDllPath );

        Pipeline::Ptr pPipeline
            = boost::dll::import_symbol< mega::pipeline::Pipeline >( pipelineLibrary, "mega_pipeline" );

        pPipeline->initialise( toolChain, configuration, osLog );

        return pPipeline;
    }
    catch( std::exception& ex )
    {
        THROW_RTE( "Failed to load pipeline: " << configuration.get() << " exception: " << ex.what() );
    }
}

PipelineResult runPipelineLocally( const boost::filesystem::path& stashDir, const mega::utilities::ToolChain& toolChain,
                                   const mega::pipeline::Configuration& pipelineConfig, const std::string& strTaskName,
                                   const std::string&             strSourceFile,
                                   const boost::filesystem::path& inputPipelineResultPath, bool bForceNoStash,
                                   bool bExecuteUpTo, std::ostream& osLog )
{
    VERIFY_RTE_MSG( !stashDir.empty(), "Local pipeline execution requires stash directry" );
    task::Stash          stash( stashDir );
    task::BuildHashCodes buildHashCodes;

    // load previous builds hash codes
    if( !inputPipelineResultPath.empty() )
    {
        mega::pipeline::PipelineResult buildPipelineResult;
        auto pInFileStream = boost::filesystem::createBinaryInputFileStream( inputPipelineResultPath );
        boost::archive::xml_iarchive archive( *pInFileStream );
        archive&                     boost::serialization::make_nvp( "PipelineResult", buildPipelineResult );
        buildHashCodes.set( buildPipelineResult.getBuildHashCodes() );
    }
    osLog << "Initialising pipeline" << std::endl;

    mega::pipeline::Pipeline::Ptr pPipeline = mega::pipeline::Registry::getPipeline( toolChain, pipelineConfig, osLog );

    mega::pipeline::PipelineResult pipelineResult( true, "", buildHashCodes.get() );

    struct ProgressReport : public mega::pipeline::Progress
    {
        task::Stash&                    m_stash;
        task::BuildHashCodes&           m_buildHashCodes;
        mega::pipeline::PipelineResult& m_pipelineResult;
        std::ostream&                   m_osLog;
        ProgressReport( mega::pipeline::PipelineResult& pipelineResult, task::Stash& stash,
                        task::BuildHashCodes& buildHashCodes, std::ostream& osLog )
            : m_pipelineResult( pipelineResult )
            , m_stash( stash )
            , m_buildHashCodes( buildHashCodes )
            , m_osLog( osLog )
        {
        }
        virtual void onStarted( const std::string& strMsg ) { m_osLog << strMsg << std::endl; }
        virtual void onProgress( const std::string& strMsg ) { m_osLog << strMsg << std::endl; }
        virtual void onFailed( const std::string& strMsg )
        {
            m_osLog << strMsg << std::endl;
            m_pipelineResult = mega::pipeline::PipelineResult( false, strMsg, m_buildHashCodes.get() );
        }
        virtual void onCompleted( const std::string& strMsg ) { m_osLog << strMsg << std::endl; }
    } progressReporter( pipelineResult, stash, buildHashCodes, osLog );

    struct StashImpl : public mega::pipeline::Stash
    {
        task::Stash&          m_stash;
        task::BuildHashCodes& m_buildHashCodes;
        bool                  bForceNoStash;
        StashImpl( task::Stash& stash, task::BuildHashCodes& buildHashCodes, bool bForceNoStash )
            : m_stash( stash )
            , m_buildHashCodes( buildHashCodes )
            , bForceNoStash( bForceNoStash )
        {
        }

        virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath )
        {
            return m_buildHashCodes.get( filePath );
        }
        virtual void setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode )
        {
            m_buildHashCodes.set( filePath, hashCode );
        }
        virtual void stash( const boost::filesystem::path& file, task::DeterminantHash code )
        {
            m_stash.stash( file, code );
        }
        virtual bool restore( const boost::filesystem::path& file, task::DeterminantHash code )
        {
            if( bForceNoStash )
                return false;
            return m_stash.restore( file, code );
        }
    } stashImpl( stash, buildHashCodes, bForceNoStash );

    struct Dependencies : public mega::pipeline::DependencyProvider
    {
        boost::shared_ptr< EG_PARSER_INTERFACE > m_pParser;
        Dependencies( const mega::utilities::ToolChain& toolChain )
        {
            try
            {
                m_pParser = boost::dll::import_symbol< EG_PARSER_INTERFACE >(
                    toolChain.parserPath, "g_parserSymbol", boost::dll::load_mode::append_decorations );
                VERIFY_RTE_MSG( m_pParser, "Failed to load parser" );
            }
            catch( std::exception& ex )
            {
                THROW_RTE( "Failed to load parser dll with exception: " << ex.what() );
            }
        }
        EG_PARSER_INTERFACE* getParser() { return m_pParser.get(); }
    } dependencies( toolChain );

    if( !strTaskName.empty() && !strSourceFile.empty() && !bExecuteUpTo )
    {
        mega::pipeline::Schedule schedule = pPipeline->getSchedule( progressReporter, stashImpl );
        osLog << "Running ONLY task: " << strTaskName << " with source: " << strSourceFile << std::endl;
        std::optional< mega::pipeline::TaskDescriptor > taskOpt = schedule.getTask( strTaskName, strSourceFile );
        VERIFY_RTE_MSG( taskOpt.has_value(),
                        "Failed to locate task with name: " << strTaskName << " and source file: " << strSourceFile );
        pPipeline->execute( taskOpt.value(), progressReporter, stashImpl, dependencies );
    }
    else
    {
        mega::pipeline::Schedule schedule = pPipeline->getSchedule( progressReporter, stashImpl );

        if( bExecuteUpTo )
        {
            if( !strSourceFile.empty() )
            {
                mega::pipeline::Schedule schedule = pPipeline->getSchedule( progressReporter, stashImpl );
                osLog << "Running UP TO task: " << strTaskName << " with source: " << strSourceFile << std::endl;
                schedule = schedule.getUpTo( strTaskName, strSourceFile );
            }
            else
            {
                osLog << "Running UP TO task: " << strTaskName << std::endl;
                schedule = schedule.getUpTo( strTaskName );
            }
        }
        while( !schedule.isComplete() && pipelineResult.getSuccess() )
        {
            bool bProgress = false;
            for( const mega::pipeline::TaskDescriptor& task : schedule.getReady() )
            {
                pPipeline->execute( task, progressReporter, stashImpl, dependencies );
                if( !pipelineResult.getSuccess() )
                    break;
                schedule.complete( task );
                bProgress = true;
            }
            VERIFY_RTE_MSG( bProgress, "Failed to make progress executing pipeline" );
        }
        if( pipelineResult.getSuccess() )
        {
            pipelineResult = mega::pipeline::PipelineResult( true, "", buildHashCodes.get() );
        }
    }

    return pipelineResult;
}

} // namespace mega::pipeline
