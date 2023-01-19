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

#include "database/model/DerivationAnalysis.hxx"
#include "database/model/HyperGraphAnalysis.hxx"
#include "database/model/HyperGraphAnalysisView.hxx"
#include "database/model/HyperGraphAnalysisRollout.hxx"
#include "database/model/manifest.hxx"

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"
#include "database/types/cardinality.hpp"

namespace mega::compiler
{

class Task_HyperGraph : public BaseTask
{
    const mega::io::Manifest m_manifest;

public:
    Task_HyperGraph( const TaskArguments& taskArguments, const mega::io::manifestFilePath& manifestFilePath )
        : BaseTask( taskArguments )
        , m_manifest( m_environment, manifestFilePath )
    {
    }

    HyperGraphAnalysis::Interface::IContext* getLinkTarget( HyperGraphAnalysis::Interface::Link* pLink ) const
    {
        const auto targets = pLink->get_link_interface()->get_contexts();
        VERIFY_RTE( targets.size() == 1 );
        return targets.front();
    }

    std::map< HyperGraphAnalysis::Interface::Link*, HyperGraphAnalysis::HyperGraph::Relation* >
    calculateGraph( HyperGraphAnalysis::Database&                                       database,
                    const std::vector< HyperGraphAnalysis::Interface::Link* >&          links,
                    const std::vector< HyperGraphAnalysis::Interface::LinkInterface* >& linkInterfaces ) const
    {
        using namespace HyperGraphAnalysis;
        using namespace HyperGraphAnalysis::HyperGraph;

        struct RelTemp
        {
            using Ptr = std::shared_ptr< RelTemp >;
            Interface::LinkInterface *      pLinkInterfaceSrc, *pLinkInterfaceTarget;
            std::vector< Interface::Link* > sources, targets;
        };
        std::vector< RelTemp::Ptr >                         tempRelations;
        std::map< Interface::LinkInterface*, RelTemp::Ptr > temp;

        // establish all relations with their pair of link interfaces
        for( Interface::LinkInterface* pLinkInterface : linkInterfaces )
        {
            auto iFind = temp.find( pLinkInterface );
            if( iFind == temp.end() )
            {
                auto pLinkTarget          = getLinkTarget( pLinkInterface );
                auto pLinkInterfaceTarget = db_cast< Interface::LinkInterface >( pLinkTarget );
                VERIFY_RTE_MSG( pLinkInterfaceTarget, "Link interface not to corresponding link interface" );

                RelTemp::Ptr pRelation( new RelTemp{ pLinkInterface, pLinkInterfaceTarget, {}, {} } );
                tempRelations.push_back( pRelation );
                temp.insert( { pLinkInterface, pRelation } );
                temp.insert( { pLinkInterfaceTarget, pRelation } );
            }
        }

        // associate all links with corresponding relation as either a source or target
        for( Interface::Link* pLink : links )
        {
            VERIFY_RTE( !db_cast< Interface::LinkInterface >( pLink ) );
            
            auto pLinkInterface = db_cast< Interface::LinkInterface >( getLinkTarget( pLink ) );
            VERIFY_RTE_MSG( pLinkInterface, "Link does not inherit link interface correctly" );

            auto iFind = temp.find( pLinkInterface );
            VERIFY_RTE( iFind != temp.end() );
            auto pRelation = iFind->second;

            if( pRelation->pLinkInterfaceSrc == pLinkInterface )
            {
                pRelation->sources.push_back( pLink );
            }
            else if( pRelation->pLinkInterfaceTarget == pLinkInterface )
            {
                pRelation->targets.push_back( pLink );
            }
            else
            {
                THROW_RTE( "Error resolving link interface" );
            }
        }

        // construct the actual relations
        std::map< Interface::Link*, Relation* > relations;
        for( RelTemp::Ptr pTempRel : tempRelations )
        {
            Relation* pRelation = database.construct< Relation >( Relation::Args{
                pTempRel->sources, pTempRel->targets, pTempRel->pLinkInterfaceSrc, pTempRel->pLinkInterfaceTarget } );

            relations.insert( { pTempRel->pLinkInterfaceSrc, pRelation } );
            relations.insert( { pTempRel->pLinkInterfaceTarget, pRelation } );
            for( auto pLink : pTempRel->sources )
            {
                relations.insert( { pLink, pRelation } );
            }
            for( auto pLink : pTempRel->targets )
            {
                relations.insert( { pLink, pRelation } );
            }
        }

        return relations;
    }

    using PathSet = std::set< mega::io::megaFilePath >;
    PathSet getSortedSourceFiles() const
    {
        PathSet sourceFiles;
        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            sourceFiles.insert( sourceFilePath );
        }
        return sourceFiles;
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::manifestFilePath    manifestFilePath = m_environment.project_manifest();
        const mega::io::CompilationFilePath hyperGraphCompilationFile
            = m_environment.HyperGraphAnalysis_Model( manifestFilePath );
        start( taskProgress, "Task_HyperGraph", manifestFilePath.path(), hyperGraphCompilationFile.path() );

