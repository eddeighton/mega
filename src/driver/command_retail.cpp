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

#include "database/common/environment_archive.hpp"
#include "database/common/archive.hpp"

#include "database/model/FinalStage.hxx"

#include "utilities/clang_format.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

namespace driver::retail
{
namespace
{
class TemplateEngine
{
    ::inja::Environment& m_injaEnvironment;
    ::inja::Template     m_retailCodeTemplate;
    ::inja::Template     m_contextCodeTemplate;
    ::inja::Template     m_invocationCodeTemplate;

public:
    TemplateEngine( ::inja::Environment&           injaEnv,
                    const boost::filesystem::path& retailCodeTemplatePath,
                    const boost::filesystem::path& contextCodeTemplatePath,
                    const boost::filesystem::path& invocationCodeTemplatePath )
        : m_injaEnvironment( injaEnv )
        , m_retailCodeTemplate( m_injaEnvironment.parse_template( retailCodeTemplatePath.string() ) )
        , m_contextCodeTemplate( m_injaEnvironment.parse_template( contextCodeTemplatePath.string() ) )
        , m_invocationCodeTemplate( m_injaEnvironment.parse_template( invocationCodeTemplatePath.string() ) )
    {
    }
    void renderInvocation( const nlohmann::json& data, std::ostream& os ) const
    {
        m_injaEnvironment.render_to( os, m_invocationCodeTemplate, data );
    }
    void renderContext( const nlohmann::json& data, std::ostream& os ) const
    {
        m_injaEnvironment.render_to( os, m_contextCodeTemplate, data );
    }
    void renderRetail( const nlohmann::json& data, std::ostream& os ) const
    {
        m_injaEnvironment.render_to( os, m_retailCodeTemplate, data );
    }
};

inline std::string toHex( mega::TypeID typeID )
{
    std::ostringstream os;
    os << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << static_cast< mega::U32 >( typeID.getSymbolID() );
    return os.str();
}

struct CleverUtility
{
    using IDList = std::vector< std::string >;
    IDList& theList;
    CleverUtility( IDList& theList, const std::string& strID )
        : theList( theList )
    {
        theList.push_back( strID );
    }
    ~CleverUtility() { theList.pop_back(); }
};

struct InvocationInfo
{
    using IContextMap
        = std::multimap< const FinalStage::Interface::IContext*, const FinalStage::Operations::Invocation* >;
    using DimensionMap
        = std::multimap< const FinalStage::Interface::DimensionTrait*, const FinalStage::Operations::Invocation* >;
    IContextMap  contextInvocations;
    DimensionMap dimensionInvocations;
};

struct InvocationTree
{
    struct Node
    {
        using OperationsMap = std::map< mega::ExplicitOperationID, const FinalStage::Operations::Invocation* >;
        using ChildNodeMap  = std::map< const FinalStage::Symbols::SymbolTypeID*, Node >;

        const FinalStage::Symbols::SymbolTypeID* pSymbol = nullptr;

        ChildNodeMap  m_children;
        OperationsMap m_operations;
    };
    Node m_root;

    FinalStage::Symbols::SymbolTable*                          m_pSymbolTable;
    const decltype( m_pSymbolTable->get_symbol_type_ids() )    m_symbols;
    const decltype( m_pSymbolTable->get_interface_type_ids() ) m_interface;

    InvocationTree( FinalStage::Symbols::SymbolTable* pSymbolTable )
        : m_pSymbolTable( pSymbolTable )
        , m_symbols( m_pSymbolTable->get_symbol_type_ids() )
        , m_interface( m_pSymbolTable->get_interface_type_ids() )
    {
    }

