
#ifndef PIPELINE_27_MAY_2022
#define PIPELINE_27_MAY_2022

#include "task.hpp"
#include "stash.hpp"
#include "configuration.hpp"

#include "version/version.hpp"

#include "boost/config.hpp"
#include "boost/shared_ptr.hpp"

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>

namespace mega
{
namespace pipeline
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

    TaskDescriptor::Vector getReady() const;

    void complete( const TaskDescriptor& task ) { m_complete.insert( task ); }

    bool isComplete() const { return m_dependencies.getTasks() == m_complete; }

private:
    Dependencies          m_dependencies;
    Dependencies::TaskSet m_complete;
};

class BOOST_SYMBOL_VISIBLE Progress
{
    Progress( const Progress& ) = delete;
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

class BOOST_SYMBOL_VISIBLE Pipeline
{
    friend class Registry;
    Pipeline( const Pipeline& ) = delete;
    Pipeline& operator=( const Pipeline& ) = delete;

protected:
    Pipeline();

public:
    using Ptr = boost::shared_ptr< Pipeline >;

    virtual ~Pipeline();

    virtual void     initialise( const Configuration& configuration )                        = 0;
    virtual Schedule getSchedule( Progress& progress, Stash& stash )                         = 0;
    virtual void     execute( const TaskDescriptor& task, Progress& progress, Stash& stash ) = 0;
};

class Registry
{
public:
    static Pipeline::Ptr getPipeline( const Configuration& configuration );
};
} // namespace pipeline
} // namespace mega

#endif // PIPELINE_27_MAY_2022
