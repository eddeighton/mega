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

#ifndef PIPELINE_27_MAY_2022
#define PIPELINE_27_MAY_2022

#include "task.hpp"
#include "stash.hpp"
#include "configuration.hpp"

#include "utilities/tool_chain_hash.hpp"

#include "version/version.hpp"

#include <boost/config.hpp>
#include <boost/shared_ptr.hpp>

#include <optional>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>

struct EG_PARSER_INTERFACE;

namespace mega::pipeline
{

class Dependencies
{
public:
    using TaskSet = std::set< TaskDescriptor >;
    using Graph   = std::multimap< TaskDescriptor, TaskDescriptor >;
    void           add( const TaskDescriptor& newTask, const TaskDescriptor::Vector& dependencies );
    const TaskSet& getTasks() const { return m_tasks; }
    const Graph&   getDependencies() const { return m_graph; }

private:
    TaskSet m_tasks;
    Graph   m_graph;
};

class Schedule
{
public:
    Schedule( const Dependencies& dependencies );

    TaskDescriptor::Vector          getReady() const;
    std::optional< TaskDescriptor > getTask( const std::string& strTaskName, const std::string& strSourceFile ) const;
    bool                            isComplete() const { return m_dependencies.getTasks() == m_complete; }

    void complete( const TaskDescriptor& task ) { m_complete.insert( task ); }

private:
    Dependencies          m_dependencies;
    Dependencies::TaskSet m_complete;
};

class BOOST_SYMBOL_VISIBLE Progress
{
    Progress( const Progress& )            = delete;
    Progress& operator=( const Progress& ) = delete;

protected:
    Progress();

public:
    virtual ~Progress();

    virtual void onStarted( const std::string& strMsg )   = 0;
    virtual void onProgress( const std::string& strMsg )  = 0;
    virtual void onFailed( const std::string& strMsg )    = 0;
    virtual void onCompleted( const std::string& strMsg ) = 0;
};

class DependencyProvider
{
public:
    virtual ~DependencyProvider();

    virtual EG_PARSER_INTERFACE* getParser() = 0;
};

class BOOST_SYMBOL_VISIBLE Pipeline
{
    friend class Registry;
    Pipeline( const Pipeline& )            = delete;
    Pipeline& operator=( const Pipeline& ) = delete;

protected:
    Pipeline();

    virtual void initialise( const mega::utilities::ToolChain& toolChain, const Configuration& configuration,
                             std::ostream& osLog )
        = 0;

public:
    using Ptr = boost::shared_ptr< Pipeline >;

    virtual ~Pipeline();

    virtual Schedule getSchedule( Progress& progress, Stash& stash ) = 0;
    virtual void     execute( const TaskDescriptor& task, Progress& progress, Stash& stash,
                              DependencyProvider& dependencies )
        = 0;
};

class Registry
{
public:
    static Pipeline::Ptr getPipeline( const mega::utilities::ToolChain& toolChain, const Configuration& configuration,
                                      std::ostream& osLog );
};
} // namespace mega::pipeline

#endif // PIPELINE_27_MAY_2022
