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

#include "database/CPPSourceStage.hxx"

#include "mega/common_strings.hpp"
#include "mega/make_unique_without_reorder.hpp"

#include "common/file.hpp"

#include <boost/wave.hpp>
#include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp> // lexer class

#include <memory>
#include <iostream>

namespace CPPSourceStage
{
#include "compiler/symbol_path.hpp"
#include "compiler/common_ancestor.hpp"
#include "compiler/interface.hpp"
#include "compiler/concrete.hpp"

namespace FunctionBody
{
#include "compiler/derivation.hpp"
}

} // namespace CPPSourceStage

using namespace CPPSourceStage;

struct SymbolInfo
{
    using SymbolNameMap       = std::map< std::string, Symbols::SymbolID* >;
    using SymbolIDMap         = std::map< ::mega::interface::SymbolID, Symbols::SymbolID* >;
    using SymbolIDSequenceMap = std::map< ::mega::interface::SymbolIDSequence, Symbols::InterfaceTypeID* >;

    SymbolNameMap       symbolNameMap;
    SymbolIDMap         symbolIDMap;
    SymbolIDSequenceMap symbolSequenceMap;
};

namespace mega::compiler
{

struct IncludeFileReplacements : public boost::wave::context_policies::default_preprocessing_hooks
{
    IncludeFileReplacements( std::ostream& os )
        : os( os )
    {
    }

    template < typename ContextT >
    bool locate_include_file( ContextT& ctx, std::string& file_path, bool is_system, char const* current_name,
                              std::string& dir_path, std::string& native_name )
    {
        if( !ctx.find_include_file( file_path, dir_path, is_system, current_name ) )
        {
            return false; // could not locate file
        }

        boost::filesystem::path native_path( boost::wave::util::create_path( file_path ) );
        if( !boost::filesystem::exists( native_path ) )
        {
            BOOST_WAVE_THROW_CTX(
                ctx, boost::wave::preprocess_exception, bad_include_file, file_path.c_str(), ctx.get_main_pos() );
            return false;
        }

        // return the unique full file system path of the located file
        native_name = boost::wave::util::native_file_string( native_path );
        return true; // include file has been located successfully
    }

    template < typename ContextT >
    bool found_include_directive( const ContextT& ctx, const std::string& filename, bool include_next )
    {
        // if system include then DO NOT expand
        if( !filename.empty() && filename.front() == '<' )
        {
            os << "#include " << filename << "\n";
            return true; // return true to AVOID expanding the include
        }
        else
        {
            return false;
        }
    }

    std::ostream& os;
};

static void printLineDirective( const boost::wave::util::file_position_type& current_position, std::ostream& os )
{
    os << "\n#line " << current_position.get_line() << " \"" << current_position.get_file() << "\"\n";
}

class Task_CPP_Source : public BaseTask
{
    const mega::io::cppFilePath& m_sourceFilePath;

    using token_type        = boost::wave::cpplexer::lex_token<>;
    using lex_iterator_type = boost::wave::cpplexer::lex_iterator< token_type >;
    using context_type
        = boost::wave::context< std::string::iterator, lex_iterator_type,
                                boost::wave::iteration_context_policies::load_file_to_string, IncludeFileReplacements >;

    class Namespacing
    {
        using NamespaceList = std::pair< int, std::vector< std::string > >;

        std::vector< NamespaceList > m_namespaceStack;
        int                          m_iBraceCount            = 0;
        bool                         m_bSpecifyingNameSpace   = false;
        bool                         m_bPreviousTokenWasUsing = false;

        const SymbolInfo& m_symbolInfo;
        std::ostream&     m_os;

    public:
        Namespacing( const SymbolInfo& symbolInfo, std::ostream& os )
            : m_symbolInfo( symbolInfo )
            , m_os( os )
        {
            m_namespaceStack.push_back( { -1, {} } );
        }

        std::vector< Symbols::SymbolID* > collectSymbolSequence() const
        {
            std::vector< Symbols::SymbolID* > symbols;

            for( const auto& ns : m_namespaceStack )
            {
                VERIFY_RTE_MSG(
                    ( ns.first == -1 ) || ( !ns.second.empty() ), "Invalid anonymous namespace in namespace stack" );
                for( const auto& n : ns.second )
                {
                    auto iFind = m_symbolInfo.symbolNameMap.find( n );
                    VERIFY_RTE_MSG( iFind != m_symbolInfo.symbolNameMap.end(), "Failed to locate symbol: " << n );
                    symbols.push_back( iFind->second );
                }
            }

            return symbols;
        }

