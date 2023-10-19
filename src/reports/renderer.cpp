
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

#include "reports/renderer.hpp"
#include "reports/report.hpp"
#include "reports/reporter.hpp"

#include "mega/any_io.hpp"
#include "mega/bitset_io.hpp"
#include "mega/invocation_io.hpp"
#include "mega/reference_io.hpp"
#include "mega/operator_io.hpp"
#include "mega/type_id_io.hpp"
#include "mega/native_types_io.hpp"
#include "mega/relation_io.hpp"

#include "common/process.hpp"
#include "common/file.hpp"
#include "common/string.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include "boost/filesystem.hpp"

namespace mega::reports
{
namespace
{

class Inja
{
    ::inja::Environment     m_injaEnvironment;
    boost::filesystem::path m_tempFolder;

    enum TemplateType
    {
        eReport,
        eMultiLine,
        eBranch,
        eTable,
        eGraph,
        TOTAL_TEMPLATE_TYPES
    };

    std::array< std::string, TOTAL_TEMPLATE_TYPES >      m_templateNames;
    std::array< ::inja::Template, TOTAL_TEMPLATE_TYPES > m_templates;

    void renderTemplate( const nlohmann::json& data, TemplateType templateType, std::ostream& os )
    {
        try
        {
            m_injaEnvironment.render_to( os, m_templates[ templateType ], data );
        }
        catch( ::inja::RenderError& ex )
        {
            THROW_RTE( "Inja Exception rendering template: " << m_templateNames[ templateType ]
                                                             << " error: " << ex.what() );
        }
        catch( std::exception& ex )
        {
            THROW_RTE( "Exception rendering template: " << m_templateNames[ templateType ] << " error: " << ex.what() );
        }
    }

public:
    Inja( const boost::filesystem::path& templateDir )
        : m_templateNames{ "report.jinja", "multiline.jinja", "branch.jinja", "table.jinja", "graph.jinja" }
        , m_tempFolder( boost::filesystem::temp_directory_path() / "graphs" / common::uuid() )
    {
        boost::filesystem::create_directories( m_tempFolder );
        VERIFY_RTE_MSG(
            boost::filesystem::exists( m_tempFolder ), "Failed to create temporary folder: " << m_tempFolder.string() );

        m_injaEnvironment.set_trim_blocks( true );
        for( int i = 0; i != TOTAL_TEMPLATE_TYPES; ++i )
        {
            const auto templateType = static_cast< TemplateType >( i );
            auto       templatePath = templateDir / m_templateNames[ templateType ];
            VERIFY_RTE_MSG( boost::filesystem::exists( templatePath ),
                            "Failed to locate report template: " << templatePath.string() );
            m_templates[ templateType ] = m_injaEnvironment.parse_template( templatePath.string() );
        }
    }

    ~Inja()
    {
        //
        boost::filesystem::remove_all( m_tempFolder );
    }

    void renderReport( const nlohmann::json& data, std::ostream& os ) { renderTemplate( data, eReport, os ); }

    void renderMultiLine( const nlohmann::json& data, std::ostream& os )
    {
        //
        renderTemplate( data, eMultiLine, os );
    }

    void renderBranch( const nlohmann::json& data, std::ostream& os )
    {
        //
        renderTemplate( data, eBranch, os );
    }

    void renderTable( const nlohmann::json& data, std::ostream& os )
    {
        //
        renderTemplate( data, eTable, os );
    }

    void renderGraph( const nlohmann::json& data, std::ostream& os )
    {
        std::ostringstream osDot;
        renderTemplate( data, eGraph, osDot );

        boost::filesystem::path tempDotFile = m_tempFolder / "temp.dot";
        boost::filesystem::path tempSVGFile = m_tempFolder / "temp.svg";

        // write the temporary file
        {
            auto pTempFile = boost::filesystem::createNewFileStream( tempDotFile );
            *pTempFile << osDot.str();
        }

        std::ostringstream osCmd;
        osCmd << "dot -Tsvg -o" << tempSVGFile.string() << " " << tempDotFile.string();

        std::string strOutput, strError;
        const int   iExitCode = common::runProcess( osCmd.str(), strOutput, strError );
        VERIFY_RTE_MSG( strError.empty(), "Graphviz failed with error: " << strError );
        VERIFY_RTE_MSG( strOutput.empty(), "Graphviz failed with output: " << strOutput );

        boost::filesystem::loadAsciiFile( tempSVGFile, os );
    }

