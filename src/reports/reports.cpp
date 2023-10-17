
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

#include "reports/reports.hpp"

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
    ::inja::Environment m_injaEnvironment;

    enum TemplateType
    {
        TOTAL_TEMPLATE_TYPES
    };

    // std::array< std::string, TOTAL_TEMPLATE_TYPES >      m_templateNames;
    // std::array< ::inja::Template, TOTAL_TEMPLATE_TYPES > m_templates;
    //
    // void renderTemplate( const nlohmann::json& data, TemplateType templateType, std::ostream& os )
    // {
    //     try
    //     {
    //         m_injaEnvironment.render_to( os, m_templates[ templateType ], data );
    //     }
    //     catch( ::inja::RenderError& ex )
    //     {
    //         THROW_RTE( "Inja Exception rendering template: " << m_templateNames[ templateType ]
    //                                                         << " error: " << ex.what() );
    //     }
    //     catch( std::exception& ex )
    //     {
    //         THROW_RTE( "Exception rendering template: " << m_templateNames[ templateType ] << " error: " << ex.what()
    //         );
    //     }
    // }

public:
    using Ptr = std::unique_ptr< Inja >;

    Inja() { m_injaEnvironment.set_trim_blocks( true ); }

    // Inja( const mega::MegastructureInstallation& megaInstall, const mega::Project& project )
    //     : m_templateNames{ "allocator.jinja", "relation.jinja", "program.jinja", "invocation.jinja" }
    //{
    //     m_injaEnvironment.set_trim_blocks( true );
    //     const auto runtimeTemplatesDir = megaInstall.getRuntimeTemplateDir();
    //     for( int i = 0; i != TOTAL_TEMPLATE_TYPES; ++i )
    //     {
    //         const auto templateType = static_cast< TemplateType >( i );
    //         auto       templatePath = runtimeTemplatesDir / m_templateNames[ templateType ];
    //         VERIFY_RTE_MSG( boost::filesystem::exists( templatePath ),
    //                         "Failed to locate runtime template: " << templatePath.string() );
    //         m_templates[ templateType ] = m_injaEnvironment.parse_template( templatePath.string() );
    //     }
    // }

    // void render_invocation( const nlohmann::json& data, std::ostream& os ) { renderTemplate( data, eInvocation, os );
    // } void render_allocator( const nlohmann::json& data, std::ostream& os ) { renderTemplate( data, eAllocator, os );
    // } void render_relation( const nlohmann::json& data, std::ostream& os ) { renderTemplate( data, eRelation, os ); }
    // void render_program( const nlohmann::json& data, std::ostream& os ) { renderTemplate( data, eProgram, os ); }

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

void render_Element( Inja& inja, Element::Ptr pElement, std::ostream& os );

void render_Value( Inja& inja, Property::Ptr pProperty, std::ostream& os )
{
    using ::operator<<;

    struct Visitor
    {
        std::ostream& os;

        void operator()( const std::string& value ) const { os << value; }
        void operator()( const mega::Any& value ) const { os << value; }
        void operator()( const mega::TypeID& value ) const { os << value; }
        void operator()( const mega::MP& value ) const { os << value; }
        void operator()( const mega::MPO& value ) const { os << value; }
        void operator()( const mega::SubTypeInstance& value ) const { os << value; }
        void operator()( const mega::TypeInstance& value ) const { os << value; }
        void operator()( const mega::InvocationID& value ) const { os << value; }
        void operator()( const mega::BitSet& value ) const { os << value; }
        void operator()( const mega::reference& value ) const { os << value; }
        void operator()( const mega::ReferenceVector& value ) const { os << value; }
        void operator()( const mega::LinkTypeVector& value ) const { os << value; }
        void operator()( const mega::SizeAlignment& value ) const { os << value.size << "," << value.alignment; }
        void operator()( const mega::OperationID& value ) const { os << value; }
        void operator()( const mega::ExplicitOperationID& value ) const { os << value; }
        void operator()( const mega::RelationID& value ) const { os << value; }

    } visitor{ os };
    std::visit( visitor, pProperty->m_value );
}

void render_IdentifierValue( Inja& inja, Property::Ptr pProperty, std::ostream& os )
{
    os << pProperty->m_identifier << ": ";
    render_Value( inja, pProperty, os );
}

