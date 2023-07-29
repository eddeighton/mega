
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

#ifndef GUARD_2023_May_23_templates
#define GUARD_2023_May_23_templates

#include "database/model/FinalStage.hxx"

#include "compiler/generator_utility.hpp"

#include "utilities/clang_format.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <map>

namespace driver::retail
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

/*
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
*/
}

#endif //GUARD_2023_May_23_templates
