
#ifndef PIPELINE_27_MAY_2022
#define PIPELINE_27_MAY_2022

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

class Task
{
public:
    using Vector = std::vector< Task >;

    Task();
    Task( const std::string& taskName );

    inline bool operator<( const Task& cmp ) const { return m_taskName < cmp.m_taskName; }
    inline bool operator==( const Task& cmp ) const { return m_taskName == cmp.m_taskName; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_taskName;
    }

    const std::string& str() const { return m_taskName; }

private:
    std::string m_taskName;
};

class Dependencies
{
public:
    using TaskSet = std::set< Task >;
    using Graph   = std::multimap< Task, Task >;
    void           add( const Task& newTask, const Task::Vector& dependencies );
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

    Task::Vector getReady() const;

    void complete( const Task& task ) { m_complete.insert( task ); }

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

    virtual void onStarted()   = 0;
    virtual void onProgress()  = 0;
    virtual void onCompleted() = 0;
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
    using ID  = std::string;

    virtual ~Pipeline();
    virtual Schedule getSchedule()                                   = 0;
    virtual void     execute( const Task& task, Progress& progress ) = 0;
};

class Registry
{
    using PipelinePtrMap = std::map< Pipeline::ID, Pipeline::Ptr >;

public:
    Registry();
    ~Registry();
    Pipeline::Ptr getPipeline( const Pipeline::ID& id );

private:
    PipelinePtrMap m_pipelines;
};
} // namespace pipeline
} // namespace mega

#endif // PIPELINE_27_MAY_2022
