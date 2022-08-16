
#include "code_generator.hpp"
#include "database/common/environment_archive.hpp"

#include "nlohmann/json.hpp"

#include "inja/inja.hpp"
#include "inja/environment.hpp"
#include "inja/template.hpp"

namespace
{
/*
    class TemplateEngine
    {
        const mega::io::ArchiveEnvironment& m_environment;
        ::inja::Environment&              m_injaEnvironment;
        ::inja::Template                  m_readTemplate;

    public:
        TemplateEngine( const mega::io::ArchiveEnvironment& buildEnvironment, ::inja::Environment& injaEnv )
            : m_environment( buildEnvironment )
            , m_injaEnvironment( injaEnv )
            , m_readTemplate( m_injaEnvironment.parse_template( m_environment.CodeGen_ReadTemplate() ) )
        {
        }

        void renderRead( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_readTemplate, data );
        }
    };*/
}

namespace mega
{
    namespace runtime
    {
        void CodeGenerator::generate_read( DatabaseInstance& database, const mega::InvocationID& invocationID )
        {

            
        }
    }
}
