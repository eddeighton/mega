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

#ifndef REPORTERS_24_OCT_2023
#define REPORTERS_24_OCT_2023

#include "database_reporters/compilation_reporter_args.hpp"

#include "database/FinalStage.hxx"
#include "environment/environment_archive.hpp"
#include "reports/report.hpp"
#include "reports/reporter_id.hpp"
#include "mega/values/service/url.hpp"

namespace mega::reporters
{

struct SymbolsReporter
{
    CompilationReportArgs            m_args;
    static const reports::ReporterID ID;
    reports::Container               generate( const reports::URL& url );
};

struct InterfaceTypeIDReporter
{
    CompilationReportArgs            m_args;
    static const reports::ReporterID ID;
    reports::Container               generate( const reports::URL& url );
};

struct ConcreteTypeIDReporter
{
    CompilationReportArgs            m_args;
    static const reports::ReporterID ID;
    reports::Container               generate( const reports::URL& url );
};

class InterfaceReporter
{
    void addInheritance( std::optional< FinalStage::Interface::InheritanceTrait* > inheritance,
                         reports::Branch&                                          branch );
    void addProperties( reports::Branch& typeIDs, reports::Branch& parentBranch,
                        const std::vector< FinalStage::Interface::DimensionTrait* >& dimensions );

    void recurse( reports::Branch& typeIDs, reports::Branch& parentBranch, FinalStage::Interface::IContext* pContext );

public:
    CompilationReportArgs            m_args;
    static const reports::ReporterID ID;
    reports::Container               generate( const reports::URL& url );
};

struct InheritanceReporter
{
    CompilationReportArgs            m_args;
    static const reports::ReporterID ID;
    reports::Container               generate( const reports::URL& url );
};

class AndOrTreeReporter
{
    std::size_t recurse( reports::Graph& graph, FinalStage::Automata::Vertex* pVertex,
                         std::vector< std::size_t >& nodes );

public:
    CompilationReportArgs            m_args;
    static const reports::ReporterID ID;
    reports::Container               generate( const reports::URL& url );
};

class EnumReporter
{
    using SwitchIndex       = U32;
    using SwitchIDToNodeMap = std::map< SwitchIndex, reports::Graph::Node::ID >;

    struct Edges
    {
        std::multimap< SwitchIndex, SwitchIndex > treeEdges, siblingEdges, failureEdges;
    };
    std::size_t recurse( reports::Graph& graph, FinalStage::Automata::Enum* pEnum, std::vector< std::size_t >& nodes,
                         SwitchIDToNodeMap& nodeMap, Edges& edges );

public:
    CompilationReportArgs            m_args;
    static const reports::ReporterID ID;
    reports::Container               generate( const reports::URL& url );
};

} // namespace mega::reporters

#endif // REPORTERS_24_OCT_2023