        void onToken( const token_type::string_type& strToken )
        {
            if( strToken == "{" )
            {
                ++m_iBraceCount;
            }
            else if( strToken == "}" )
            {
                --m_iBraceCount;
                if( m_namespaceStack.back().first == m_iBraceCount )
                {
                    m_namespaceStack.pop_back();
                }
                VERIFY_RTE( !m_namespaceStack.empty() );
            }

            // track namespaces
            if( !m_bPreviousTokenWasUsing && strToken == "namespace" )
            {
                m_bSpecifyingNameSpace = true;
                m_namespaceStack.push_back( { m_iBraceCount, {} } );
            }
            else if( m_bSpecifyingNameSpace )
            {
                if( strToken == "{" )
                {
                    m_bSpecifyingNameSpace = false;
                }
                else if( ( strToken == "::" ) || ( strToken == ":" ) )
                {
                }
                else
                {
                    m_namespaceStack.back().second.push_back( strToken.c_str() );
                }
            }

            if( strToken == "using" )
            {
                m_bPreviousTokenWasUsing = true;
            }
            else
            {
                m_bPreviousTokenWasUsing = false;
            }
        }
    };

    class Replacement
    {
    protected:
        Database&         m_database;
        const SymbolInfo& m_symbolInfo;
        Namespacing&      m_namespacing;

    public:
        enum TokenAction
        {
            eMaybe,
            eConsumed,
            eRejected
        };

        Replacement( Database& database, const SymbolInfo& symbolInfo, Namespacing& namespacing )
            : m_database( database )
            , m_symbolInfo( symbolInfo )
            , m_namespacing( namespacing )
        {
        }
        virtual ~Replacement()                                                                = default;
        virtual TokenAction onToken( const token_type::string_type& strToken, bool bIsSpace ) = 0;
        virtual std::string result()                                                          = 0;
        virtual bool        addLineDirective() const                                          = 0;
    };

    class TypeReplacement : public Replacement
    {
    protected:
        int                               m_state = 0;
        std::vector< Symbols::SymbolID* > m_symbols;

    public:
        TypeReplacement( Database& database, const SymbolInfo& symbolInfo, Namespacing& namespacing )
            : Replacement( database, symbolInfo, namespacing )
        {
        }
        virtual ~TypeReplacement() = default;

        TokenAction onToken( const token_type::string_type& strToken, bool bIsSpace ) override
        {
            if( bIsSpace )
            {
                // consume ALL whitespace IF next token is part of type
                return eMaybe;
            }
            switch( m_state )
            {
                default:
                case 0:
                {
                    auto iFind = m_symbolInfo.symbolNameMap.find( strToken.c_str() );
                    VERIFY_RTE_MSG(
                        iFind != m_symbolInfo.symbolNameMap.end(), "Failed to locate symbol: " << strToken );
                    m_symbols.push_back( iFind->second );
                    m_state = 1;
                    return eConsumed;
                }
                break;
                case 1:
                {
                    if( strToken == "." )
                    {
                        m_state = 0;
                        return eConsumed;
                    }
                    else
                    {
                        return eRejected;
                    }
                }
                break;
            }
        }
        bool addLineDirective() const override { return false; }
    };

    class DerivationReplacement : public TypeReplacement
    {
        Interface::Node* m_pInterfaceContext = nullptr;

    public:
        DerivationReplacement( Database& database, const SymbolInfo& symbolInfo, Namespacing& namespacing,
                               Interface::Node* pInterfaceNode )
            : TypeReplacement( database, symbolInfo, namespacing )
            , m_pInterfaceContext( pInterfaceNode )
        {
        }

