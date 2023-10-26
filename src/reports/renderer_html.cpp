
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

#include "reports/renderer_html.hpp"
#include "reports/report.hpp"

#include "mega/values/compilation/invocation_id.hpp"
#include "mega/values/compilation/relation_id.hpp"
#include "mega/values/compilation/operator_id.hpp"
#include "mega/values/compilation/type_id.hpp"

#include "mega/values/runtime/any.hpp"
#include "mega/values/runtime/reference.hpp"

#include "mega/values/native_types_io.hpp"

#include "common/process.hpp"
#include "common/file.hpp"
#include "common/string.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include "boost/filesystem.hpp"
#include <boost/algorithm/string.hpp>

#include <algorithm>

namespace mega::reports
{
namespace
{

class Inja
{
    ::inja::Environment     m_injaEnvironment;
    boost::filesystem::path m_tempFolder;
    bool                    m_bClearTempFiles;

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
    Inja( const boost::filesystem::path& templateDir, bool bClearTempFiles )
        : m_templateNames{ "report.jinja", "multiline.jinja", "branch.jinja", "table.jinja", "graph.jinja" }
        , m_tempFolder( boost::filesystem::temp_directory_path() / "graphs" / common::uuid() )
        , m_bClearTempFiles( bClearTempFiles )
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
        if( m_bClearTempFiles )
        {
            boost::filesystem::remove_all( m_tempFolder );
        }
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

        // deal with annoying graphviz behaviour with how id is generated where it doesnt work as bookmark

