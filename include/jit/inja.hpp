
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

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

#include <memory>

namespace mega
{

class Inja
{
    ::inja::Environment m_injaEnvironment;
    ::inja::Template    m_allocatorTemplate;
    ::inja::Template    m_callTemplate;
    ::inja::Template    m_getTemplate;
    ::inja::Template    m_programTemplate;
    ::inja::Template    m_readLinkTemplate;
    ::inja::Template    m_readTemplate;
    ::inja::Template    m_relationTemplate;
    ::inja::Template    m_startTemplate;
    ::inja::Template    m_writeLinkTemplate;
    ::inja::Template    m_writeTemplate;
    ::inja::Template    m_moveTemplate;

public:
    using Ptr = std::unique_ptr< Inja >;

    Inja( const mega::MegastructureInstallation& megaInstall, const mega::Project& project )
    {
        m_injaEnvironment.set_trim_blocks( true );
        m_allocatorTemplate = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateAllocation().string() );
        m_callTemplate      = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateCall().string() );
        m_getTemplate       = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateGet().string() );
        m_programTemplate   = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateProgram().string() );
        m_readLinkTemplate  = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateReadLink().string() );
        m_readTemplate      = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateRead().string() );
        m_relationTemplate  = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateRelation().string() );
        m_startTemplate     = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateStart().string() );
        m_writeLinkTemplate = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateWriteLink().string() );
        m_writeTemplate     = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateWrite().string() );
        m_moveTemplate      = m_injaEnvironment.parse_template( megaInstall.getRuntimeTemplateMove().string() );
    }

    void render_allocator( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_allocatorTemplate, data );
    }
    void render_read( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_readTemplate, data );
    }
    void render_write( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_writeTemplate, data );
    }
    void render_readLink( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_readLinkTemplate, data );
    }
    void render_writeLink( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_writeLinkTemplate, data );
    }
    void render_call( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_callTemplate, data );
    }
    void render_start( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_startTemplate, data );
    }
    void render_get( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_getTemplate, data );
    }
    void render_relation( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_relationTemplate, data );
    }
    void render_program( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_programTemplate, data );
    }
    void render_move( const nlohmann::json& data, std::ostream& os )
    {
        m_injaEnvironment.render_to( os, m_moveTemplate, data );
    }

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