    void add( const FinalStage::Operations::Invocation* pInvocation )
    {
        using namespace FinalStage;

        Node* pNode = &m_root;

        const mega::InvocationID& invocationID = pInvocation->get_id();

        for( mega::TypeID symbol : invocationID.m_type_path )
        {
            if( !symbol.isSymbolID() )
            {
                auto iFind = m_interface.find( symbol );
                VERIFY_RTE( iFind != m_interface.end() );
                symbol = iFind->second->get_symbol_ids().back();
            }

            if( !mega::isOperationType( symbol ) )
            {
                Symbols::SymbolTypeID* pSymbol = nullptr;
                {
                    auto iFind = m_symbols.find( symbol );
                    VERIFY_RTE( iFind != m_symbols.end() );
                    pSymbol = iFind->second;
                }

                auto iFind = pNode->m_children.find( pSymbol );
                if( iFind != pNode->m_children.end() )
                {
                    pNode = &iFind->second;
                }
                else
                {
                    auto ib = pNode->m_children.insert( { pSymbol, Node{ pSymbol } } );
                    VERIFY_RTE( ib.second );
                    pNode = &ib.first->second;
                }
            }
        }
        auto ib = pNode->m_operations.insert( { pInvocation->get_explicit_operation(), pInvocation } );
        VERIFY_RTE_MSG( ib.second, "Duplicate invocation found" );
    }
};

void recurseInvocations( TemplateEngine& templateEngine, CleverUtility::IDList& namespaces,
                         CleverUtility::IDList& types, const InvocationTree::Node& node, std::ostream& os,
                         nlohmann::json& interfaceOperations )
{
    std::ostringstream osTypeName;
    osTypeName << "_" << node.pSymbol->get_symbol();

    CleverUtility c( types, osTypeName.str() );

    std::ostringstream osNested;
    for( const auto& [ pSymbol, childNode ] : node.m_children )
    {
        recurseInvocations( templateEngine, namespaces, types, childNode, osNested, interfaceOperations );
    }

    nlohmann::json invocation( {

        { "type", osTypeName.str() },
        { "symbol", node.pSymbol->get_symbol() },
        { "nested", osNested.str() },
        { "operations", nlohmann::json::array() }

    } );

    for( const auto& [ opType, pInvocation ] : node.m_operations )
    {
        std::ostringstream osType;
        osType << "_" << mega::getExplicitOperationString( opType );

        std::unique_ptr< CleverUtility > pOptionalNesting;
        {
            if( !mega::isOperationImplicit( pInvocation->get_operation() ) )
            {
                pOptionalNesting = std::make_unique< CleverUtility >( types, osType.str() );
            }
        }

        nlohmann::json operation( {

            { "automata", false },
            { "implicit", mega::isOperationImplicit( pInvocation->get_operation() ) },
            { "has_args", mega::isOperationArgs( pInvocation->get_operation() ) },
            { "type", osType.str() },
            { "has_namespaces", !namespaces.empty() },
            { "namespaces", namespaces },
            { "types", types },
            { "symbol", mega::getExplicitOperationString( opType ) },
            { "return_type", pInvocation->get_return_type_str() },
            { "params_string", "" },
            { "body", "" }

        } );

        switch( opType )
        {
            case mega::id_exp_Read:
            {
            }
            break;
            case mega::id_exp_Write:
            {
                std::ostringstream osParams;
                // osParams << "const " << pInvocation->get_runtime_return_type_str() << "& arg";
                osParams << "Args... args";
                operation[ "params_string" ] = osParams.str();
            }
            break;
            case mega::id_exp_Read_Link:
            {
            }
            break;
            case mega::id_exp_Write_Link:
            {
            }
            break;
            case mega::id_exp_Allocate:
            {
            }
            break;
            case mega::id_exp_Call:
            {
            }
            break;
            case mega::id_exp_Start:
            {
            }
            break;
            case mega::id_exp_Stop:
            {
            }
            break;
            case mega::id_exp_Save:
            {
            }
            break;
            case mega::id_exp_Load:
            {
            }
            break;
            case mega::id_exp_Files:
            {
            }
            break;
            case mega::id_exp_GetAction:
            {
            }
            break;
            case mega::id_exp_GetDimension:
            {
            }
            break;
            case mega::id_exp_Done:
            {
            }
            break;
            case mega::id_exp_Range:
            {
            }
            break;
            case mega::id_exp_Raw:
            {
            }
            break;
            default:
            case mega::HIGHEST_EXPLICIT_OPERATION_TYPE:
            {
                THROW_RTE( "Unknown invocation type" );
            }
            break;
        }
        invocation[ "operations" ].push_back( operation );
        interfaceOperations.push_back( operation );
    }

    templateEngine.renderInvocation( invocation, os );
}

template < typename TContextType >
static std::vector< nlohmann::json >
getDimensionTraits( CleverUtility::IDList& typenames, TContextType* pContext,
                    const std::vector< FinalStage::Interface::DimensionTrait* >& dimensionTraits )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;