        std::string str;
        {
            boost::filesystem::loadAsciiFile( tempSVGFile, str );

            static const std::string strSearch  = "<g id=\"a_";
            static const std::string strReplace = "<text ";

            using Iter = std::string::iterator;

            for( Iter i = str.begin(), iEnd = str.end(); i != iEnd;
                 i = std::search( i, iEnd, strSearch.begin(), strSearch.end() ) )
            {
                Iter r = std::search( i, iEnd, strReplace.begin(), strReplace.end() );
                if( r != iEnd )
                {
                    Iter idStart    = i + 2; // NOTE: include the space before the id=
                    int  quoteCount = 0;

                    // find the end of the id="something" by counting the quotes
                    Iter idEnd = idStart;
                    for( ; ( idEnd != iEnd ) && ( quoteCount != 2 ); )
                    {
                        if( *idEnd == '\"' )
                        {
                            ++quoteCount;
                        }
                        ++idEnd;
                    }

                    // want to move the id string to 5 chars after r ( ignore the space )
                    Iter newID = r + 6;

                    // move ( idStart to idEnd ) to newID
                    std::string strTemp( idStart, idEnd );
                    boost::replace_all( strTemp, "\"a_", "  \"" );

                    auto iNewIDStart = std::copy( idEnd, newID, idStart );
                    std::copy( strTemp.begin(), strTemp.end(), iNewIDStart );
                }
            }
        }
        os << str;
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

std::string escapeHTML( std::string data )
{
    using boost::algorithm::replace_all;
    replace_all( data, "&", "&amp;" );
    replace_all( data, "\"", "&quot;" );
    replace_all( data, "\'", "&apos;" );
    replace_all( data, "<", "&lt;" );
    replace_all( data, ">", "&gt;" );
    return data;
}

int javascriptKeyCode( char c )
{
    // clang-format off
    switch( c )
    {
        case 'A': return 65;
        case 'B': return 66;
        case 'C': return 67;
        case 'D': return 68;
        case 'E': return 69;
        case 'F': return 70;
        case 'G': return 71;
        case 'H': return 72;
        case 'I': return 73;
        case 'J': return 74;
        case 'K': return 75;
        case 'L': return 76;
        case 'M': return 77;
        case 'N': return 78;
        case 'O': return 79;
        case 'P': return 80;
        case 'Q': return 81;
        case 'R': return 82;
        case 'S': return 83;
        case 'T': return 84;
        case 'U': return 85;
        case 'V': return 86;
        case 'W': return 87;
        case 'X': return 88;
        case 'Y': return 89;
        case 'Z': return 90;
        
        case 'b': return 98;
        case 'c': return 99;
        case 'd': return 100;
        case 'e': return 101;
        case 'f': return 102;
        case 'g': return 103;
        case 'h': return 104;
        case 'i': return 105;
        case 'j': return 106;
        case 'k': return 107;
        case 'l': return 108;
        case 'm': return 109;
        case 'n': return 110;
        case 'o': return 111;
        case 'p': return 112;
        case 'q': return 113;
        case 'r': return 114;
        case 's': return 115;
        case 't': return 116;
        case 'u': return 117;
        case 'v': return 118;
        case 'w': return 119;
        case 'x': return 120;
        case 'y': return 121;
        case 'z': return 122;
    default:
        THROW_RTE( "Unknown javascript keycode: " << c );
    }
    // clang-format on
}

struct Args
{
    Inja&   inja;
    Linker* pLinker = nullptr;
};

std::string javascriptHREF( const URL& url )
{
    std::ostringstream os;

    os << "javascript:navigateTo( &quot;" << url.encoded_path() << "&quot;,";

    bool bHasReportType = false;
    {
        auto iFind = url.params().find( "report" );
        if( iFind != url.params().end() )
        {
            bHasReportType = true;
        }
    }

    if( ( !url.params().empty() ) && ( !bHasReportType || ( url.params().size() > 1 ) ) )
    {
        if( bHasReportType )
        {
            // remove the report type
            URL temp = url;
            temp.params().erase( "report" );
            os << " &quot;" << temp.encoded_params() << "&quot; ,";
        }
        else
        {
            os << " &quot;" << url.encoded_params() << "&quot; ,";
        }
    }
    else
    {
        os << " &quot;&quot; ,";
    }

    if( url.has_fragment() )
    {
        os << " &quot;" << url.encoded_fragment() << "&quot; )";
    }
    else
    {
        os << " &quot;&quot; )";
    }

    return os.str();
}

void valueToJSON( Args& args, const Value& value, nlohmann::json& data )
{
    std::optional< URL > urlOpt;
    if( args.pLinker )
    {
        urlOpt = args.pLinker->link( value );
    }

    std::ostringstream os;
    if( urlOpt.has_value() )
    {
        os << "<a href=\"" << javascriptHREF( urlOpt.value() ) << "\">" << escapeHTML( toString( value ) ) << "</a>";
    }
    else
    {
        os << escapeHTML( toString( value ) );
    }

    data.push_back( os.str() );
}

void graphValueToJSON( Args& args, const Value& value, const std::optional< Value >& bookmarkOpt, nlohmann::json& data )
{
    std::ostringstream os;

    os << "<td";

    std::optional< URL > urlOpt;
    if( args.pLinker )
    {
        urlOpt = args.pLinker->link( value );
    }

    if( bookmarkOpt.has_value() )
    {
        os << " ID=\"" << escapeHTML( toString( bookmarkOpt.value() ) ) << "\"";
        if( !urlOpt.has_value() )
        {
            // ensure href because graphviz will NOT generate ID if no href present in <td>
            URL url;
            url.set_fragment( toString( bookmarkOpt.value() ) );
            os << " href=\"" << javascriptHREF( url ) << "\"";
        }
    }

    if( urlOpt.has_value() )
    {
        os << " href=\"" << javascriptHREF( urlOpt.value() ) << "\"><U>" << escapeHTML( toString( value ) )
           << "</U></td>";
    }
    else
    {
        os << ">" << escapeHTML( toString( value ) ) << "</td>";
    }

    data.push_back( os.str() );
}

void valueVectorToJSON( Args& args, const ValueVector& textVector, nlohmann::json& data )
{
    for( const auto& text : textVector )
    {
        valueToJSON( args, text, data );
    }
}

template < typename T >
void addOptionalBookmark( Args& args, T& element, nlohmann::json& data )
{
    if( element.m_bookmark.has_value() )
    {
        data[ "has_bookmark" ] = true;
        data[ "bookmark" ]     = escapeHTML( toString( element.m_bookmark.value() ) );
    }
}

template < typename T >
bool addOptionalLink( Args& args, T& element, nlohmann::json& data )
{
    if( element.m_url.has_value() )
    {
        data[ "has_link" ] = true;
        data[ "link" ]     = javascriptHREF( element.m_url.value() );
        return true;
    }
    return false;
}

void renderLine( Args& args, const Line& line, std::ostream& os )
{
    nlohmann::json data( { { "style", "multiline_default" },
                           { "elements", nlohmann::json::array() },
                           { "has_bookmark", false },
                           { "has_link", false },
                           { "bookmark", "" } } );
    addOptionalBookmark( args, line, data );
    if( addOptionalLink( args, line, data ) )
    {
        Args noLinkerArgs{ args.inja, nullptr };
        valueToJSON( noLinkerArgs, line.m_element, data[ "elements" ] );
    }
    else
    {
        valueToJSON( args, line.m_element, data[ "elements" ] );
    }
    args.inja.renderMultiLine( data, os );
}

void renderMultiline( Args& args, const Multiline& multiline, std::ostream& os )
{
    nlohmann::json data( { { "style", "multiline_default" },
                           { "elements", nlohmann::json::array() },
                           { "has_bookmark", false },
                           { "has_link", false },
                           { "bookmark", "" } } );
    addOptionalBookmark( args, multiline, data );
    if( addOptionalLink( args, multiline, data ) )
    {
        Args noLinkerArgs{ args.inja, nullptr };
        valueVectorToJSON( noLinkerArgs, multiline.m_elements, data[ "elements" ] );
    }
    else
    {
        valueVectorToJSON( args, multiline.m_elements, data[ "elements" ] );
    }

    args.inja.renderMultiLine( data, os );
}

void renderContainer( Args& args, const Container& container, std::ostream& os );

void renderBranch( Args& args, const Branch& branch, std::ostream& os )
{
    nlohmann::json data( { { "style", "branch_default" },
                           { "has_bookmark", false },
                           { "bookmark", "" },
                           { "label", nlohmann::json::array() },
                           { "elements", nlohmann::json::array() } } );

    addOptionalBookmark( args, branch, data );
    valueVectorToJSON( args, branch.m_label, data[ "label" ] );

    for( const auto& pChildElement : branch.m_elements )
    {
        std::ostringstream osChild;
        renderContainer( args, pChildElement, osChild );
        data[ "elements" ].push_back( osChild.str() );
    }

    args.inja.renderBranch( data, os );
}

void renderTable( Args& args, const Table& table, std::ostream& os )
{
    nlohmann::json data( { { "headings", nlohmann::json::array() }, { "rows", nlohmann::json::array() } } );

    if( !table.m_headings.empty() )
    {
        valueVectorToJSON( args, table.m_headings, data[ "headings" ] );
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

    nlohmann::json data( { { "rank_direction", graph.m_rankDirection.str() },
                           { "nodes", nlohmann::json::array() },
                           { "edges", nlohmann::json::array() },
                           { "subgraphs", nlohmann::json::array() } } );

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
            { "has_url", false }

        } );

