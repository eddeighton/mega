
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

#include "automata.hpp"

#include "database/model/AutomataStage.hxx"
#include "database/types/component_type.hpp"
#include "database/types/sources.hpp"

#include <common/stash.hpp>

#include <vector>

namespace mega::compiler
{

class Task_Automata : public BaseTask
{
    const mega::io::megaFilePath& m_sourceFilePath;

public:
    Task_Automata( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFile
            = m_environment.AutomataStage_AutomataAnalysis( m_sourceFilePath );
        start( taskProgress, "Task_Automata", m_sourceFilePath.path(), compilationFile.path() );

        const task::DeterminantHash determinant(
            { m_toolChain.toolChainHash,
              m_environment.getBuildHashCode( m_environment.ParserStage_Body( m_sourceFilePath ) ) } );

        if( m_environment.restore( compilationFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            cached( taskProgress );
            return;
        }

        using namespace AutomataStage;

        Database database( m_environment, m_sourceFilePath );

        for( Meta::Automata* pAutomata : database.many< Meta::Automata >( m_sourceFilePath ) )
        {
            std::string strBody;
            {
                std::ostringstream osBody;
                for( auto pDef : pAutomata->get_interface_action()->get_action_defs() )
                {
                    if( !pDef->get_body().empty() )
                    {
                        osBody << pDef->get_body();
                        break;
                    }
                }
                strBody = osBody.str();
            }

            automata::TokenVector tokens;
            automata::tokenise( strBody, tokens );

            Automata::Start* pStart = database.construct< Automata::Start >( Automata::Start::Args{ pAutomata, {} } );

            using NodeStack = std::vector< Automata::Node* >;
            NodeStack stack;
            struct Visitor
            {
                Database&        m_database;
                Automata::Start* m_pStart;
                NodeStack&       m_stack;
                Visitor( Database& database, Automata::Start* pStart, NodeStack& stack )
                    : m_database( database )
                    , m_pStart( pStart )
                    , m_stack( stack )
                {
                }
                void operator()( const std::string_view& literal )
                {
                    Automata::Literal* pLit = m_database.construct< Automata::Literal >(
                        Automata::Literal::Args{ Automata::Node::Args{ {} }, std::string{ literal } } );
                    if( m_stack.empty() )
                    {
                        m_pStart->push_back_nodes( pLit );
                    }
                    else
                    {
                        m_stack.back()->push_back_nodes( pLit );
                    }
                }
                void operator()( automata::TokenType token )
                {
                    switch( token )
                    {
                        case mega::automata::eBraceOpen:
                            break;
                        case mega::automata::eBraceClose:
                        {
                            VERIFY_RTE_MSG( !m_stack.empty(), "Invalid stack in automata" );
                            m_stack.pop_back();
                        }
                        break;
                        case mega::automata::eMaybe:
                        case mega::automata::eOr:
                        {
                            Automata::Alternative* pAlt = m_database.construct< Automata::Alternative >(
                                Automata::Alternative::Args{ Automata::Node::Args{ {} } } );
                            if( m_stack.empty() )
                            {
                                m_pStart->push_back_nodes( pAlt );
                            }
                            else
                            {
                                m_stack.back()->push_back_nodes( pAlt );
                            }
                            m_stack.push_back( pAlt );
                        }
                        break;
                        case mega::automata::eRepeat:
                        {
                            Automata::Repeat* pRepeat = m_database.construct< Automata::Repeat >(
                                Automata::Repeat::Args{ Automata::Node::Args{ {} } } );
                            if( m_stack.empty() )
                            {
                                m_pStart->push_back_nodes( pRepeat );
                            }
                            else
                            {
                                m_stack.back()->push_back_nodes( pRepeat );
                            }
                            m_stack.push_back( pRepeat );
                        }
                        break;
                        case mega::automata::eInterupt:
                        {
                            Automata::Interupt* pInterupt = m_database.construct< Automata::Interupt >(
                                Automata::Interupt::Args{ Automata::Node::Args{ {} } } );
                            if( m_stack.empty() )
                            {
                                m_pStart->push_back_nodes( pInterupt );
                            }
                            else
                            {
                                m_stack.back()->push_back_nodes( pInterupt );
                            }
                            m_stack.push_back( pInterupt );
                        }
                        break;
                        case mega::automata::TOTAL_TOKENS:
                        default:
                            break;
                    }
                }
            } visitor( database, pStart, stack );

            for( const auto& token : tokens )
            {
                std::visit( visitor, token );
            }
            VERIFY_RTE_MSG( stack.empty(), "Invalid stack in automata" );
        }

        const task::FileHash fileHashCode = database.save_AutomataAnalysis_to_temp();
        m_environment.setBuildHashCode( compilationFile, fileHashCode );
        m_environment.temp_to_real( compilationFile );
        m_environment.stash( compilationFile, determinant );

        succeeded( taskProgress );
    }
};

BaseTask::Ptr create_Task_Automata( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_Automata >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
