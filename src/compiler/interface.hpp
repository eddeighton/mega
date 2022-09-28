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




class InterfaceGen
{
public:
    struct InterfaceNode
    {
        std::vector< IContext* > contexts;

        using Ptr           = std::shared_ptr< InterfaceNode >;
        using PtrVector     = std::vector< Ptr >;
        using IdentifierMap = std::map< std::string, Ptr >;

        PtrVector     children;
        IdentifierMap identifiers;
    };

    static void buildInterfaceTree( InterfaceNode::Ptr pNode, IContext* pContext )
    {
        InterfaceNode::Ptr pChildNode;
        {
            auto iFind = pNode->identifiers.find( pContext->get_identifier() );
            if ( iFind == pNode->identifiers.end() )
            {
                pChildNode = std::make_shared< InterfaceNode >();
                pNode->identifiers.insert( std::make_pair( pContext->get_identifier(), pChildNode ) );
                pNode->children.push_back( pChildNode );
            }
            else
            {
                // check its a namespace
                Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext );
                VERIFY_RTE_MSG( pNamespace, "Duplicate identifier is not namespace: " << pContext->get_identifier() );
                pChildNode = iFind->second;
            }
        }
        VERIFY_RTE( pChildNode );
        pChildNode->contexts.push_back( pContext );