    std::string render( const std::string& strTemplate, const nlohmann::json& data )
    {
        try
        {
            return m_injaEnvironment.render( strTemplate, data );
        }
        catch( inja::InjaError& ex )
        {
            THROW_RTE( "inja::InjaError in CodeGenerator::render rendering: " << ex.what() );
        }
    }
};

struct Args
{
    Inja&     inja;
    Reporter* pLinker = nullptr;
};

void renderValue( Args& args, const Value& value, std::ostream& os )
{
    using ::operator<<;

    std::ostringstream osValue;
    {
        // clang-format off
        struct CompilerTimeVisitor
        {
            std::ostream& os;
            void operator()( const mega::TypeID                 & value ) const { os << value; }
            void operator()( const mega::SubTypeInstance        & value ) const { os << value; }
            void operator()( const mega::TypeInstance           & value ) const { os << value; }
            void operator()( const mega::InvocationID           & value ) const { os << value; }
            void operator()( const mega::OperationID            & value ) const { os << value; }
            void operator()( const mega::ExplicitOperationID    & value ) const { os << value; }
            void operator()( const mega::RelationID             & value ) const { os << value; }
        } compilerTimeVisitor{ osValue };

        struct RuntimeVisitor
        {
            std::ostream& os;
            void operator()( const mega::Any                    & value ) const { os << value; }
            void operator()( const mega::MP                     & value ) const { os << value; }
            void operator()( const mega::MPO                    & value ) const { os << value; }
            void operator()( const mega::reference              & value ) const { os << value; }
            void operator()( const mega::ReferenceVector        & value ) const { os << value; }
            void operator()( const mega::LinkTypeVector         & value ) const { os << value; }
            void operator()( const mega::BitSet                 & value ) const { os << value; }
            
        } runtimeVisitor{ osValue };

        struct Visitor
        {
            CompilerTimeVisitor& compilerTimeVisitor;
            RuntimeVisitor&      runtimeVisitor;
            void operator()( const CompileTimeIdentities& value )   const { std::visit( compilerTimeVisitor, value ); }
            void operator()( const RuntimeValue& value )            const { std::visit( runtimeVisitor, value ); }
        } visitor{ compilerTimeVisitor, runtimeVisitor };
        std::visit( visitor, value );
        // clang-format on
    }

    bool bRendered = false;
    if( args.pLinker )
    {
        if( auto urlOpt = args.pLinker->link( value ); urlOpt.has_value() )
        {
            os << "<a href=\"" << urlOpt.value() << "\" >" << osValue.str() << "</a>";
            bRendered = true;
        }
    }
    if( !bRendered )
    {
        os << osValue.str();
    }
}

void textToJSON( Args& args, const Text& text, nlohmann::json& data )
{
    struct Visitor
    {
        Args&           args;
        nlohmann::json& data;
        void            operator()( const std::string& str ) const { data.push_back( str ); }
        void            operator()( const Value& value ) const
        {
            std::ostringstream osValue;
            renderValue( args, value, osValue );
            data.push_back( osValue.str() );
        }
    } visitor{ args, data };
    std::visit( visitor, text );
}

void textVectorToJSON( Args& args, const TextVector& textVector, nlohmann::json& data )
{
    for( const auto& text : textVector )
    {
        textToJSON( args, text, data );
    }
}

void renderLine( Args& args, const Line& line, std::ostream& os )
{
    nlohmann::json data( { { "style", "multiline_default" }, { "elements", nlohmann::json::array() } } );
    textToJSON( args, line.m_element, data[ "elements" ] );
    args.inja.renderMultiLine( data, os );
}

void renderMultiline( Args& args, const Multiline& multiline, std::ostream& os )
{
    nlohmann::json data( { { "style", "multiline_default" }, { "elements", nlohmann::json::array() } } );

    textVectorToJSON( args, multiline.m_elements, data[ "elements" ] );

    args.inja.renderMultiLine( data, os );
}

void renderContainer( Args& args, const Container& container, std::ostream& os );

void renderBranch( Args& args, const Branch& branch, std::ostream& os )
{
    nlohmann::json data( { { "style", "branch_default" },
                           { "elements", nlohmann::json::array() },
                           { "children", nlohmann::json::array() } } );

    textVectorToJSON( args, branch.m_label, data[ "elements" ] );

    for( const auto& pChildElement : branch.m_elements )
    {
        std::ostringstream osChild;
        renderContainer( args, pChildElement, osChild );
        data[ "children" ].push_back( osChild.str() );
    }

    args.inja.renderBranch( data, os );
}

void renderTable( Args& args, const Table& table, std::ostream& os )
{
    nlohmann::json data( { { "headings", nlohmann::json::array() }, { "rows", nlohmann::json::array() } } );

    for( const auto& heading : table.m_headings )
    {
        data[ "headings" ].push_back( heading );
    }

    for( const auto& pRow : table.m_rows )
    {
        nlohmann::json row( { { "values", nlohmann::json::array() } } );
        for( const auto pContainer : pRow )
        {
            std::ostringstream osChild;
            renderContainer( args, pContainer, osChild );
            row[ "values" ].push_back( osChild.str() );
        }
        data[ "rows" ].push_back( row );
    }

    args.inja.renderTable( data, os );
}

void renderGraph( Args& args, const Graph& graph, std::ostream& os )
{
    using namespace std::string_literals;

    nlohmann::json data( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

    std::vector< std::string > nodeNames;
    for( const auto& node : graph.m_nodes )
    {
        std::ostringstream osNodeName;
        osNodeName << "node_" << nodeNames.size();
        nodeNames.push_back( osNodeName.str() );

        nlohmann::json nodeData( {

            { "name", osNodeName.str() },
            { "colour", node.m_colour.str() },
            { "rows", nlohmann::json::array() },
            { "has_url", false },
            { "has_bookmark", false }

        } );

        if( node.m_url.has_value() )
        {
            nodeData[ "has_url" ] = true;
            nodeData[ "url" ]     = node.m_url.value().str();
        }

        if( node.m_bookmark.has_value() )
        {
            // render bookmark WITHOUT url value
            std::ostringstream osBookmark;
            Args               bookmarkArgs{ args.inja, nullptr };
            renderValue( bookmarkArgs, node.m_bookmark.value(), osBookmark );
            nodeData[ "has_bookmark" ] = true;
            nodeData[ "bookmark" ]     = osBookmark.str();
        }

        for( const TextVector& row : node.m_rows )
        {
            nlohmann::json rowData( { { "values", nlohmann::json::array() } } );
            for( const Text& text : row )
            {
                textToJSON( args, text, rowData[ "values" ] );
            }
            nodeData[ "rows" ].push_back( rowData );
        }

        data[ "nodes" ].push_back( nodeData );
    }

    for( const auto& edge : graph.m_edges )
    {
        VERIFY_RTE_MSG( ( edge.m_source ) >= 0 && ( edge.m_source < nodeNames.size() ),
                        "Invalid edge node id of: " << edge.m_source );
        VERIFY_RTE_MSG( ( edge.m_target ) >= 0 && ( edge.m_target < nodeNames.size() ),
                        "Invalid edge node id of: " << edge.m_target );
                        
        nlohmann::json edgeData( {

            { "from", nodeNames[ edge.m_source ] },
            { "to", nodeNames[ edge.m_target ] },
            { "colour", edge.m_colour.str() },
            { "style", edge.m_style.str() }

        } );

        data[ "edges" ].push_back( edgeData );
    }

    args.inja.renderGraph( data, os );
}

void renderContainer( Args& args, const Container& container, std::ostream& os )
{
    using namespace mega::reports;

    struct Visitor
    {
        Args&         args;
        std::ostream& os;
        void          operator()( const Line& line ) const { renderLine( args, line, os ); }
        void          operator()( const Multiline& multiline ) const { renderMultiline( args, multiline, os ); }
        void          operator()( const Branch& branch ) const { renderBranch( args, branch, os ); }
        void          operator()( const Table& table ) const { renderTable( args, table, os ); }
        void          operator()( const Graph& graph ) const { renderGraph( args, graph, os ); }
    } visitor{ args, os };
    std::visit( visitor, container );
}

void renderReport( Args& args, const Container& container, std::ostream& os )
{
    std::ostringstream osContainer;
    renderContainer( args, container, osContainer );
    nlohmann::json report( { { "body", osContainer.str() } } );
    args.inja.renderReport( report, os );
}

} // namespace

Renderer::Renderer( const boost::filesystem::path& templateDir )
    : m_pInja( new Inja( templateDir ) )
{
}
Renderer::~Renderer()
{
    delete reinterpret_cast< Inja* >( m_pInja );
}

void Renderer::render( const Container& report, std::ostream& os )
{
    Args args{ *reinterpret_cast< Inja* >( m_pInja ), nullptr };
    renderReport( args, report, os );
}

void Renderer::render( const Container& report, Reporter& linker, std::ostream& os )
{
    Args args{ *reinterpret_cast< Inja* >( m_pInja ), &linker };
    renderReport( args, report, os );
}

} // namespace mega::reports