        if( node.m_url.has_value() )
        {
            nodeData[ "has_url" ] = true;
            nodeData[ "url" ]     = javascriptHREF( node.m_url.value() );
        }

        // addOptionalBookmark( args, node, nodeData );

        bool bFirst = true;
        for( const ValueVector& row : node.m_rows )
        {
            nlohmann::json rowData( { { "values", nlohmann::json::array() } } );
            for( const Value& value : row )
            {
                // NOTE graph value generates <td id="bookmark">value</td> so can generate href in graphviz
                if( bFirst )
                {
                    bFirst = false;
                    graphValueToJSON( args, value, node.m_bookmark, rowData[ "values" ] );
                }
                else
                {
                    graphValueToJSON( args, value, std::nullopt, rowData[ "values" ] );
                }
            }
            nodeData[ "rows" ].push_back( rowData );
        }

        data[ "nodes" ].push_back( nodeData );
    }

    std::vector< std::string > subgraphNames;
    for( const auto& subgraph : graph.m_subgraphs )
    {
        std::ostringstream osNodeName;
        // NOTE: MUST start with 'cluster'
        osNodeName << "cluster_" << subgraphNames.size();
        subgraphNames.push_back( osNodeName.str() );

        nlohmann::json subgraphData( {

            { "name", osNodeName.str() },
            { "colour", subgraph.m_colour.str() },
            { "rows", nlohmann::json::array() },
            { "nodes", nlohmann::json::array() },
            { "has_url", false },
            { "has_label", false }

        } );

        if( subgraph.m_url.has_value() )
        {
            subgraphData[ "has_url" ] = true;
            subgraphData[ "url" ]     = javascriptHREF( subgraph.m_url.value() );
        }

        VERIFY_RTE_MSG( !subgraph.m_bookmark.has_value(), "Subgraph bookmark deprecated" );
        // addOptionalBookmark( args, subgraph, subgraphData );

        for( const ValueVector& row : subgraph.m_rows )
        {
            nlohmann::json rowData( { { "values", nlohmann::json::array() } } );
            for( const Value& value : row )
            {
                // NOTE graph value generates <td>value</td> so can generate href in graphviz
                graphValueToJSON( args, value, std::nullopt, rowData[ "values" ] );
                subgraphData[ "has_label" ] = true;
            }
            subgraphData[ "rows" ].push_back( rowData );
        }

        for( auto iNode : subgraph.m_nodes )
        {
            VERIFY_RTE_MSG( ( iNode ) >= 0 && ( iNode < nodeNames.size() ), "Invalid subgraph node id of: " << iNode );
            subgraphData[ "nodes" ].push_back( nodeNames[ iNode ] );
        }

        data[ "subgraphs" ].push_back( subgraphData );
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
            { "style", edge.m_style.str() },
            { "constraint", edge.m_bIgnoreInLayout ? "false" : "true" }

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
        using result_type = void;

        Args&         args;
        std::ostream& os;

        void operator()( const Line& line ) const { renderLine( args, line, os ); }
        void operator()( const Multiline& multiline ) const { renderMultiline( args, multiline, os ); }
        void operator()( const Branch& branch ) const { renderBranch( args, branch, os ); }
        void operator()( const Table& table ) const { renderTable( args, table, os ); }
        void operator()( const Graph& graph ) const { renderGraph( args, graph, os ); }
    } visitor{ args, os };