        std::string result() override
        {
            FunctionBody::InterObjectDerivationPolicy       policy{ m_database };
            FunctionBody::InterObjectDerivationPolicy::Spec spec{
                m_pInterfaceContext->get_inheritors(), m_symbols, true };
            std::vector< FunctionBody::Derivation::Or* > frontier;
            auto pRoot = FunctionBody::solveContextFree( spec, policy, frontier );

            std::ostringstream os;
            {
                os << MEGA_POINTER << "< ";
                bool                                bFirst = true;
                std::set< mega::interface::TypeID > uniqueInterfaceTypeIDs;
                for( auto pOr : frontier )
                {
                    const auto interfaceTypeID = Concrete::getInterfaceTypeID( pOr->get_vertex() );
                    if( !uniqueInterfaceTypeIDs.contains( interfaceTypeID ) )
                    {
                        if( bFirst )
                        {
                            bFirst = false;
                        }
                        else
                        {
                            os << ", ";
                        }
                        os << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << interfaceTypeID.getValue();
                        uniqueInterfaceTypeIDs.insert( interfaceTypeID );
                    }
                }
                os << " >";
            }

            return os.str();
        }
    };

    class AbsoluteReplacement : public TypeReplacement
    {
    public:
        AbsoluteReplacement( Database& database, const SymbolInfo& symbolInfo, Namespacing& namespacing )
            : TypeReplacement( database, symbolInfo, namespacing )
        {
        }

        std::string result() override
        {
            mega::interface::SymbolIDSequence symbolIDSeq;
            for( auto pSymbolID : m_symbols )
            {
                symbolIDSeq.push_back( pSymbolID->get_id() );
            }
            auto pInterfaceNode
                = Interface::resolve( m_symbolInfo.symbolSequenceMap, m_symbolInfo.symbolIDMap, symbolIDSeq );

            std::ostringstream os;
            os << MEGA_POINTER << "< "
               << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' )
               << pInterfaceNode->get_interface_id()->get_type_id().getValue() << " > ";
            return os.str();
        }
    };

    class FunctionReplacement : public Replacement
    {
    public:
        enum FunctionType
        {
            eFunction,
            eAction,
            eInterupt,
            eRequirement,
            eDecider
        };

    protected:
        FunctionType m_functionType;

        enum State
        {
            eName,
            eArgs,
            eArrow,
            eReturn,
            eBody
        } m_state
            = eName;

        std::vector< Symbols::SymbolID* > m_symbols;
        Interface::Node*                  m_pInterfaceNode = nullptr;
        int                               m_parenCount     = 0;
        std::ostringstream                m_osLineStart;
        std::ostringstream                m_osArgs;
        int                               m_braceCount = 0;
        std::ostringstream                m_osReturn;
        std::ostringstream                m_osBody;
        std::unique_ptr< Replacement >    m_pReplacement;
        std::optional< std::string >      m_maybeConsumed;

        void calculateInterfaceType()
        {
            mega::interface::SymbolIDSequence symbolIDSeq;
            for( auto pSymbolID : m_symbols )
            {
                symbolIDSeq.push_back( pSymbolID->get_id() );
            }
            m_pInterfaceNode
                = Interface::resolve( m_symbolInfo.symbolSequenceMap, m_symbolInfo.symbolIDMap, symbolIDSeq );
            VERIFY_RTE_MSG( m_pInterfaceNode, "Failed to resolve interface node type" );
        }

    public:
        FunctionReplacement( Database& database, const SymbolInfo& symbolInfo, Namespacing& namespacing,
                             FunctionType functionType, const boost::wave::util::file_position_type& current_position )
            : Replacement( database, symbolInfo, namespacing )
            , m_functionType( functionType )
        {
            printLineDirective( current_position, m_osLineStart );
            // determine the interface type of the function
            m_symbols = m_namespacing.collectSymbolSequence();
        }
        bool addLineDirective() const override { return true; }