void render_Values( Inja& inja, Element::Ptr pElement, std::ostream& os )
{
    nlohmann::json data( {

        { "values", nlohmann::json::array() }

    } );

    bool bFirst = true;
    for( auto pProp : pElement->m_properties )
    {
        if( bFirst )
        {
            bFirst = false;
        }
        else
        {
            os << " ";
        }
        std::ostringstream osValue;
        render_IdentifierValue( inja, pProp, osValue );
        data[ "values" ].push_back( osValue.str() );
    }

    static const char* szTemplate =
        R"TEMPLATE(
{% for value in values %}
<p>{{ value }}</p>
{% endfor %}
)TEMPLATE";

    os << inja.render( szTemplate, data );
}

void render_Table( Inja& inja, Table::Ptr pElement, std::ostream& os )
{
    nlohmann::json data( {

        { "headers", nlohmann::json::array() }, { "rows", nlohmann::json::array() }

    } );

    int iColumnCount = 0;
    for( auto pRow : pElement->m_rows )
    {
        int iCol = 0;

        nlohmann::json row( { { "columns", nlohmann::json::array() } } );

        for( auto p : pRow )
        {
            {
                std::ostringstream osElement;
                render_Element( inja, p, osElement );
                row[ "columns" ].push_back( osElement.str() );
            }

            if( iColumnCount <= iCol )
            {
                if( !p->m_properties.empty() )
                {
                    auto pFirst = p->m_properties.front();
                    data[ "headers" ].push_back( pFirst->m_identifier );
                }
                else
                {
                    data[ "headers" ].push_back( "" );
                }
                ++iColumnCount;
            }
            ++iCol;
        }

        data[ "rows" ].push_back( row );
    }

    static const char* szTemplate =
        R"TEMPLATE(
<table style="width:100%">
<tr>
{% for header in headers %}
<th>{{ header }}</th>
{% endfor %}
</tr>

{% for row in rows %}
<tr>
{% for col in row.columns %}
<td>
{{ col }}
</td>
{% endfor %}
</tr>
{% endfor %}
</table>
)TEMPLATE";

    os << inja.render( szTemplate, data );
}

void render_TreeNode( Inja& inja, TreeNode::Ptr pElement, std::ostream& os )
{
    nlohmann::json data( {

        { "name", "" }, { "rows", nlohmann::json::array() }

    } );
    {
        std::ostringstream osName;
        render_Values( inja, pElement, osName );
        data[ "name" ] = osName.str();
    }

    for( auto pChild : pElement->m_children )
    {
        std::ostringstream osChild;
        render_Element( inja, pChild, osChild );
        data[ "rows" ].push_back( osChild.str() );
    }

    static const char* szTemplate =
        R"TEMPLATE(
{{ name }}
<ol>
{% for row in rows %}
<li>{{ row }}</li>
{% endfor %}
</ol>
)TEMPLATE";

    os << inja.render( szTemplate, data );
}

std::string graphVizEscape( const std::string& str )
{
    std::ostringstream os;
    for( auto c : str )
    {
        switch( c )
        {
            case '<':
                os << "\\<";
                break;
            case '>':
                os << "\\>";
                break;
            case '|':
                os << "\\|";
                break;
            case ':':
                os << "\\:";
                break;
            default:
                os << c;
                break;
        }
    }
    return os.str();
}