    std::vector< nlohmann::json > traits;
    for( DimensionTrait* pDimensionTrait : dimensionTraits )
    {
        const std::string& strType = pDimensionTrait->get_type();

        std::ostringstream osName;
        osName << pDimensionTrait->get_id()->get_str();

        nlohmann::json traitNames = typenames;
        traitNames.push_back( osName.str() );

        nlohmann::json trait_struct(
            { { "name", osName.str() }, { "types", traitNames }, { "traits", nlohmann::json::array() } } );

        {
            {
                std::ostringstream osTrait;
                osTrait << "using Type = " << strType;
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }
            {
                std::ostringstream osTrait;
                osTrait << "using Read = mega::DimensionTraits< " << strType << " >::Read";
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }
            {
                std::ostringstream osTrait;
                osTrait << "using Write = mega::DimensionTraits< " << strType << " >::Write";
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }
            {
                std::ostringstream osTrait;
                osTrait << "using Erased = mega::DimensionTraits< " << strType << " >::Erased";
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }
        }
        traits.push_back( trait_struct );
    }

    return traits;
}

void recurseInterface( const InvocationInfo& invocationInfo, FinalStage::Symbols::SymbolTable* pSymbolTable,
                       TemplateEngine& templateEngine, CleverUtility::IDList& namespaces, CleverUtility::IDList& types,
                       FinalStage::Interface::IContext* pContext, std::ostream& os, nlohmann::json& interfaceOperations,
                       nlohmann::json& traitStructs )
{
    using namespace FinalStage;

    CleverUtility c( types, pContext->get_identifier() );

    std::ostringstream osNested;
    for( Interface::IContext* pChildContext : pContext->get_children() )
    {
        recurseInterface( invocationInfo, pSymbolTable, templateEngine, namespaces, types, pChildContext, osNested,
                          interfaceOperations, traitStructs );
    }

    InvocationTree tree( pSymbolTable );
    {
        for( auto i    = invocationInfo.contextInvocations.lower_bound( pContext ),
                  iEnd = invocationInfo.contextInvocations.upper_bound( pContext );
             i != iEnd;
             ++i )
        {
            tree.add( i->second );
        }
    }

    std::ostringstream osInvocations;
    for( const auto& [ pSymbol, childNode ] : tree.m_root.m_children )
    {
        recurseInvocations( templateEngine, namespaces, types, childNode, osInvocations, interfaceOperations );
    }

    nlohmann::json context( {

        { "name", pContext->get_identifier() },
        { "typeid", toHex( pContext->get_interface_id() ) },
        { "has_operation", false },
        { "operation_has_args", false },
        { "invocations", osInvocations.str() },
        { "nested", osNested.str() },
        { "trait_structs", nlohmann::json::array() }

    } );

    if( Interface::Namespace* pNamespace = db_cast< Interface::Namespace >( pContext ) )
    {
        for( const nlohmann::json& trait : getDimensionTraits( types, pNamespace, pNamespace->get_dimension_traits() ) )
        {
            context[ "trait_structs" ].push_back( trait );
            traitStructs.push_back( trait );
        }
    }
    else if( Interface::Abstract* pAbstract = db_cast< Interface::Abstract >( pContext ) )
    {
        for( const nlohmann::json& trait : getDimensionTraits( types, pAbstract, pAbstract->get_dimension_traits() ) )
        {
            context[ "trait_structs" ].push_back( trait );
            traitStructs.push_back( trait );
        }
    }
    else if( Interface::Action* pAction = db_cast< Interface::Action >( pContext ) )
    {
        for( const nlohmann::json& trait : getDimensionTraits( types, pAction, pAction->get_dimension_traits() ) )
        {
            context[ "trait_structs" ].push_back( trait );
            traitStructs.push_back( trait );
        }

        context[ "has_operation" ] = true;

        if( auto pStartState = db_cast< Meta::Automata >( pAction ) )
        {
            context[ "operation_return_type" ] = "mega::ActionCoroutine";
            context[ "operation_parameters" ]  = "mega::U64 _blockID";
            context[ "operation_has_args" ]    = true;
        }
        else
        {
            context[ "operation_return_type" ] = "mega::ActionCoroutine";
            context[ "operation_parameters" ]  = "";
            context[ "operation_has_args" ]    = false;
        }
    }
    else if( Interface::Event* pEvent = db_cast< Interface::Event >( pContext ) )
    {
        for( const nlohmann::json& trait : getDimensionTraits( types, pEvent, pEvent->get_dimension_traits() ) )
        {
            context[ "trait_structs" ].push_back( trait );
            traitStructs.push_back( trait );
        }
    }
    else if( Interface::Function* pFunction = db_cast< Interface::Function >( pContext ) )
    {
        context[ "has_operation" ]         = true;
        context[ "operation_has_args" ]    = true;
        context[ "operation_return_type" ] = pFunction->get_return_type_trait()->get_str();
        context[ "operation_parameters" ]  = pFunction->get_arguments_trait()->get_str();
    }
    else if( Interface::Object* pObject = db_cast< Interface::Object >( pContext ) )
    {
        for( const nlohmann::json& trait : getDimensionTraits( types, pObject, pObject->get_dimension_traits() ) )
        {
            context[ "trait_structs" ].push_back( trait );
            traitStructs.push_back( trait );
        }
    }
    else if( Interface::Link* pLink = db_cast< Interface::Link >( pContext ) )
    {
    }

    templateEngine.renderContext( context, os );
}

template < typename T >
nlohmann::json generateBlock( T* pNode, mega::U64 blockID, std::ostream& osBody )
{
    using namespace FinalStage;
    std::ostringstream os;
    {
        for( auto pNode : pNode->get_nodes() )
        {
            if( auto pLit = db_cast< Automata::Literal >( pNode ) )
            {
                os << pLit->get_value();
                osBody << pLit->get_value();
            }
        }
    }
    return nlohmann::json( { { "id", blockID }, { "body", os.str() } } );
}

void generateAutomataRecurse( FinalStage::Automata::Block* pBlock, nlohmann::json& data, std::ostream& osBody )
{
    using namespace FinalStage;

    data[ "blocks" ].push_back( generateBlock( pBlock, pBlock->get_id(), osBody ) );

    for( auto pNode : pBlock->get_nodes() )
    {
        if( auto pNestedBlock = db_cast< Automata::Block >( pNode ) )
        {
            generateAutomataRecurse( pNestedBlock, data, osBody );
        }
    }
}

void recurseOperations( FinalStage::Interface::IContext* pContext,
                        nlohmann::json&                  data,
                        CleverUtility::IDList&           namespaces,
                        CleverUtility::IDList&           types )
{
    using namespace FinalStage;
    using namespace FinalStage::Interface;

    if( auto pNamespace = db_cast< Namespace >( pContext ) )
    {
        /*if ( pNamespace->get_is_global() )
        {
            CleverUtility c( namespaces, pNamespace->get_identifier() );
            for ( IContext* pNestedContext : pNamespace->get_children() )
            {
                recurseOperations( pNestedContext, data, namespaces, types );
            }
        }
        else*/
        {
            CleverUtility c( types, pNamespace->get_identifier() );
            for( IContext* pNestedContext : pNamespace->get_children() )
            {
                recurseOperations( pNestedContext, data, namespaces, types );
            }
        }
    }
    else if( auto pAbstract = db_cast< Abstract >( pContext ) )
    {
        CleverUtility c( types, pAbstract->get_identifier() );
        for( IContext* pNestedContext : pAbstract->get_children() )
        {
            recurseOperations( pNestedContext, data, namespaces, types );
        }
    }
    else if( Interface::Action* pAction = db_cast< Interface::Action >( pContext ) )
    {
        CleverUtility c( types, pAction->get_identifier() );

        if( auto pStartState = db_cast< Automata::Start >( pAction ) )
        {
            nlohmann::json operation( {

                { "automata", true },
                { "return_type", "mega::ActionCoroutine" },
                { "has_args", true },
                { "body", "" },
                { "has_namespaces", !namespaces.empty() },
                { "namespaces", namespaces },
                { "types", types },
                { "params_string", "mega::U64 _blockID" },
                { "blocks", nlohmann::json::array() }

            } );

            {
                nlohmann::json param( { { "type", "mega::U64" }, { "name", "_blockID" } } );
                operation[ "params" ].push_back( param );
            }

            std::ostringstream osBody;
            generateAutomataRecurse( pStartState->get_sequence(), operation, osBody );
            operation[ "hash" ] = common::Hash{ osBody.str() }.toHexString();

            data[ "operations" ].push_back( operation );
        }
        else
        {
            std::ostringstream osBody;
            for( auto pDef : pAction->get_action_defs() )
            {
                if( !pDef->get_body().empty() )
                {
                    osBody << pDef->get_body();
                    break;
                }
            }

            osBody << "\nco_return mega::done();";

            nlohmann::json operation( {

                { "automata", false },
                { "return_type", "mega::ActionCoroutine" },
                { "has_args", false },
                { "body", osBody.str() },
                { "has_namespaces", !namespaces.empty() },
                { "namespaces", namespaces },
                { "types", types },
                { "params_string", "" }

            } );

            data[ "operations" ].push_back( operation );
        }

        for( IContext* pNestedContext : pAction->get_children() )
        {
            recurseOperations( pNestedContext, data, namespaces, types );
        }
    }
    else if( auto pEvent = db_cast< Event >( pContext ) )
    {
    }
    else if( auto pFunction = db_cast< Function >( pContext ) )
    {
        CleverUtility c( types, pFunction->get_identifier() );

        std::string strBody;
        {
            for( auto pDef : pFunction->get_function_defs() )
            {
                if( !pDef->get_body().empty() )
                {
                    strBody = pDef->get_body();
                    break;
                }
            }
        }

        nlohmann::json operation( {

            { "automata", false },
            { "return_type", pFunction->get_return_type_trait()->get_str() },
            { "has_args", true },
            { "body", strBody },
            { "has_namespaces", !namespaces.empty() },
            { "namespaces", namespaces },
            { "types", types },
            { "params_string", pFunction->get_arguments_trait()->get_str() }

        } );

        data[ "operations" ].push_back( operation );
    }
    else if( auto pObject = db_cast< Object >( pContext ) )
    {
        CleverUtility c( types, pObject->get_identifier() );
        for( IContext* pNestedContext : pObject->get_children() )
        {
            recurseOperations( pNestedContext, data, namespaces, types );
        }
    }
    else if( auto pLink = db_cast< Link >( pContext ) )
    {
        CleverUtility c( types, pLink->get_identifier() );
        for( IContext* pNestedContext : pLink->get_children() )
        {
            recurseOperations( pNestedContext, data, namespaces, types );
        }
    }
    else if( auto pBuffer = db_cast< Buffer >( pContext ) )
    {
    }
    else
    {
        THROW_RTE( "Unknown context type" );
    }
}
} // namespace