        TokenAction onToken( const token_type::string_type& strToken, bool bIsSpace ) override
        {
            if( m_pReplacement )
            {
                switch( m_pReplacement->onToken( strToken, bIsSpace ) )
                {
                    case eMaybe:
                    {
                        m_maybeConsumed = strToken.c_str();
                        return eConsumed;
                    }
                    break;
                    case eConsumed:
                    {
                        m_maybeConsumed.reset();
                        return eConsumed;
                    }
                    break;
                    case eRejected:
                    {
                        m_osArgs << m_pReplacement->result();
                        m_pReplacement.reset();
                        if( m_maybeConsumed.has_value() )
                        {
                            m_osArgs << m_maybeConsumed.value();
                            m_maybeConsumed.reset();
                        }
                    }
                    break;
                }
            }

            switch( m_state )
            {
                case eName:
                {
                    if( bIsSpace )
                    {
                        // ignor
                        return eConsumed;
                    }
                    else if( strToken == "::" || strToken == ":" )
                    {
                        return eConsumed;
                    }
                    else if( strToken == "{" )
                    {
                        // determine function interface type
                        calculateInterfaceType();

                        m_braceCount = 1;
                        m_state      = eBody;
                        return eConsumed;
                    }
                    else if( strToken == "(" )
                    {
                        // determine function interface type
                        calculateInterfaceType();

                        m_parenCount = 1;
                        m_state      = eArgs;
                        return eConsumed;
                    }
                    else
                    {
                        auto iFind = m_symbolInfo.symbolNameMap.find( strToken.c_str() );
                        VERIFY_RTE_MSG(
                            iFind != m_symbolInfo.symbolNameMap.end(), "Failed to locate symbol: " << strToken );
                        m_symbols.push_back( iFind->second );
                        return eConsumed;
                    }
                }
                break;
                case eArgs:
                {
                    if( bIsSpace )
                    {
                        m_osArgs << strToken;
                        return eConsumed;
                    }
                    else if( strToken == "(" )
                    {
                        ++m_parenCount;
                        m_osArgs << strToken;
                        return eConsumed;
                    }
                    else if( strToken == ")" )
                    {
                        --m_parenCount;
                        if( m_parenCount == 0 )
                        {
                            m_state = eArrow;
                        }
                        else
                        {
                            m_osArgs << strToken;
                        }
                        return eConsumed;
                    }
                    else if( strToken == "^" )
                    {
                        m_pReplacement
                            = std::make_unique< AbsoluteReplacement >( m_database, m_symbolInfo, m_namespacing );
                        return eConsumed;
                    }
                    else if( strToken == "@" )
                    {
                        m_pReplacement = std::make_unique< DerivationReplacement >(
                            m_database, m_symbolInfo, m_namespacing, m_pInterfaceNode );
                        return eConsumed;
                    }
                    else
                    {
                        m_osArgs << strToken;
                        return eConsumed;
                    }
                }
                break;
                case eArrow:
                {
                    if( bIsSpace )
                    {
                        // ignor
                        return eConsumed;
                    }
                    else if( strToken == "->" )
                    {
                        m_state = eReturn;
                        return eConsumed;
                    }
                    else if( strToken == "{" )
                    {
                        m_braceCount = 1;
                        m_state      = eBody;
                        return eConsumed;
                    }
                    else
                    {
                        THROW_RTE( "Unexpected token while parsing function" );
                    }
                }
                break;
                case eReturn:
                {
                    if( bIsSpace )
                    {
                        // ignor
                        return eConsumed;
                    }
                    else if( strToken == "{" )
                    {
                        m_braceCount = 1;
                        m_state      = eBody;
                        return eConsumed;
                    }
                    else if( strToken == "^" )
                    {
                        m_pReplacement
                            = std::make_unique< AbsoluteReplacement >( m_database, m_symbolInfo, m_namespacing );
                        return eConsumed;
                    }
                    else if( strToken == "@" )
                    {
                        m_pReplacement = std::make_unique< DerivationReplacement >(
                            m_database, m_symbolInfo, m_namespacing, m_pInterfaceNode );
                        return eConsumed;
                    }
                    else
                    {
                        m_osReturn << strToken;
                        return eConsumed;
                    }
                }
                break;
                case eBody:
                {
                    if( bIsSpace )
                    {
                        m_osBody << strToken;
                        return eConsumed;
                    }
                    else if( strToken == "{" )
                    {
                        ++m_braceCount;
                        m_osBody << strToken;
                        return eConsumed;
                    }
                    else if( strToken == "}" )
                    {
                        --m_braceCount;
                        if( m_braceCount == 0 )
                        {
                            return eRejected;
                        }
                        else
                        {
                            m_osBody << strToken;
                            return eConsumed;
                        }
                    }
                    else if( strToken == "^" )
                    {
                        m_pReplacement
                            = std::make_unique< AbsoluteReplacement >( m_database, m_symbolInfo, m_namespacing );
                        return eConsumed;
                    }
                    else if( strToken == "@" )
                    {
                        m_pReplacement = std::make_unique< DerivationReplacement >(
                            m_database, m_symbolInfo, m_namespacing, m_pInterfaceNode );
                        return eConsumed;
                    }
                    else
                    {
                        m_osBody << strToken;
                        return eConsumed;
                    }
                }
                break;
            }
            THROW_RTE( "Unexpected error while parsing function" );
        }

