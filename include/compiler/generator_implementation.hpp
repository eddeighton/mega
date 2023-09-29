
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

class ImplementationGen
{
    class TemplateEngine
    {
        const mega::io::StashEnvironment& m_environment;
        ::inja::Environment&              m_injaEnvironment;
        ::inja::Template                  m_implTemplate;

    public:
        TemplateEngine( const mega::io::StashEnvironment& buildEnvironment, ::inja::Environment& injaEnv )
            : m_environment( buildEnvironment )
            , m_injaEnvironment( injaEnv )
            , m_implTemplate( m_injaEnvironment.parse_template( m_environment.ImplementationTemplate().string() ) )
        {
        }

        void renderImpl( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_implTemplate, data );
        }
    };

    const mega::io::StashEnvironment& m_environment;
    std::string                       m_strUnitName;

public:
    ImplementationGen( const mega::io::StashEnvironment& buildEnvironment, const std::string& strUnitName )
        : m_environment( buildEnvironment )
        , m_strUnitName( strUnitName )
    {
    }

    void generate( Operations::Invocations* pInvocations, std::ostream& os )
    {
        ::inja::Environment injaEnvironment;
        {
            injaEnvironment.set_trim_blocks( true );
        }

        TemplateEngine templateEngine( m_environment, injaEnvironment );

        nlohmann::json implData( { { "unitname", m_strUnitName },
                                   { "invocations", nlohmann::json::array() },
                                   { "interfaces", nlohmann::json::array() } } );

        for( auto& [ id, pInvocation ] : pInvocations->get_invocations() )
        {
            std::ostringstream osContextIDs;
            {
                bool bFirst = true;
                for( mega::TypeID typeID : id.m_context )
                {
                    if( bFirst )
                        bFirst = false;
                    else
                        osContextIDs << ',';
                    osContextIDs << "mega::TypeID{ " << typeID.getSymbolID() << " }";
                }
            }

            std::ostringstream osTypePathIDs;
            {
                bool bFirst = true;
                for( mega::TypeID typeID : id.m_type_path )
                {
                    if( bFirst )
                        bFirst = false;
                    else
                        osTypePathIDs << ',';
                    osTypePathIDs << "mega::TypeID{ " << typeID.getSymbolID() << " }";
                }
            }

            std::ostringstream osRuntimeReturnType;
            {
                // deterine the runtime return type
                auto pReturnType = pInvocation->get_return_type();
                if( auto pVoid = db_cast< Operations::ReturnTypes::Void >( pReturnType ) )
                {
                    osRuntimeReturnType << "void";
                }
                else if( auto pDimension = db_cast< Operations::ReturnTypes::Dimension >( pReturnType ) )
                {
                    std::set< std::string > canonicalTypes;
                    for( auto pDim : pDimension->get_dimensions() )
                    {
                        canonicalTypes.insert( pDim->get_canonical_type() );
                    }
                    VERIFY_RTE_MSG( canonicalTypes.size() == 1,
                                    "Cannot generate runtime return type for non-homogeneous dimension" );
                    osRuntimeReturnType << *canonicalTypes.begin();
                }
                else if( auto pFunctionReturnType = db_cast< Operations::ReturnTypes::Function >( pReturnType ) )
                {
                    std::set< std::string >                canonicalTypes;
                    std::set< std::vector< std::string > > parameterTypes;
                    for( Interface::Function* pFunction : pFunctionReturnType->get_functions() )
                    {
                        canonicalTypes.insert( pFunction->get_return_type_trait()->get_canonical_type() );
                        parameterTypes.insert( pFunction->get_arguments_trait()->get_canonical_types() );
                    }
                    VERIFY_RTE_MSG( canonicalTypes.size() == 1,
                                    "Cannot generate runtime return type for non-homogeneous function" );
                    VERIFY_RTE_MSG(
                        parameterTypes.size() == 1, "Cannot generate runtime parameters for non-homogeneous function" );

                    const std::string&                strReturnType = *canonicalTypes.begin();
                    const std::vector< std::string >& parameters    = *parameterTypes.begin();

                    // define function pointer type
                    osRuntimeReturnType << strReturnType << "(*)( " << mega::psz_mega_reference << "* ";

                    for( const std::string& strType : parameters )
                        osRuntimeReturnType << "," << strType;

                    osRuntimeReturnType << ")";
                }
                else if( auto pContext = db_cast< Operations::ReturnTypes::Context >( pReturnType ) )
                {
                    osRuntimeReturnType << mega::psz_mega_reference;
                }
                else if( auto pRange = db_cast< Operations::ReturnTypes::Range >( pReturnType ) )
                {
                    osRuntimeReturnType << mega::psz_mega_reference_vector;
                }
                else
                {
                    THROW_RTE( "Unknown return type" );
                }
            }

            nlohmann::json invocation( {

                { "return_type", pInvocation->get_return_type()->get_canonical_type() },
                { "runtime_return_type", osRuntimeReturnType.str() },

                { "context", pInvocation->get_canonical_context() },
                { "context_type_id_list", osContextIDs.str() },
                { "context_size", id.m_context.size() },

                { "type_path", pInvocation->get_canonical_type_path() },
                { "type_path_type_id_list", osTypePathIDs.str() },
                { "type_path_size", id.m_type_path.size() },

                { "operation", pInvocation->get_canonical_operation() },
                { "explicit_operation", mega::getExplicitOperationString( pInvocation->get_explicit_operation() ) }

            } );

            implData[ "invocations" ].push_back( invocation );
        }

        templateEngine.renderImpl( implData, os );
    }
};