void render_SVG( Inja& inja, SVG::Ptr pSVG, std::ostream& os )
{
    nlohmann::json data
        = nlohmann::json::object( { { "nodes", nlohmann::json::array() }, { "edges", nlohmann::json::array() } } );

    std::map< Element::Ptr, int > elementIDs;
    for( auto p : pSVG->m_children )
    {
        elementIDs.insert( { p, elementIDs.size() } );
    }

    for( auto pElement : pSVG->m_children )
    {
        nlohmann::json node = nlohmann::json::object(
            { { "name", elementIDs[ pElement ] }, { "label", "" }, { "properties", nlohmann::json::array() } } );

        bool bFirst = true;
        for( auto pProp : pElement->m_properties )
        {
            if( bFirst )
            {
                bFirst = false;
                std::ostringstream osLabel;
                render_IdentifierValue( inja, pProp, osLabel );
                node[ "label" ] = osLabel.str();
            }
            else
            {
                nlohmann::json     property = nlohmann::json::object( { { "name", "" }, { "value", "" } } );
                std::ostringstream osProperty;
                render_Value( inja, pProp, osProperty );
                property[ "name" ]  = pProp->m_identifier;
                property[ "value" ] = graphVizEscape( osProperty.str() );
                node[ "properties" ].push_back( property );
            }
        }

        data[ "nodes" ].push_back( node );
    }

    for( auto pEdge : pSVG->m_edges )
    {
        nlohmann::json edge = nlohmann::json::object( { { "from", elementIDs[ pEdge->m_source ] },
                                                        { "to", elementIDs[ pEdge->m_target ] },
                                                        { "properties", nlohmann::json::array() } } );

        for( auto pProp : pEdge->m_properties )
        {
            nlohmann::json     property = nlohmann::json::object( { { "name", "" }, { "value", "" } } );
            std::ostringstream osProperty;
            render_Value( inja, pProp, osProperty );
            property[ "name" ]  = pProp->m_identifier;
            property[ "value" ] = graphVizEscape( osProperty.str() );
            edge[ "properties" ].push_back( property );
        }

        data[ "edges" ].push_back( edge );
    }

    static const char* szTemplate =
        R"TEMPLATE(
digraph
{
    fontname="Helvetica,Arial,sans-serif"
    rankdir=LR;
    node [ shape=record style=filled fillcolor=gray color=darkblue];
    edge [ ];
    graph [splines=true overlap=false]
    ratio = auto;
    concentrate=True;

{% for node in nodes %}
    "{{ node.name }}" [
        label = "{{ node.label }}{% for property in node.properties %}|{{ property.name }}: {{ property.value }}{% endfor %}"
        shape = "record"
    ];
{% endfor %}

{% for edge in edges %}
    {{ edge.from }} -> {{ edge.to }} {% for property in edge.properties %}[{{ property.name }}="#{{ property.value }}"]{% endfor %}

{% endfor %}
}
)TEMPLATE";

    std::ostringstream osDot;
    osDot << inja.render( szTemplate, data );

    boost::filesystem::path testFolder = boost::filesystem::temp_directory_path() / common::uuid();
    boost::filesystem::create_directories( testFolder );
    VERIFY_RTE_MSG(
        boost::filesystem::exists( testFolder ), "Failed to create temporary folder: " << testFolder.string() );

    boost::filesystem::path tempDotFile = testFolder / "temp.dot";
    boost::filesystem::path tempSVGFile = testFolder / "temp.svg";

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

    boost::filesystem::loadAsciiFile( tempSVGFile, os );

    boost::filesystem::remove_all( testFolder );
}

void render_Element( Inja& inja, Element::Ptr pElement, std::ostream& os )
{
    using namespace mega::reports;
    if( auto pTable = std::dynamic_pointer_cast< Table >( pElement ) )
    {
        render_Table( inja, pTable, os );
    }
    else if( auto pTreeNode = std::dynamic_pointer_cast< TreeNode >( pElement ) )
    {
        render_TreeNode( inja, pTreeNode, os );
    }
    else if( auto pSVG = std::dynamic_pointer_cast< SVG >( pElement ) )
    {
        render_SVG( inja, pSVG, os );
    }
    else
    {
        render_Values( inja, pElement, os );
    }
}

void render_body( Inja& inja, Element::Ptr pElement, std::ostream& os )
{
    std::ostringstream osElement;
    render_Element( inja, pElement, osElement );
    os << osElement.str();
}

void render_html( Inja& inja, Element::Ptr pElement, std::ostream& os )
{
    std::ostringstream osBody;
    render_body( inja, pElement, osBody );

    static const char* szTemplate =
        R"TEMPLATE(
<html>

<head>
<style>
th, td {
  border-style:solid;
  border-color: #96D4D4;
}
</st
</style>
</head>

<body>
{{ body }}
</body>
</html>
)TEMPLATE";

    nlohmann::json templateData( { { "body", osBody.str() }

    } );

    os << inja.render( szTemplate, templateData );
}
} // namespace

void renderHTML( Element::Ptr pElement, std::ostream& os )
{
    Inja inja;
    render_html( inja, pElement, os );
}
} // namespace mega::reports