    boost::apply_visitor( visitor, container );
}

void renderReport( Args&                                    args,
                   const Container&                         container,
                   const HTMLRenderer::JavascriptShortcuts& shortcuts,
                   std::ostream&                            os )
{
    std::ostringstream osContainer;
    renderContainer( args, container, osContainer );
    nlohmann::json report( { { "body", osContainer.str() }, { "reports", nlohmann::json::array() } } );

    for( const auto& reporterID : shortcuts.get() )
    {
        std::string strChar;
        strChar.push_back( reporterID.key );
        nlohmann::json reporterIDData( { { "key_char", strChar },
                                         { "key_code", javascriptKeyCode( reporterID.key ) },
                                         { "name", reporterID.strAction } } );
        report[ "reports" ].push_back( reporterIDData );
    }

    args.inja.renderReport( report, os );
}

} // namespace

HTMLRenderer::HTMLRenderer( const boost::filesystem::path& templateDir,
                            JavascriptShortcuts            shortcuts,
                            bool                           bClearTempFiles )
    : m_pInja( new Inja( templateDir, bClearTempFiles ) )
    , m_shortcuts( std::move( shortcuts ) )
{
}
HTMLRenderer::~HTMLRenderer()
{
    delete reinterpret_cast< Inja* >( m_pInja );
}

void HTMLRenderer::render( const Container& report, std::ostream& os )
{
    Args args{ *reinterpret_cast< Inja* >( m_pInja ), nullptr };
    renderReport( args, report, m_shortcuts, os );
}

void HTMLRenderer::render( const Container& report, Linker& linker, std::ostream& os )
{
    Args args{ *reinterpret_cast< Inja* >( m_pInja ), &linker };
    renderReport( args, report, m_shortcuts, os );
}

} // namespace mega::reports