        for ( IContext* pChild : pContext->get_children() )
        {
            buildInterfaceTree( pChildNode, pChild );
        }
    }

    class TemplateEngine
    {
        const mega::io::StashEnvironment& m_environment;
        ::inja::Environment&              m_injaEnvironment;
        ::inja::Template                  m_contextTemplate;
        ::inja::Template                  m_interfaceTemplate;

    public:
        TemplateEngine( const mega::io::StashEnvironment& buildEnvironment, ::inja::Environment& injaEnv )
            : m_environment( buildEnvironment )
            , m_injaEnvironment( injaEnv )
            , m_contextTemplate( m_injaEnvironment.parse_template( m_environment.ContextTemplate().native() ) )
            , m_interfaceTemplate( m_injaEnvironment.parse_template( m_environment.InterfaceTemplate().native() ) )
        {
        }

        void renderContext( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_contextTemplate, data );
        }

        void renderInterface( const nlohmann::json& data, std::ostream& os ) const
        {
            m_injaEnvironment.render_to( os, m_interfaceTemplate, data );
        }
    };

    template < typename TContextType >
    static std::vector< nlohmann::json > getInheritanceTraits( const nlohmann::json& typenames, TContextType* pContext,
                                                               InheritanceTrait* pInheritanceTrait )
    {
        std::vector< nlohmann::json > traits;
        int                           iCounter = 0;
        for ( const std::string& strType : pInheritanceTrait->get_strings() )
        {
            nlohmann::json     traitNames = typenames;
            std::ostringstream os;
            os << mega::EG_BASE_PREFIX_TRAIT_TYPE << iCounter++;
            traitNames.push_back( os.str() );

            nlohmann::json trait_struct( { { "name", os.str() },
                                           { "typeid", pContext->get_type_id() },
                                           { "types", traitNames },
                                           { "traits", nlohmann::json::array() } } );

            {
                std::ostringstream osTrait;
                osTrait << "using Type  = " << strType;
                trait_struct[ "traits" ].push_back( osTrait.str() );
            }
            traits.push_back( trait_struct );
        }
        return traits;
    }

    template < typename TContextType >
    static std::vector< nlohmann::json > getDimensionTraits( const nlohmann::json& typenames, TContextType* pContext,
                                                             const std::vector< DimensionTrait* >& dimensionTraits )
    {
        std::vector< nlohmann::json > traits;
        for ( DimensionTrait* pDimensionTrait : dimensionTraits )
        {
            const std::string& strType = pDimensionTrait->get_type();

            nlohmann::json     traitNames = typenames;
            std::ostringstream os;
            os << pDimensionTrait->get_id()->get_str();
            traitNames.push_back( os.str() );

            nlohmann::json trait_struct( { { "name", os.str() },
                                           { "typeid", pDimensionTrait->get_type_id() },
                                           { "types", traitNames },
                                           { "traits", nlohmann::json::array() } } );

            {
                {
                    std::ostringstream osTrait;
                    osTrait << "using Type = " << strType;
                    trait_struct[ "traits" ].push_back( osTrait.str() );
                }
                {
                    std::ostringstream osTrait;
                    osTrait << "static const mega::U64 " << mega::EG_TRAITS_SIZE << " = mega::DimensionTraits< "
                            << strType << " >::Size";
                    trait_struct[ "traits" ].push_back( osTrait.str() );
                }
                {
                    std::ostringstream osTrait;
                    osTrait << "static const mega::U64 " << mega::EG_TRAITS_ALIGNMENT << " = mega::DimensionTraits< "
                            << strType << " >::Alignment";
                    trait_struct[ "traits" ].push_back( osTrait.str() );
                }
                {
                    std::ostringstream osTrait;
                    osTrait << "static const mega::U64 " << mega::EG_TRAITS_SIMPLE << " = mega::DimensionTraits< "
                            << strType << " >::Simple";
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
                    osTrait << "using Get = mega::DimensionTraits< " << strType << " >::Get";
                    trait_struct[ "traits" ].push_back( osTrait.str() );
                }
            }
            traits.push_back( trait_struct );
        }

        return traits;
    }

    template < typename TContextType >
    static nlohmann::json getSizeTrait( const nlohmann::json& typenames, TContextType* pContext, SizeTrait* pSizeTrait )
    {
        nlohmann::json     traitNames = typenames;
        std::ostringstream os;
        os << mega::EG_SIZE_PREFIX_TRAIT_TYPE;
        traitNames.push_back( os.str() );

        nlohmann::json trait_struct( { { "name", os.str() },
                                       { "typeid", pContext->get_type_id() },
                                       { "types", traitNames },
                                       { "traits", nlohmann::json::array() } } );

        {
            std::ostringstream osTrait;
            osTrait << "static const mega::U64 " << mega::EG_TRAITS_SIZE << " = " << pSizeTrait->get_str();
            trait_struct[ "traits" ].push_back( osTrait.str() );
        }

        return trait_struct;
    }

    template < typename TContextType >
    static nlohmann::json getFunctionTraits( const nlohmann::json& typenames, TContextType* pContext,
                                             ReturnTypeTrait* pReturnTypeTrait )
    {
        nlohmann::json     traitNames = typenames;
        std::ostringstream os;
        os << mega::EG_FUNCTION_TRAIT_TYPE;
        traitNames.push_back( os.str() );

        nlohmann::json trait_struct( { { "name", os.str() },
                                       { "typeid", pContext->get_type_id() },
                                       { "types", traitNames },
                                       { "traits", nlohmann::json::array() } } );
        {
            std::ostringstream osTrait;
            osTrait << "using Type  = " << pReturnTypeTrait->get_str();
            trait_struct[ "traits" ].push_back( osTrait.str() );
        }

        return trait_struct;
    }

    static void recurse( TemplateEngine& templateEngine, InterfaceNode::Ptr pInterfaceNode, nlohmann::json& structs,
                         const nlohmann::json& parentTypeNames, std::ostream& os )
    {
        IContext* pFirstContext = pInterfaceNode->contexts.front();

        nlohmann::json typenames = parentTypeNames;
        typenames.push_back( pFirstContext->get_identifier() );

        mega::SymbolID id;
        {
            if ( pInterfaceNode->contexts.size() > 1 )
            {
                id = pFirstContext->get_symbol();
            }
            else
            {
                id = pFirstContext->get_type_id();
            }
        }

        std::ostringstream osNested;
        for ( InterfaceNode::Ptr pChild : pInterfaceNode->children )
        {
            recurse( templateEngine, pChild, structs, typenames, osNested );
        }

        nlohmann::json contextData( { { "name", pFirstContext->get_identifier() },
                                      { "typeid", id },
                                      { "trait_structs", nlohmann::json::array() },
                                      { "nested", osNested.str() },
                                      { "has_operation", false },
                                      { "operation_return_type", "mega::ActionCoroutine" },
                                      { "operation_parameters", "" }

        } );

        // if ( pInterfaceNode->bCreateTraits )
        for ( IContext* pContext : pInterfaceNode->contexts )
        {
            bool bFoundType = false;

            {
                if ( Namespace* pNamespace = dynamic_database_cast< Namespace >( pContext ) )
                {
                    VERIFY_RTE( !bFoundType );
                    bFoundType = true;
                    for ( const nlohmann::json& trait :
                          getDimensionTraits( typenames, pNamespace, pNamespace->get_dimension_traits() ) )
                    {
                        contextData[ "trait_structs" ].push_back( trait );
                        structs.push_back( trait );
                    }
                    templateEngine.renderContext( contextData, os );
                }
            }
            {
                if ( Abstract* pAbstract = dynamic_database_cast< Abstract >( pContext ) )
                {
                    VERIFY_RTE( !bFoundType );
                    bFoundType = true;
                    if ( auto opt = pAbstract->get_inheritance_trait() )
                    {
                        for ( const nlohmann::json& trait : getInheritanceTraits( typenames, pAbstract, opt.value() ) )
                        {
                            contextData[ "trait_structs" ].push_back( trait );
                            structs.push_back( trait );
                        }
                    }
                    for ( const nlohmann::json& trait :
                          getDimensionTraits( typenames, pAbstract, pAbstract->get_dimension_traits() ) )
                    {
                        contextData[ "trait_structs" ].push_back( trait );
                        structs.push_back( trait );
                    }
                    templateEngine.renderContext( contextData, os );
                }
            }
            {
                if ( Action* pAction = dynamic_database_cast< Action >( pContext ) )
                {
                    VERIFY_RTE( !bFoundType );
                    bFoundType                     = true;
                    contextData[ "has_operation" ] = true;
                    if ( auto opt = pAction->get_inheritance_trait() )
                    {
                        for ( const nlohmann::json& trait : getInheritanceTraits( typenames, pAction, opt.value() ) )
                        {
                            contextData[ "trait_structs" ].push_back( trait );
                            structs.push_back( trait );
                        }
                    }
                    for ( const nlohmann::json& trait :
                          getDimensionTraits( typenames, pAction, pAction->get_dimension_traits() ) )
                    {
                        contextData[ "trait_structs" ].push_back( trait );
                        structs.push_back( trait );
                    }
                    if ( pAction->get_size_trait().has_value() )
                    {
                        const nlohmann::json& trait
                            = getSizeTrait( typenames, pAction, pAction->get_size_trait().value() );
                        contextData[ "trait_structs" ].push_back( trait );
                        structs.push_back( trait );
                    }
                    templateEngine.renderContext( contextData, os );
                }
            }
            {
                if ( Event* pEvent = dynamic_database_cast< Event >( pContext ) )
                {
                    VERIFY_RTE( !bFoundType );
                    bFoundType = true;
                    if ( auto opt = pEvent->get_inheritance_trait() )
                    {
                        for ( const nlohmann::json& trait : getInheritanceTraits( typenames, pEvent, opt.value() ) )
                        {
                            contextData[ "trait_structs" ].push_back( trait );
                            structs.push_back( trait );
                        }
                    }
                    for ( const nlohmann::json& trait :
                          getDimensionTraits( typenames, pEvent, pEvent->get_dimension_traits() ) )
                    {
                        contextData[ "trait_structs" ].push_back( trait );
                        structs.push_back( trait );
                    }
                    if ( pEvent->get_size_trait().has_value() )
                    {
                        const nlohmann::json& trait
                            = getSizeTrait( typenames, pEvent, pEvent->get_size_trait().value() );
                        contextData[ "trait_structs" ].push_back( trait );
                        structs.push_back( trait );
                    }
                    templateEngine.renderContext( contextData, os );
                }
            }
            {
                if ( Function* pFunction = dynamic_database_cast< Function >( pContext ) )
                {
                    VERIFY_RTE( !bFoundType );
                    bFoundType                             = true;
                    contextData[ "has_operation" ]         = true;
                    contextData[ "operation_return_type" ] = pFunction->get_return_type_trait()->get_str();
                    contextData[ "operation_parameters" ]  = pFunction->get_arguments_trait()->get_str();

                    if ( !pFunction->get_return_type_trait()->get_str().empty()
                         || !pFunction->get_arguments_trait()->get_str().empty() )
                    {
                        const nlohmann::json& trait
                            = getFunctionTraits( typenames, pFunction, pFunction->get_return_type_trait() );
                        contextData[ "trait_structs" ].push_back( trait );
                        structs.push_back( trait );
                    }

                    templateEngine.renderContext( contextData, os );
                }
            }
            {
                if ( Object* pObject = dynamic_database_cast< Object >( pContext ) )
                {
                    VERIFY_RTE( !bFoundType );
                    bFoundType = true;
                    if ( auto opt = pObject->get_inheritance_trait() )
                    {
                        for ( const nlohmann::json& trait : getInheritanceTraits( typenames, pObject, opt.value() ) )
                        {
                            contextData[ "trait_structs" ].push_back( trait );
                            structs.push_back( trait );
                        }
                    }
                    for ( const nlohmann::json& trait :
                          getDimensionTraits( typenames, pObject, pObject->get_dimension_traits() ) )
                    {
                        contextData[ "trait_structs" ].push_back( trait );
                        structs.push_back( trait );
                    }
                    templateEngine.renderContext( contextData, os );
                }
            }
            {
                if ( Link* pLink = dynamic_database_cast< Link >( pContext ) )
                {
                    VERIFY_RTE( !bFoundType );
                    bFoundType = true;
                    for ( const nlohmann::json& trait :
                          getInheritanceTraits( typenames, pLink, pLink->get_link_target() ) )
                    {
                        contextData[ "trait_structs" ].push_back( trait );
                        structs.push_back( trait );
                    }

                    templateEngine.renderContext( contextData, os );
                }
            }
            VERIFY_RTE( bFoundType );
        }
    }
};