void command( bool bHelp, const std::vector< std::string >& args )
{
    boost::filesystem::path inputArchiveFilePath, templateDir, retailSourceDir;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Generate retail archive" );
    {
        // clang-format off
            commandOptions.add_options()
                ( "input",    po::value< boost::filesystem::path >( &inputArchiveFilePath ),        "Compilation Archive" )
                ( "template_dir", po::value< boost::filesystem::path >( &templateDir ),             "Template directory" )
                ( "output",   po::value< boost::filesystem::path >( &retailSourceDir ),             "Retail source directory" )
                ;
        // clang-format on
    }

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
    po::notify( vm );

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        VERIFY_RTE_MSG( boost::filesystem::exists( retailSourceDir ),
                        "Failed to locate retail source directory: " << retailSourceDir.string() );
        VERIFY_RTE_MSG( boost::filesystem::exists( inputArchiveFilePath ),
                        "Failed to locate input archive: " << inputArchiveFilePath.string() );

        VERIFY_RTE_MSG(
            boost::filesystem::exists( templateDir ), "Failed to locate template directory: " << templateDir.string() );
        const boost::filesystem::path retailCodeTemplateFilePath = templateDir / "retail.jinja";
        const boost::filesystem::path contextTemplateFilePath    = templateDir / "retail_context.jinja";
        const boost::filesystem::path invocationTemplateFilePath = templateDir / "retail_invocation.jinja";

        VERIFY_RTE_MSG( boost::filesystem::exists( retailCodeTemplateFilePath ),
                        "Failed to locate retail code template file: " << retailCodeTemplateFilePath.string() );
        VERIFY_RTE_MSG( boost::filesystem::exists( contextTemplateFilePath ),
                        "Failed to locate retail code template file: " << contextTemplateFilePath.string() );

        std::string strCode;
        {
            ::inja::Environment injaEnvironment;
            {
                injaEnvironment.set_trim_blocks( true );
            }

            TemplateEngine templateEngine(
                injaEnvironment, retailCodeTemplateFilePath, contextTemplateFilePath, invocationTemplateFilePath );

            mega::io::ArchiveEnvironment environment( inputArchiveFilePath );
            const mega::io::Manifest     manifest( environment, environment.project_manifest() );
            using namespace FinalStage;
            Database database( environment );
            std::cout << "Loaded database: " << environment.getVersion() << std::endl;

            InvocationInfo invocationInfo;

            for( const auto& megaSrcFile : manifest.getMegaSourceFiles() )
            {
                Operations::Invocations* pInvocations = database.one< Operations::Invocations >( megaSrcFile );

                for( const auto& [ id, pInvocation ] : pInvocations->get_invocations() )
                {
                    const auto context = pInvocation->get_context();

                    for( const auto vec : context->get_vectors() )
                    {
                        for( const auto element : vec->get_elements() )
                        {
                            auto interfaceContext = element->get_interface();
                            if( interfaceContext->get_context().has_value() )
                            {
                                invocationInfo.contextInvocations.insert(
                                    { interfaceContext->get_context().value(), pInvocation } );
                            }
                            else
                            {
                                invocationInfo.dimensionInvocations.insert(
                                    { interfaceContext->get_dimension().value(), pInvocation } );
                            }
                        }
                    }
                }
            }

            Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( environment.project_manifest() );

            std::ostringstream osInterface;
            nlohmann::json     interfaceOperations;
            nlohmann::json     traitStructs;
            {
                for( const auto& megaSrcFile : manifest.getMegaSourceFiles() )
                {
                    Interface::Root*      pRoot = database.one< Interface::Root >( megaSrcFile );
                    CleverUtility::IDList namespaces, types;
                    for( Interface::IContext* pContext : pRoot->get_children() )
                    {
                        recurseInterface( invocationInfo, pSymbolTable, templateEngine, namespaces, types, pContext,
                                          osInterface, interfaceOperations, traitStructs );
                    }
                }
            }
            std::ostringstream osConcrete;

            nlohmann::json data( { { "cppIncludes", nlohmann::json::array() },
                                   { "systemIncludes", nlohmann::json::array() },
                                   { "result_types", nlohmann::json::array() },
                                   { "interface", osInterface.str() },
                                   { "objects", nlohmann::json::array() },
                                   { "trait_structs", traitStructs },
                                   { "operations", interfaceOperations }

            } );

            {
                std::set< boost::filesystem::path > cppIncludes;
                for( const mega::io::megaFilePath& megaFile : manifest.getMegaSourceFiles() )
                {
                    for( Parser::CPPInclude* pCPPInclude : database.many< Parser::CPPInclude >( megaFile ) )
                    {
                        cppIncludes.insert( pCPPInclude->get_cppSourceFilePath() );
                    }
                }
                for( const auto& cppInclude : cppIncludes )
                {
                    data[ "cppIncludes" ].push_back( cppInclude.string() );
                }
            }
            {
                std::set< std::string > systemIncludes;
                for( const mega::io::megaFilePath& megaFile : manifest.getMegaSourceFiles() )
                {
                    for( Parser::SystemInclude* pSystemInclude : database.many< Parser::SystemInclude >( megaFile ) )
                    {
                        systemIncludes.insert( pSystemInclude->get_str() );
                    }
                }
                for( const auto& systemInclude : systemIncludes )
                {
                    data[ "systemIncludes" ].push_back( systemInclude );
                }
            }

            using ConcreteNames = std::map< Concrete::Context*, std::string >;
            ConcreteNames concreteNames;
            {
                for( const auto& megaSrcFile : manifest.getMegaSourceFiles() )
                {
                    for( Concrete::Object* pObject : database.many< Concrete::Object >( megaSrcFile ) )
                    {
                        std::ostringstream os;
                        os << "o_" << pObject->get_concrete_id().getObjectID();
                        concreteNames.insert( { ( Concrete::Context* )pObject, os.str() } );

                        nlohmann::json object( { { "type", os.str() }, { "members", nlohmann::json::array() }

                        } );

                        for( auto pBuffer : pObject->get_buffers() )
                        {
                            for( auto pPart : pBuffer->get_parts() )
                            {
                                for( auto pAllocation : pPart->get_allocation_dimensions() )
                                {
                                    auto pAllocatorDim = db_cast< Concrete::Dimensions::Allocator >( pAllocation );
                                    VERIFY_RTE( pAllocatorDim );

                                    auto getLocalDomainSize = []( Concrete::Context* pContext ) -> mega::U64
                                    {
                                        if( auto pObject = db_cast< Concrete::Object >( pContext ) )
                                        {
                                            return 1U;
                                        }
                                        else if( auto pEvent = db_cast< Concrete::Event >( pContext ) )
                                        {
                                            return pEvent->get_local_size();
                                        }
                                        else if( auto pAction = db_cast< Concrete::Action >( pContext ) )
                                        {
                                            return pAction->get_local_size();
                                        }
                                        else if( auto pLink = db_cast< Concrete::Link >( pContext ) )
                                        {
                                            return 1U;
                                        }
                                        else
                                        {
                                            return 1U;
                                        }
                                    };

                                    std::ostringstream     osTypeName;
                                    Allocators::Allocator* pAllocator = pAllocatorDim->get_allocator();
                                    if( auto pAlloc = db_cast< Allocators::Nothing >( pAllocator ) )
                                    {
                                        // do nothing
                                        THROW_RTE( "Unreachable" );
                                    }
                                    else if( auto pAlloc = db_cast< Allocators::Singleton >( pAllocator ) )
                                    {
                                        osTypeName << "bool";
                                    }
                                    else if( auto pAlloc = db_cast< Allocators::Range32 >( pAllocator ) )
                                    {
                                        osTypeName << "mega::Bitmask32Allocator< "
                                                   << getLocalDomainSize( pAlloc->get_allocated_context() ) << " >";
                                    }
                                    else if( auto pAlloc = db_cast< Allocators::Range64 >( pAllocator ) )
                                    {
                                        osTypeName << "mega::Bitmask64Allocator< "
                                                   << getLocalDomainSize( pAlloc->get_allocated_context() ) << " >";
                                    }
                                    else if( auto pAlloc = db_cast< Allocators::RangeAny >( pAllocator ) )
                                    {
                                        osTypeName << "mega::RingAllocator< mega::Instance, "
                                                   << getLocalDomainSize( pAlloc->get_allocated_context() ) << " >";
                                    }
                                    else
                                    {
                                        THROW_RTE( "Unknown allocator type" );
                                    }

                                    std::ostringstream osName;
                                    osName << "m_" << pAllocation->get_concrete_id().getSubObjectID();
                                    nlohmann::json member( { { "type", osTypeName.str() }, { "name", osName.str() } } );
                                    object[ "members" ].push_back( member );
                                    concreteNames.insert( { ( Concrete::Context* )pAllocation, osName.str() } );
                                }
                                for( auto pLink : pPart->get_link_dimensions() )
                                {
                                    std::ostringstream osType;
                                    if( auto pLinkSingle = db_cast< Concrete::Dimensions::LinkSingle >( pLink ) )
                                    {
                                        osType << "mega::reference";
                                    }
                                    else if( auto pLinkMany = db_cast< Concrete::Dimensions::LinkMany >( pLink ) )
                                    {
                                        osType << "std::vector< mega::reference >";
                                    }
                                    else
                                    {
                                        THROW_RTE( "Unknown link reference type" );
                                    }
                                    std::ostringstream osName;
                                    osName << "m_" << pLink->get_concrete_id().getSubObjectID();
                                    nlohmann::json member( { { "type", osType.str() }, { "name", osName.str() } } );
                                    object[ "members" ].push_back( member );
                                    concreteNames.insert( { ( Concrete::Context* )pLink, osName.str() } );
                                }
                                for( auto pDim : pPart->get_user_dimensions() )
                                {
                                    std::ostringstream osName;
                                    osName << "m_" << pDim->get_concrete_id().getSubObjectID();
                                    nlohmann::json member(
                                        { { "type", pDim->get_interface_dimension()->get_canonical_type() },
                                          { "name", osName.str() } } );
                                    object[ "members" ].push_back( member );
                                    concreteNames.insert( { ( Concrete::Context* )pDim, osName.str() } );
                                }
                            }
                        }
                        data[ "objects" ].push_back( object );
                    }
                }
            }

            std::ostringstream osOperations;
            {
                for( const auto& megaSrcFile : manifest.getMegaSourceFiles() )
                {
                    Interface::Root* pRoot = database.one< Interface::Root >( megaSrcFile );

                    CleverUtility::IDList namespaces, types;
                    for( Interface::IContext* pContext : pRoot->get_children() )
                    {
                        recurseOperations( pContext, data, namespaces, types );
                    }
                }
            }

            std::ostringstream os;
            templateEngine.renderRetail( data, os );
            strCode = os.str();
            mega::utilities::clang_format( strCode, std::optional< boost::filesystem::path >() );
        }
        const boost::filesystem::path retailSrcPath = retailSourceDir / "code.cpp";
        boost::filesystem::updateFileIfChanged( retailSrcPath, strCode );
        std::cout << "Generated: " << retailSrcPath.string() << std::endl;
    }
}
} // namespace driver::retail
