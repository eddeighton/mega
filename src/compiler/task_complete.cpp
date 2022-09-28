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



#include "base_task.hpp"

#include "database/model/FinalStage.hxx"
#include "database/types/component_type.hpp"
#include "database/types/sources.hpp"
#include <boost/filesystem/operations.hpp>
#include <common/stash.hpp>

namespace mega
{
namespace compiler
{

class Task_InterfaceComponent : public BaseTask
{
    const std::string& m_strComponentName;

public:
    Task_InterfaceComponent( const TaskArguments& taskArguments, const std::string& strComponentName )
        : BaseTask( taskArguments )
        , m_strComponentName( strComponentName )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;

        Database database( m_environment, m_environment.project_manifest() );

        Components::Component* pComponent = nullptr;
        for ( Components::Component* pIter :
              database.template many< Components::Component >( m_environment.project_manifest() ) )
        {
            if ( pIter->get_name() == m_strComponentName )
            {
                pComponent = pIter;
                break;
            }
        }
        VERIFY_RTE( pComponent );
        const mega::io::ComponentFilePath componentFilePath = pComponent->get_file_path();

        start( taskProgress, "Task_InterfaceComponent", m_strComponentName, componentFilePath.path() );

        task::DeterminantHash determinant{ m_toolChain.toolChainHash };
        {
            for ( auto megaSrc : pComponent->get_mega_source_files() )
            {
                determinant ^= m_environment.getBuildHashCode( m_environment.ImplementationObj( megaSrc ) );
            }
            for ( auto cppSrc : pComponent->get_cpp_source_files() )
            {
                determinant ^= m_environment.getBuildHashCode( m_environment.CPPObj( cppSrc ) );
            }
        }

        /*if ( m_environment.restore( componentFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( componentFilePath );
            cached( taskProgress );
            return;
        }*/

        /*const auto actualFilePath = m_environment.FilePath( componentFilePath );
        if ( boost::filesystem::exists( actualFilePath ) )
        {
            // stash it but then delete it!!
            m_environment.setBuildHashCode( componentFilePath );
            m_environment.stash( componentFilePath, determinant );

            // if component has changed then delete existing one in build folder to ensure rebuilt
            boost::filesystem::remove( actualFilePath );
        }*/

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_InterfaceComponent( const TaskArguments& taskArguments, const std::string& strComponentName )
{
    return std::make_unique< Task_InterfaceComponent >( taskArguments, strComponentName );
}

class Task_LibraryComponent : public BaseTask
{
    const std::string& m_strComponentName;

public:
    Task_LibraryComponent( const TaskArguments& taskArguments, const std::string& strComponentName )
        : BaseTask( taskArguments )
        , m_strComponentName( strComponentName )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        using namespace FinalStage;

        Database database( m_environment, m_environment.project_manifest() );

        Components::Component* pComponent = nullptr;
        for ( Components::Component* pIter :
              database.template many< Components::Component >( m_environment.project_manifest() ) )
        {
            if ( pIter->get_name() == m_strComponentName )
            {
                pComponent = pIter;
                break;
            }
        }
        VERIFY_RTE( pComponent );
        const mega::io::ComponentFilePath componentFilePath = pComponent->get_file_path();

        start( taskProgress, "Task_LibraryComponent", m_strComponentName, componentFilePath.path() );

        task::DeterminantHash determinant{ m_toolChain.toolChainHash };
        for ( auto megaSrc : pComponent->get_mega_source_files() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ImplementationObj( megaSrc ) );
        }
        for ( auto cppSrc : pComponent->get_cpp_source_files() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.CPPObj( cppSrc ) );
        }

        /*if ( m_environment.restore( componentFilePath, determinant ) )
        {
            m_environment.setBuildHashCode( componentFilePath );
            cached( taskProgress );
            return;
        }

        // stash it but then delete it!!
        m_environment.setBuildHashCode( componentFilePath );
        m_environment.stash( componentFilePath, determinant );

        // if component has changed then delete existing one in build folder to ensure rebuilt
        const auto actualFilePath = m_environment.FilePath( componentFilePath );
        if ( boost::filesystem::exists( actualFilePath ) )
        {
            boost::filesystem::remove( actualFilePath );
        }*/

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_LibraryComponent( const TaskArguments& taskArguments, const std::string& strComponentName )
{
    return std::make_unique< Task_LibraryComponent >( taskArguments, strComponentName );
}

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