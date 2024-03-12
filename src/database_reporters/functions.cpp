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

#include "reporters.hpp"

#include "database_reporters/factory.hpp"

#include "environment/environment_archive.hpp"
#include "database/FinalStage.hxx"

#include "report/url.hpp"
#include "mega/values/service/project.hpp"
#include "mega/values/compilation/concrete/type_id.hpp"
#include "mega/values/compilation/interface/type_id.hpp"

#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace FinalStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/interface.hpp"
} // namespace FinalStage

namespace mega::reporters
{

using namespace FinalStage;

Report FunctionsReporter::generate( const URL& url )
{
    using namespace std::string_literals;

    Branch files{ { "Files"s } };

    std::map< TypeID, std::size_t >      objectIDs;
    std::multimap< TypeID, std::size_t > objectGroups;
    for( const mega::io::megaFilePath& sourceFilePath : m_args.manifest.getMegaSourceFiles() )
    {
        Database database( m_args.environment, sourceFilePath );

        Branch fileBranch{ { sourceFilePath.path() } };

        for( Interface::InvocationContext* pContext : database.many< Interface::InvocationContext >( sourceFilePath ) )
        {
            Branch contextBranch{ { Interface::printIContextFullType( pContext ), " "s, pContext->get_interface_id() },
                                  {},
                                  pContext->get_interface_id() };

            for( auto pInvocationInstance : pContext->get_invocation_instances() )
            {
                auto pInvocation = pInvocationInstance->get_invocation();

                Branch invocationBranch{ { "Invocation "s, pInvocation->get_id() } };
                {
                    invocationBranch.m_bookmark = pInvocation->get_id();
                    Table table{ {},

                                 {
                                     { Line{ "Context"s }, Line{ pInvocation->get_canonical_context() } },
                                     { Line{ "Type Path"s }, Line{ pInvocation->get_canonical_type_path() } },
                                     { Line{ "Explicit Operation"s },
                                       Line{ getExplicitOperationString( pInvocation->get_explicit_operation() ) } },

                                 }

                    };
                    invocationBranch.m_elements.push_back( table );
                }
                {
                    Graph graph;
                    //buildGraph( pInvocation, pInvocation->get_root_instruction(), graph );
                    invocationBranch.m_elements.push_back( graph );
                }
                contextBranch.m_elements.push_back( invocationBranch );
            }

            fileBranch.m_elements.push_back( contextBranch );
        }

        for( Concrete::Context* pContext : database.many< Concrete::Context >( sourceFilePath ) )
        {
            Branch contextBranch{ { Concrete::printContextFullType( pContext ), " "s, pContext->get_concrete_id() },
                                  {},
                                  pContext->get_concrete_id() };

            if( auto pInterupt = db_cast< Concrete::Interupt >( pContext ) )
            {
                {
                    Branch dispatches{ { "Interupt Event Dispatches"s } };
                    for( auto pEventDispatch : pInterupt->get_event_dispatches() )
                    {
                        Graph graph;
                        //buildGraph( pEventDispatch, pEventDispatch->get_root_instruction(), graph );
                        dispatches.m_elements.push_back( graph );
                    }
                    contextBranch.m_elements.push_back( dispatches );
                }
            }

            fileBranch.m_elements.push_back( contextBranch );
        }

        files.m_elements.push_back( fileBranch );
    }

    return files;
}

} // namespace mega::reporters