        std::string result() override
        {
            std::ostringstream os;

            VERIFY_RTE( !m_symbols.empty() );

            os << "namespace ";
            bool bFirst = true;
            for( const auto& pSymbolID : m_symbols )
            {
                if( bFirst )
                {
                    bFirst = false;
                }
                else
                {
                    os << "::";
                }
                os << pSymbolID->get_token();
            }

            os << "\n";
            os << "{\n";
            os << "namespace\n";
            os << "{\n";
            {
                std::ostringstream osTypeID;
                osTypeID << std::hex << std::setw( 8 ) << std::setfill( '0' )
                         << m_pInterfaceNode->get_interface_id()->get_type_id().getValue();
                os << "struct Impl : " << mega::MEGA_POINTER << "< 0x" << osTypeID.str() << " >\n";
            }
            os << "{\n";

            os << m_osLineStart.str();

            switch( m_functionType )
            {
                case eFunction:
                    os << m_osReturn.str();
                    break;
                case eAction:
                    os << "mega::ActionCoroutine";
                    break;
                case eInterupt:
                case eRequirement:
                    os << "void";
                    break;
                case eDecider:
                    os << "int";
                    break;
            }
            os << " operator()( " << m_osArgs.str() << ") const\n";
            os << "{";
            os << m_osBody.str();
            os << "}\n";
            os << "};\n";
            os << "}\n";

            return os.str();
        }
    };

public:
    Task_CPP_Source( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath compilationFile = m_environment.CPPSourceStage_Source( m_sourceFilePath );
        const mega::io::GeneratedHPPSourceFilePath cppHeaderFile = m_environment.CPPSource( m_sourceFilePath );
        const auto                                 srcFilePath   = m_environment.FilePath( m_sourceFilePath );

        start( taskProgress, "Task_CPP_Source", m_sourceFilePath.path(), cppHeaderFile.path() );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash, srcFilePath } );

        if( m_environment.restore( compilationFile, determinant )
            && m_environment.restore( cppHeaderFile, determinant ) )
        {
            m_environment.setBuildHashCode( compilationFile );
            m_environment.setBuildHashCode( cppHeaderFile );
            cached( taskProgress );
            return;
        }

        Database database( m_environment, m_sourceFilePath );

        auto pSymbolTable = database.one< Symbols::SymbolTable >( m_environment.project_manifest() );
        VERIFY_RTE( pSymbolTable );

        SymbolInfo symbolInfo{ pSymbolTable->get_symbol_names(), pSymbolTable->get_symbol_ids(),
                               pSymbolTable->get_interface_symbol_id_sequences() };

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
        VERIFY_RTE( pInterfaceComponent );

        std::string strFileContents;
        boost::filesystem::loadAsciiFile( srcFilePath, strFileContents );

