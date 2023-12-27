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

#include "database/ObjectStage.hxx"

#include "compiler/clang_compilation.hpp"

#include "mega/common_strings.hpp"

#include "mega/values/compilation/hyper_graph.hpp"
#include "mega/values/runtime/pointer.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <memory>

using namespace ObjectStage; // used as view of CPPSourceStage

namespace ObjectStage
{
// #include "compiler/generator_implementation.hpp"
}


namespace mega::compiler
{
    
class Task_CPP_Impl : public BaseTask
{
    const mega::io::cppFilePath& m_sourceFilePath;

public:
    Task_CPP_Impl( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::GeneratedCPPSourceFilePath cppImplFile = m_environment.CPPImpl( m_sourceFilePath );
        start( taskProgress, "Task_CPP_Impl", m_sourceFilePath.path(), cppImplFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash } );

        if( m_environment.restore( cppImplFile, determinant ) )
        {
            m_environment.setBuildHashCode( cppImplFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_sourceFilePath );

        Components::Component* pInterfaceComponent = nullptr;
        {
            for( auto pComponent : database.many< Components::Component >( m_environment.project_manifest() ) )
            {
                if( pComponent->get_type() == mega::ComponentType::eInterface )
                {
                    VERIFY_RTE_MSG( !pInterfaceComponent, "Multiple interface components found" );
                    pInterfaceComponent = pComponent;
                }
            }
        }
        
        std::ostringstream os;
        {
            // ImplementationGen implGen( m_environment, m_sourceFilePath.path().string() );
            // implGen.generate( database.one< Operations::Invocations >( m_sourceFilePath ), os );
            os << "\n";
        }
        
        if( boost::filesystem::updateFileIfChanged( m_environment.FilePath( cppImplFile ), os.str() ) )
        {
            m_environment.setBuildHashCode( cppImplFile );
            m_environment.stash( cppImplFile, determinant );
            succeeded( taskProgress );
        }
        else
        {
            m_environment.setBuildHashCode( cppImplFile );
            cached( taskProgress );
        }
    }
};

BaseTask::Ptr create_Task_CPP_Impl( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
{
    return std::make_unique< Task_CPP_Impl >( taskArguments, sourceFilePath );
}

}
