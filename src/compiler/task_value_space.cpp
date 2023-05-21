
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

#include "database/model/ValueSpaceStage.hxx"

#include "database/types/sources.hpp"

#include <common/stash.hpp>

#include <vector>
#include <sstream>

namespace mega::compiler
{

class Task_ValueSpace : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_ValueSpace( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

private:
    using BlockRange  = std::pair< mega::U64, mega::U64 >;
    using BlockRanges = std::unordered_map< mega::U64, BlockRange >;

    void recurseBlocks( ValueSpaceStage::Database& database, ValueSpaceStage::Operations::Invocations* pInvocations,
                        const BlockRanges& blockRanges, ValueSpaceStage::Automata::Block* pBlock )
    {
        using namespace ValueSpaceStage;

        const mega::U64 id    = pBlock->get_id();
        auto            iFind = blockRanges.find( id );
        VERIFY_RTE_MSG( iFind != blockRanges.end(), "Failed to locate block: " << id );
        const BlockRange& blockRange = iFind->second;
        ASSERT( blockRange.second > blockRange.first );

        using Actions = std::vector< Interface::Action* >;
        Actions blockActions;
        {
            for( const auto& [ id, pInvocation ] : pInvocations->get_invocations() )
            {
                if( pInvocation->get_explicit_operation() == id_exp_Start )
                {
                    for( const auto offset : pInvocation->get_file_offsets() )
                    {
                        if( ( offset >= blockRange.first ) && ( offset <= blockRange.second ) )
                        {
                            // is this invocation for an automata action or animation action?
                            Actions actions;
                            bool    bNonActionInvocation = false;
                            for( auto pContext : pInvocation->get_return_type_contexts() )
                            {
                                if( auto pAction = db_cast< Interface::Action >( pContext ) )
                                {
                                    actions.push_back( pAction );
                                }
                                else
                                {
                                    bNonActionInvocation = true;
                                }
                            }
                            VERIFY_RTE_MSG( actions.empty() || bNonActionInvocation == false,
                                            "Invalid action invocation in block" );

                            if( !actions.empty() )
                            {
                                for( auto pAction : actions )
                                {
                                    blockActions.push_back( pAction );
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }

        database.construct< Automata::Block >( Automata::Block::Args{ pBlock, blockActions } );

        for( Automata::Node* pNodeChild : pBlock->get_nodes() )
        {
            if( auto pNestedBlock = db_cast< Automata::Block >( pNodeChild ) )
            {
                recurseBlocks( database, pInvocations, blockRanges, pNestedBlock );
            }
        }
    }

public:
    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFile
            = m_environment.ValueSpaceStage_ValueSpace( m_sourceFilePath );
        start( taskProgress, "Task_ValueSpace", m_sourceFilePath.path(), compilationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash, m_environment.getBuildHashCode( m_environment.Operations( m_sourceFilePath ) ),
              m_environment.getBuildHashCode( m_environment.AutomataStage_AutomataAnalysis( m_sourceFilePath ) ) } );

        if( m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        using namespace ValueSpaceStage;
        Database database( m_environment, m_sourceFilePath );

        const io::GeneratedHPPSourceFilePath operationsHeader = m_environment.Operations( m_sourceFilePath );

        // find all blocks
        BlockRanges blockRanges;
        {
            std::string strOperationsHeaderText;
            boost::filesystem::loadAsciiFile( m_environment.FilePath( operationsHeader ), strOperationsHeaderText );

            static const std::string strBlockBegin = "//_MEGABEGIN";
            static const std::string strBlockEnd   = "//_MEGAEND";
            for( auto i = strOperationsHeaderText.cbegin(), iBegin = strOperationsHeaderText.cbegin(),
                      iEnd = strOperationsHeaderText.cend();
                 i != iEnd; )
            {
                i = std::search( i, iEnd, strBlockBegin.cbegin(), strBlockBegin.cend() );
                if( i != iEnd )
                {
                    const std::string::const_iterator iStart = i;
                    i += strBlockBegin.size();

                    mega::U64 blockID;
                    {
                        std::stringstream os;
                        while( std::isalnum( *i ) )
                        {
                            os << *i;
                            ++i;
                        }
                        os >> blockID;
                    }

                    i = std::search( i, iEnd, strBlockEnd.cbegin(), strBlockEnd.cend() );
                    VERIFY_RTE_MSG( i != iEnd, "Failed to locate block end" );
                    i += strBlockEnd.size();

                    VERIFY_RTE(
                        blockRanges
                            .insert( { blockID, { std::distance( iBegin, iStart ), std::distance( iBegin, i ) } } )
                            .second );
                }
            }
        }

        Operations::Invocations* pInvocations = database.one< Operations::Invocations >( m_sourceFilePath );

        for( auto pStart : database.many< Automata::Start >( m_sourceFilePath ) )
        {
            recurseBlocks( database, pInvocations, blockRanges, pStart->get_sequence() );
        }

        const task::FileHash fileHashCode = database.save_ValueSpace_to_temp();
        m_environment.setBuildHashCode( compilationFile, fileHashCode );
        m_environment.temp_to_real( compilationFile );
        m_environment.stash( compilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_ValueSpace( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ValueSpace >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