        task::DeterminantHash determinant( m_toolChain.toolChainHash );
        for( const mega::io::megaFilePath& sourceFilePath : m_manifest.getMegaSourceFiles() )
        {
            determinant ^= m_environment.getBuildHashCode( m_environment.ParserStage_AST( sourceFilePath ) );
        }

        if( m_environment.restore( hyperGraphCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( hyperGraphCompilationFile );
            cached( taskProgress );
            return;
        }

        struct InterfaceHashCodeGenerator
        {
            const mega::io::StashEnvironment& env;
            task::DeterminantHash             toolChainHash;
            InterfaceHashCodeGenerator( const mega::io::StashEnvironment& env, task::DeterminantHash toolChainHash )
                : env( env )
                , toolChainHash( toolChainHash )
            {
            }
            inline task::DeterminantHash operator()( const mega::io::megaFilePath& megaFilePath ) const
            {
                return task::DeterminantHash(
                    { toolChainHash, env.getBuildHashCode( env.ParserStage_AST( megaFilePath ) ) } );
            }

        } hashCodeGenerator( m_environment, m_toolChain.toolChainHash );

        using namespace HyperGraphAnalysis;
        using namespace HyperGraphAnalysis::HyperGraph;

        Database database( m_environment, manifestFilePath );
        {
            std::vector< Interface::Link* >          links;
            std::vector< Interface::LinkInterface* > linkInterfaces;

            for( const mega::io::megaFilePath& sourceFilePath : getSortedSourceFiles() )
            {
                for( Interface::Link* pLink : database.many< Interface::Link >( sourceFilePath ) )
                {
                    if( auto pLinkInterface = db_cast< Interface::LinkInterface >( pLink ) )
                    {
                        linkInterfaces.push_back( pLinkInterface );
                    }
                    else
                    {
                        links.push_back( pLink );
                    }
                }
            }
            database.construct< Graph >( Graph::Args{ calculateGraph( database, links, linkInterfaces ) } );
        }

        const task::FileHash fileHashCode = database.save_Model_to_temp();
        m_environment.setBuildHashCode( hyperGraphCompilationFile, fileHashCode );
        m_environment.temp_to_real( hyperGraphCompilationFile );
        m_environment.stash( hyperGraphCompilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_HyperGraph( const TaskArguments&              taskArguments,
                                      const mega::io::manifestFilePath& manifestFilePath )
{
    return std::make_unique< Task_HyperGraph >( taskArguments, manifestFilePath );
}

class Task_HyperGraphRollout : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_HyperGraphRollout( const TaskArguments& taskArguments, const mega::io::megaFilePath& megaSourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( megaSourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath hyperGraphAnalysisCompilationFile
            = m_environment.HyperGraphAnalysis_Model( m_environment.project_manifest() );
        const mega::io::CompilationFilePath rolloutCompilationFile
            = m_environment.HyperGraphAnalysisRollout_PerSourceModel( m_sourceFilePath );
        start( taskProgress, "Task_ConcreteTypeRollout", m_sourceFilePath.path(), rolloutCompilationFile.path() );

        const task::DeterminantHash determinant
            = { m_environment.getBuildHashCode( hyperGraphAnalysisCompilationFile ) };

        if( m_environment.restore( rolloutCompilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( rolloutCompilationFile );
            cached( taskProgress );
            return;
        }

        using namespace HyperGraphAnalysisRollout;

        Database database( m_environment, m_sourceFilePath );

        {
            HyperGraph::Graph* pHyperGraph = database.one< HyperGraph::Graph >( m_environment.project_manifest() );
            auto relations = pHyperGraph->get_relations();
            for( Interface::Link* pLink : database.many< Interface::Link >( m_sourceFilePath ) )
            {
                auto iFind = relations.find( pLink );
                VERIFY_RTE( iFind != relations.end() );
                database.construct< Interface::Link >( Interface::Link::Args{ pLink, iFind->second } );
            }
        }

        const task::FileHash fileHashCode = database.save_PerSourceModel_to_temp();
        m_environment.setBuildHashCode( rolloutCompilationFile, fileHashCode );
        m_environment.temp_to_real( rolloutCompilationFile );
        m_environment.stash( rolloutCompilationFile, determinant );
        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_HyperGraphRollout( const TaskArguments&          taskArguments,
                                             const mega::io::megaFilePath& megaSourceFilePath )
{
    return std::make_unique< Task_HyperGraphRollout >( taskArguments, megaSourceFilePath );
}

} // namespace mega::compiler