        std::ostringstream osSourceFile;
        {
            // current file position is saved for exception handling
            boost::wave::util::file_position_type current_position;

            try
            {
                IncludeFileReplacements includeFilesReplacements( osSourceFile );

                context_type ctx( strFileContents.begin(), strFileContents.end(), srcFilePath.string().c_str(),
                                  includeFilesReplacements );
                {
                    for( const auto& includeDir : pInterfaceComponent->get_include_directories() )
                    {
                        ctx.add_include_path( includeDir.string().c_str() );
                    }
                    ctx.add_macro_definition( "MEGA_CLANG_COMPILATION" );
                }

                context_type::iterator_type first = ctx.begin();
                context_type::iterator_type last  = ctx.end();

                Namespacing namespacing( symbolInfo, osSourceFile );

                std::unique_ptr< Replacement > pReplacement;

                std::optional< std::string > maybeRejectedToken;
                while( first != last )
                {
                    current_position = ( *first ).get_position();

                    const auto& strToken = ( *first ).get_value();
                    const bool  bIsSpace = ( strToken.size() == 1 ) && ( std::isspace( *strToken.c_str() ) );

                    if( !bIsSpace )
                    {
                        namespacing.onToken( strToken );
                    }

                    bool bConsumed = false;
                    if( pReplacement )
                    {
                        switch( pReplacement->onToken( strToken, bIsSpace ) )
                        {
                            case Replacement::eRejected:
                            {
                                osSourceFile << pReplacement->result();
                                if( pReplacement->addLineDirective() )
                                {
                                    printLineDirective( current_position, osSourceFile );
                                }

                                pReplacement.reset();

                                if( maybeRejectedToken.has_value() )
                                {
                                    osSourceFile << maybeRejectedToken.value();
                                    maybeRejectedToken.reset();
                                }
                            }
                            break;
                            case Replacement::eConsumed:
                            {
                                maybeRejectedToken.reset();
                                bConsumed = true;
                            }
                            break;
                            case Replacement::eMaybe:
                            {
                                maybeRejectedToken = strToken.c_str();
                                bConsumed          = true;
                            }
                            break;
                            default:
                            {
                                THROW_RTE( "Unknown token action type" );
                            }
                            break;
                        }
                    }

                    if( !bConsumed )
                    {
                        if( strToken == "^" )
                        {
                            pReplacement = std::make_unique< AbsoluteReplacement >( database, symbolInfo, namespacing );
                        }
                        else if( strToken == "@" )
                        {
                            THROW_RTE( "Invalid use of derivation at global scope" );
                        }
                        else if( strToken == "function" )
                        {
                            pReplacement = std::make_unique< FunctionReplacement >(
                                database, symbolInfo, namespacing, FunctionReplacement::eFunction, current_position );
                        }
                        else if( strToken == "action" )
                        {
                            pReplacement = std::make_unique< FunctionReplacement >(
                                database, symbolInfo, namespacing, FunctionReplacement::eAction, current_position );
                        }
                        else if( strToken == "interupt" )
                        {
                            pReplacement = std::make_unique< FunctionReplacement >(
                                database, symbolInfo, namespacing, FunctionReplacement::eInterupt, current_position );
                        }
                        else if( strToken == "requirement" )
                        {
                            pReplacement = std::make_unique< FunctionReplacement >( database, symbolInfo, namespacing,
                                                                                    FunctionReplacement::eRequirement,
                                                                                    current_position );
                        }
                        else if( strToken == "decider" )
                        {
                            pReplacement = std::make_unique< FunctionReplacement >(
                                database, symbolInfo, namespacing, FunctionReplacement::eDecider, current_position );
                        }
                        else
                        {
                            osSourceFile << strToken;
                        }
                    }

                    ++first;
                }
            }
            catch( boost::wave::cpp_exception const& e )
            {
                // some preprocessing error
                THROW_RTE( e.file_name() << ":" << e.line_no() << " " << e.description() );
            }
            catch( std::exception const& e )
            {
                // use last recognized token to retrieve the error position
                THROW_RTE( current_position.get_file() << ":" << current_position.get_line() << " "
                                                       << "exception caught: " << e.what() );
            }
        }

        {
            if( boost::filesystem::updateFileIfChanged( m_environment.FilePath( cppHeaderFile ), osSourceFile.str() ) )
            {
                m_environment.setBuildHashCode( cppHeaderFile );
                m_environment.stash( cppHeaderFile, determinant );
            }
            else
            {
                m_environment.setBuildHashCode( cppHeaderFile );
            }

            const task::FileHash fileHashCode = database.save_Source_to_temp();
            m_environment.setBuildHashCode( compilationFile, fileHashCode );
            m_environment.temp_to_real( compilationFile );
            m_environment.stash( compilationFile, determinant );

            succeeded( taskProgress );
            return;
        }
        failed( taskProgress );
    }
};

BaseTask::Ptr create_Task_CPP_Source( const TaskArguments& taskArguments, const mega::io::cppFilePath& sourceFilePath )
{
    return std::make_unique< Task_CPP_Source >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
