
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

#ifndef GUARD_2023_January_25_inja
#define GUARD_2023_January_25_inja

#include "service/network/log.hpp"

#include "utilities/megastructure_installation.hpp"
#include "utilities/project.hpp"

#include "common/requireSemicolon.hpp"
#include "common/assert_verify.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include "boost/filesystem/operations.hpp"

#include <memory>

namespace mega
{

class Inja
{
    ::inja::Environment m_injaEnvironment;

    enum TemplateType
    {
        eAllocator,
        eRelation,
        eProgram,
        eInvocation,
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
    using Ptr = std::unique_ptr< Inja >;

    Inja( const mega::MegastructureInstallation& megaInstall, const mega::Project& project )
        : m_templateNames{ "allocator.jinja", "relation.jinja", "program.jinja", "invocation.jinja" }
    {
        m_injaEnvironment.set_trim_blocks( true );

        const auto runtimeTemplatesDir = megaInstall.getRuntimeTemplateDir();

        for( int i = 0; i != TOTAL_TEMPLATE_TYPES; ++i )
        {
            const auto templateType = static_cast< TemplateType >( i );
            auto       templatePath = runtimeTemplatesDir / m_templateNames[ templateType ];
            VERIFY_RTE_MSG( boost::filesystem::exists( templatePath ),
                            "Failed to locate runtime template: " << templatePath.string() );
            m_templates[ templateType ] = m_injaEnvironment.parse_template( templatePath.string() );
        }
    }

    void render_invocation( const nlohmann::json& data, std::ostream& os ) { renderTemplate( data, eInvocation, os ); }
    void render_allocator( const nlohmann::json& data, std::ostream& os ) { renderTemplate( data, eAllocator, os ); }
    void render_relation( const nlohmann::json& data, std::ostream& os ) { renderTemplate( data, eRelation, os ); }
    void render_program( const nlohmann::json& data, std::ostream& os ) { renderTemplate( data, eProgram, os ); }

    std::string render( const std::string& strTemplate, const nlohmann::json& data )
    {
        try
        {
            return m_injaEnvironment.render( strTemplate, data );
        }
        catch( inja::InjaError& ex )
        {
            SPDLOG_ERROR( "inja::InjaError in CodeGenerator::render rendering: {}", ex.what() );
            THROW_RTE( "inja::InjaError in CodeGenerator::render rendering: " << ex.what() );
        }
    }
};

} // namespace mega

#endif // GUARD_2023_January_25_inja
