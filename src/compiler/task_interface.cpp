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

#include "database/model/InterfaceStage.hxx"

namespace mega::compiler
{
class Task_InterfaceTree : public BaseTask
{
public:
    Task_InterfaceTree( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
        : BaseTask( taskArguments )
        , m_sourceFilePath( sourceFilePath )
    {
    }

    using Name       = std::vector< std::string >;
    using NameResMap = std::map< Name, InterfaceStage::Interface::ContextGroup* >;

    Name fromParserID( const Name& currentName, const std::vector< InterfaceStage::Parser::Identifier* >& ids ) const
    {
        VERIFY_RTE( !ids.empty() );
        using namespace InterfaceStage;
        Name name = currentName;
        for( const Parser::Identifier* pID : ids )
        {
            name.push_back( pID->get_str() );
        }
        return name;
    }

    InterfaceStage::Interface::ContextGroup* findContextGroup( const Name& name, const NameResMap& namedContexts )
    {
        NameResMap::const_iterator iFind = namedContexts.find( name );
        if( iFind != namedContexts.end() )
        {
            return iFind->second;
        }
        return nullptr;
    }

    template < typename TParserContextDefType, typename TInterfaceContextType, typename ConstructorLambda,
               typename AggregateLambda >
    void constructOrAggregate( InterfaceStage::Database&              database,
                               InterfaceStage::Components::Component* pComponent,
                               InterfaceStage::Interface::Root*       pRoot,
                               TParserContextDefType*                 pContextDef,
                               const Name&                            currentName,
                               NameResMap&                            namedContexts,
                               const ConstructorLambda&               constructorFunctor,
                               const AggregateLambda&                 aggregateFunctor )
    {
        using namespace InterfaceStage;
        using namespace InterfaceStage::Interface;

        const Name             name                   = fromParserID( currentName, pContextDef->get_id()->get_ids() );
        TInterfaceContextType* pInterfaceContextGroup = nullptr;
        {
            if( Interface::ContextGroup* pContextGroup = findContextGroup( name, namedContexts ) )
            {
                pInterfaceContextGroup = db_cast< TInterfaceContextType >( pContextGroup );
                VERIFY_PARSER( pInterfaceContextGroup,
                               "IContext definition of identifier: " << toString( name ) << " with mixed types",
                               pContextDef->get_id() );
                aggregateFunctor( pInterfaceContextGroup, pContextDef );
            }
            else
            {
                Interface::ContextGroup* pParent = pRoot;
                if( name.size() > 1U )
                {
                    Name parentName = name;
                    parentName.pop_back();
                    pParent = findContextGroup( parentName, namedContexts );
                    VERIFY_PARSER(
                        pParent, "Failed to locate parent for: " << toString( name ), pContextDef->get_id() );
                }
                pInterfaceContextGroup = constructorFunctor( database, name.back(), pParent, pComponent, pContextDef );
                pParent->push_back_children( pInterfaceContextGroup );
                namedContexts.insert( std::make_pair( name, pInterfaceContextGroup ) );
            }
        }
        recurse( database, pComponent, pRoot, pInterfaceContextGroup, pContextDef, name, namedContexts );
    }

    struct isGlobalNamespace
    {
        bool operator()( InterfaceStage::Interface::ContextGroup* pParent )
        {
            using namespace InterfaceStage;
            using namespace InterfaceStage::Interface;

            if( Root* pParentContext = db_cast< Root >( pParent ) )
            {
                return true;
            }
            else if( Namespace* pParentNamespace = db_cast< Namespace >( pParent ) )
            {
                return isGlobalNamespace()( pParentNamespace->get_parent() );
            }
            else
            {
                return false;
            }
        }
    };

    void recurse( InterfaceStage::Database&                database,
                  InterfaceStage::Components::Component*   pComponent,
                  InterfaceStage::Interface::Root*         pRoot,
                  InterfaceStage::Interface::ContextGroup* pGroup,
                  InterfaceStage::Parser::ContextDef*      pContextDef,
                  const Name&                              currentName,
                  NameResMap&                              namedContexts )
    {
        using namespace InterfaceStage;
        using namespace InterfaceStage::Interface;

        VERIFY_RTE( pContextDef );

        for( Parser::Include* pInclude : pContextDef->get_includes() )
        {
            if( Parser::MegaIncludeNested* pNestedInclude = db_cast< Parser::MegaIncludeNested >( pInclude ) )
            {
                Parser::IncludeRoot* pIncludeRoot       = pNestedInclude->get_root();
                Parser::ContextDef*  pIncludeContextDef = pIncludeRoot->get_ast();
                // NOTE: use of ContextDef here instead of NamespaceDef due to how
                //  include root only has ContextDef*

                const Name name                   = fromParserID( currentName, pNestedInclude->get_id()->get_ids() );
                Namespace* pInterfaceContextGroup = nullptr;
                {
                    if( Interface::ContextGroup* pContextGroup = findContextGroup( name, namedContexts ) )
                    {
                        pInterfaceContextGroup = db_cast< Namespace >( pContextGroup );
                        VERIFY_PARSER( pInterfaceContextGroup,
                                       "IContext definition of identifier: " << toString( name ) << " with mixed types",
                                       pContextDef->get_id() );
                        pInterfaceContextGroup->push_back_namespace_defs( pIncludeContextDef );
                    }
                    else
                    {
                        Interface::ContextGroup* pParent = pRoot;
                        if( name.size() > 1U )
                        {
                            Name parentName = name;
                            parentName.pop_back();
                            pParent = findContextGroup( parentName, namedContexts );
                            VERIFY_PARSER(
                                pParent, "Failed to locate parent for: " << toString( name ), pContextDef->get_id() );
                        }

                        const bool bIsGlobalNamespace = isGlobalNamespace()( pParent );

                        pInterfaceContextGroup = database.construct< Namespace >(
                            Namespace::Args( IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ),
                                                             name.back(), pParent, pComponent ),
                                             bIsGlobalNamespace, { pIncludeContextDef } ) );
                        pParent->push_back_children( pInterfaceContextGroup );
                        namedContexts.insert( std::make_pair( name, pInterfaceContextGroup ) );
                    }
                }
                recurse( database, pComponent, pRoot, pInterfaceContextGroup, pIncludeContextDef, name, namedContexts );
            }
            else if( auto pInlineInclude = db_cast< Parser::MegaIncludeInline >( pInclude ) )
            {
                Parser::IncludeRoot* pIncludeRoot = pInlineInclude->get_root();
                recurse( database, pComponent, pRoot, pGroup, pIncludeRoot->get_ast(), currentName, namedContexts );
            }
        }

        for( Parser::ContextDef* pChildContext : pContextDef->get_children() )
        {
            if( auto pNamespaceDef = db_cast< Parser::NamespaceDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::NamespaceDef, Namespace >(
                    database, pComponent, pRoot, pNamespaceDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Components::Component* pComponent, Parser::ContextDef* pNamespaceDef ) -> Namespace*
                    {
                        const bool bIsGlobalNamespace = isGlobalNamespace()( pParent );
                        return database.construct< Namespace >(
                            Namespace::Args( IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name,
                                                             pParent, pComponent ),
                                             bIsGlobalNamespace, { pNamespaceDef } ) );
                    },
                    []( Namespace* pNamespace, Parser::ContextDef* pNamespaceDef )
                    { pNamespace->push_back_namespace_defs( pNamespaceDef ); } );
            }
            else if( auto pAbstractDef = db_cast< Parser::AbstractDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::AbstractDef, Abstract >(
                    database, pComponent, pRoot, pAbstractDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Components::Component* pComponent, Parser::AbstractDef* pAbstractDef ) -> Abstract*
                    {
                        return database.construct< Abstract >(
                            Abstract::Args( IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name,
                                                            pParent, pComponent ),
                                            { pAbstractDef } ) );
                    },
                    []( Abstract* pAbstract, Parser::AbstractDef* pAbstractDef )
                    { pAbstract->push_back_abstract_defs( pAbstractDef ); } );
            }
            else if( auto pActionDef = db_cast< Parser::ActionDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::ActionDef, Action >(
                    database, pComponent, pRoot, pActionDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Components::Component* pComponent, Parser::ActionDef* pActionDef ) -> Action*
                    {
                        return database.construct< Action >( Action::Args(
                            InvocationContext::Args( IContext::Args(
                                ContextGroup::Args( std::vector< IContext* >{} ), name, pParent, pComponent ) ),
                            { pActionDef } ) );
                    },
                    []( Action* pAction, Parser::ActionDef* pActionDef )
                    { pAction->push_back_action_defs( pActionDef ); } );
            }
            else if( auto pEventDef = db_cast< Parser::EventDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::EventDef, Event >(
                    database, pComponent, pRoot, pEventDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Components::Component* pComponent, Parser::EventDef* pEventDef ) -> Event*
                    {
                        return database.construct< Event >(
                            Event::Args( IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name,
                                                         pParent, pComponent ),
                                         { pEventDef } ) );
                    },
                    []( Event* pEvent, Parser::EventDef* pEventDef ) { pEvent->push_back_event_defs( pEventDef ); } );
            }
            else if( auto pInteruptDef = db_cast< Parser::InteruptDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::InteruptDef, Interupt >(
                    database, pComponent, pRoot, pInteruptDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Components::Component* pComponent, Parser::InteruptDef* pInteruptDef ) -> Interupt*
                    {
                        return database.construct< Interupt >( Interupt::Args(
                            InvocationContext::Args( IContext::Args(
                                ContextGroup::Args( std::vector< IContext* >{} ), name, pParent, pComponent ) ),
                            { pInteruptDef } ) );
                    },
                    []( Interupt* pBuffer, Parser::InteruptDef* pInteruptDef )
                    { pBuffer->push_back_interupt_defs( pInteruptDef ); } );
            }
            else if( auto pFunctionDef = db_cast< Parser::FunctionDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::FunctionDef, Function >(
                    database, pComponent, pRoot, pFunctionDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Components::Component* pComponent, Parser::FunctionDef* pFunctionDef ) -> Function*
                    {
                        return database.construct< Function >( Function::Args(
                            InvocationContext::Args( IContext::Args(
                                ContextGroup::Args( std::vector< IContext* >{} ), name, pParent, pComponent ) ),
                            { pFunctionDef } ) );
                    },
                    []( Function* pFunction, Parser::FunctionDef* pFunctionDef )
                    { pFunction->push_back_function_defs( pFunctionDef ); } );
            }
            else if( auto pObjectDef = db_cast< Parser::ObjectDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::ObjectDef, Object >(
                    database, pComponent, pRoot, pObjectDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Components::Component* pComponent, Parser::ObjectDef* pObjectDef ) -> Object*
                    {
                        return database.construct< Object >(
                            Object::Args( IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name,
                                                          pParent, pComponent ),
                                          { pObjectDef } ) );
                    },
                    []( Object* pObject, Parser::ObjectDef* pObjectDef )
                    { pObject->push_back_object_defs( pObjectDef ); } );
            }
            else if( auto pLinkInterfaceDef = db_cast< Parser::LinkInterfaceDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::LinkInterfaceDef, LinkInterface >(
                    database, pComponent, pRoot, pLinkInterfaceDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Components::Component*    pComponent,
                        Parser::LinkInterfaceDef* pLinkInterfaceDef ) -> LinkInterface*
                    {
                        return database.construct< LinkInterface >( LinkInterface::Args(
                            Link::Args( IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name, pParent,
                                                        pComponent ),
                                        { pLinkInterfaceDef } ) ) );
                    },
                    []( LinkInterface* pLinkInterface, Parser::LinkInterfaceDef* pLinkInterfaceDef )
                    { pLinkInterface->push_back_link_defs( pLinkInterfaceDef ); } );
            }
            else if( auto pLinkDef = db_cast< Parser::LinkDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::LinkDef, Link >(
                    database, pComponent, pRoot, pLinkDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Components::Component* pComponent, Parser::LinkDef* pLinkDef ) -> Link*
                    {
                        return database.construct< Link >(
                            Link::Args( IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name, pParent,
                                                        pComponent ),
                                        { pLinkDef } ) );
                    },
                    []( Link* pLink, Parser::LinkDef* pLinkDef ) { pLink->push_back_link_defs( pLinkDef ); } );
            }
            else
            {
                THROW_RTE( "Unknown context type" );
            }
        }
    }

    void collectDimensionTraits( InterfaceStage::Database& database, InterfaceStage::Interface::IContext* pContext,
                                 InterfaceStage::Parser::ContextDef*                        pDef,
                                 std::vector< InterfaceStage::Interface::DimensionTrait* >& dimensions )
    {
        using namespace InterfaceStage;

        for( auto pParserDim : pDef->get_dimensions() )
        {
            for( Parser::Dimension* pExistingDimension : dimensions )
            {
                VERIFY_PARSER( pParserDim->get_id()->get_str() != pExistingDimension->get_id()->get_str(),
                               "IContext has duplicate dimensions", pDef->get_id() );
            }
            dimensions.push_back( database.construct< Interface::DimensionTrait >(
                Interface::DimensionTrait::Args( pParserDim, pContext ) ) );
        }
    }

    template < typename TParserType >
    void collectInheritanceTrait( InterfaceStage::Database& database, TParserType* pContextDef,
                                  std::optional< InterfaceStage::Interface::InheritanceTrait* >& inheritance )
    {
        using namespace InterfaceStage;
        Parser::Inheritance*              pInheritanceDef = pContextDef->get_inheritance();
        const std::vector< std::string >& strings         = pInheritanceDef->get_strings();
        if( !strings.empty() )
        {
            VERIFY_PARSER( !inheritance.has_value(), "Duplicate inheritance specified", pContextDef->get_id() );
            inheritance = database.construct< Interface::InheritanceTrait >(
                Interface::InheritanceTrait::Args{ pInheritanceDef } );
        }
    }

    template < typename TParserType >
    void collectSizeTrait( InterfaceStage::Database& database, TParserType* pContextDef,
                           std::optional< InterfaceStage::Interface::SizeTrait* >& size )
    {
        using namespace InterfaceStage;
        Parser::Size*      pSizeDef = pContextDef->get_size();
        const std::string& str      = pSizeDef->get_str();
        if( !str.empty() )
        {
            VERIFY_PARSER( !size.has_value(), "Duplicate size specified", pContextDef->get_id() );
            size = database.construct< Interface::SizeTrait >( Interface::SizeTrait::Args{ pSizeDef } );
        }
    }

    template < typename TParserType >
    void collectSuccessorTrait( InterfaceStage::Database& database, TParserType* pContextDef,
                                std::optional< InterfaceStage::Interface::SuccessorTypeTrait* >& successor )
    {
        using namespace InterfaceStage;
        Parser::Successor* pSuccessor = pContextDef->get_successor();
        const std::string& str        = pSuccessor->get_str();
        if( !str.empty() )
        {
            VERIFY_PARSER( !successor.has_value(), "Duplicate successor specified", pContextDef->get_id() );
            successor = database.construct< Interface::SuccessorTypeTrait >(
                Interface::SuccessorTypeTrait::Args{ pSuccessor } );
        }
    }

    void onNamespace( InterfaceStage::Database& database, InterfaceStage::Interface::Namespace* pNamespace )
    {
        using namespace InterfaceStage;
        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        {
            for( Parser::ContextDef* pDef : pNamespace->get_namespace_defs() )
            {
                if( pNamespace->get_is_global() )
                {
                    VERIFY_PARSER( pDef->get_dimensions().empty(), "Global namespace has dimensions", pDef->get_id() );
                    VERIFY_PARSER(
                        pDef->get_dependencies().empty(), "Global namespace has dependencies", pDef->get_id() );
                }
                else
                {
                    collectDimensionTraits( database, pNamespace, pDef, dimensions );
                }

                VERIFY_PARSER( pDef->get_body().empty(), "Namespace has body", pDef->get_id() );
            }
        }

        pNamespace->set_dimension_traits( dimensions );
    }

    void onAbstract( InterfaceStage::Database& database, InterfaceStage::Interface::Abstract* pAbstract )
    {
        using namespace InterfaceStage;
        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        std::optional< Interface::InheritanceTrait* >             inheritance;
        std::optional< Interface::SizeTrait* >                    size;
        for( Parser::AbstractDef* pDef : pAbstract->get_abstract_defs() )
        {
            collectDimensionTraits( database, pAbstract, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
            collectSizeTrait( database, pDef, size );
        }

        pAbstract->set_dimension_traits( dimensions );
        pAbstract->set_inheritance_trait( inheritance );
        pAbstract->set_size_trait( size );
    }
    void onAction( InterfaceStage::Database& database, InterfaceStage::Interface::Action* pAction )
    {
        using namespace InterfaceStage;

        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        std::optional< Interface::InheritanceTrait* >             inheritance;
        std::optional< Interface::SizeTrait* >                    size;
        std::optional< Interface::SuccessorTypeTrait* >           successor;
        for( Parser::ActionDef* pDef : pAction->get_action_defs() )
        {
            collectDimensionTraits( database, pAction, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
            collectSizeTrait( database, pDef, size );
            collectSuccessorTrait( database, pDef, successor );
        }

        pAction->set_dimension_traits( dimensions );
        pAction->set_inheritance_trait( inheritance );
        pAction->set_size_trait( size );
        pAction->set_successor_trait( successor );
    }
    void onEvent( InterfaceStage::Database& database, InterfaceStage::Interface::Event* pEvent )
    {
        using namespace InterfaceStage;
        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        std::optional< Interface::InheritanceTrait* >             inheritance;
        std::optional< Interface::SizeTrait* >                    size;
        for( Parser::EventDef* pDef : pEvent->get_event_defs() )
        {
            collectDimensionTraits( database, pEvent, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
            collectSizeTrait( database, pDef, size );
        }

        pEvent->set_dimension_traits( dimensions );
        pEvent->set_inheritance_trait( inheritance );
        pEvent->set_size_trait( size );
    }
    void onInterupt( InterfaceStage::Database& database, InterfaceStage::Interface::Interupt* pInterupt )
    {
        using namespace InterfaceStage;

        Interface::EventTypeTrait*                      pEventsTrait = nullptr;
        std::optional< Interface::ArgumentListTrait* >  argumentsListTrait;
        std::optional< Interface::SuccessorTypeTrait* > successor;

        std::string strArguments, strSuccessor;
        for( Parser::InteruptDef* pDef : pInterupt->get_interupt_defs() )
        {
            Parser::ArgumentList* pArguments = pDef->get_argumentList();
            {
                if( !pEventsTrait )
                {
                    strArguments         = pArguments->get_str();
                    bool bIsArgumentList = false;

                    std::string strEvents;
                    {
                        // determine if the argument list specifies a manually handled event
                        // OR an interupt type list
                        if( strArguments.starts_with( "Event" ) )
                        {
                            bIsArgumentList = true;
                            auto iStart     = strArguments.find_first_of( '<' );
                            auto iEnd       = strArguments.find_last_of( '>' );
                            VERIFY_RTE_MSG( iStart < iEnd, "Invalid Events string: " << strArguments );
                            strEvents = strArguments.substr( iStart + 1, iEnd );
                        }
                        else
                        {
                            strEvents = strArguments;
                        }
                    }
                    pEventsTrait = database.construct< Interface::EventTypeTrait >(
                        Interface::EventTypeTrait::Args{ strEvents } );

                    if( bIsArgumentList )
                    {
                        argumentsListTrait = database.construct< Interface::ArgumentListTrait >(
                            Interface::ArgumentListTrait::Args{ pArguments } );
                    }
                }
                else
                {
                    VERIFY_PARSER(
                        strArguments == pArguments->get_str(), "Function arguments mismatch", pDef->get_id() );
                }
            }
            collectSuccessorTrait( database, pDef, successor );
        }

        VERIFY_PARSER( pEventsTrait, "Interupt missing events list", pInterupt->get_interupt_defs().front()->get_id() );

        pInterupt->set_events_trait( pEventsTrait );
        pInterupt->set_opt_arguments_trait( argumentsListTrait );
        pInterupt->set_successor_trait( successor );
    }
    void onFunction( InterfaceStage::Database& database, InterfaceStage::Interface::Function* pFunction )
    {
        using namespace InterfaceStage;

        Interface::ArgumentListTrait* pArgumentListTrait = nullptr;
        Interface::ReturnTypeTrait*   pReturnTypeTrait   = nullptr;

        std::string strArguments, strReturnType;
        for( Parser::FunctionDef* pDef : pFunction->get_function_defs() )
        {
            VERIFY_PARSER( pDef->get_dimensions().empty(), "Dimension has dimensions", pDef->get_id() );

            Parser::ArgumentList* pArguments = pDef->get_argumentList();
            // if ( !pArguments->get_str().empty() )
            {
                if( !pArgumentListTrait )
                {
                    pArgumentListTrait = database.construct< Interface::ArgumentListTrait >(
                        Interface::ArgumentListTrait::Args{ pArguments } );
                    strArguments = pArguments->get_str();
                }
                else
                {
                    VERIFY_PARSER(
                        strArguments == pArguments->get_str(), "Function arguments mismatch", pDef->get_id() );
                }
            }

            Parser::ReturnType* pReturnType = pDef->get_returnType();
            // if ( !pReturnType->get_str().empty() )
            {
                if( !pReturnTypeTrait )
                {
                    pReturnTypeTrait = database.construct< Interface::ReturnTypeTrait >(
                        Interface::ReturnTypeTrait::Args{ pReturnType } );
                    strReturnType = pReturnType->get_str();
                }
                else
                {
                    VERIFY_PARSER( strReturnType == pReturnType->get_str(), "Return type mismatch", pDef->get_id() );
                }
            }
        }

        VERIFY_PARSER(
            pArgumentListTrait, "Function missing argument list", pFunction->get_function_defs().front()->get_id() );
        VERIFY_PARSER(
            pReturnTypeTrait, "Function missing return type", pFunction->get_function_defs().front()->get_id() );

        pFunction->set_arguments_trait( pArgumentListTrait );
        pFunction->set_return_type_trait( pReturnTypeTrait );
    }
    void onObject( InterfaceStage::Database& database, InterfaceStage::Interface::Object* pObject )
    {
        using namespace InterfaceStage;
        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        std::optional< Interface::InheritanceTrait* >             inheritance;
        std::optional< Interface::SizeTrait* >                    size;
        for( Parser::ObjectDef* pDef : pObject->get_object_defs() )
        {
            collectDimensionTraits( database, pObject, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
            collectSizeTrait( database, pDef, size );
            VERIFY_PARSER( pDef->get_body().empty(), "Object has body", pDef->get_id() );
        }

        pObject->set_dimension_traits( dimensions );
        pObject->set_inheritance_trait( inheritance );
        pObject->set_size_trait( size );
    }
    void onLink( InterfaceStage::Database& database, InterfaceStage::Interface::Link* pLink )
    {
        using namespace InterfaceStage;

        for( Parser::LinkDef* pDef : pLink->get_link_defs() )
        {
            VERIFY_PARSER( pDef->get_dimensions().empty(), "Link has dimensions", pDef->get_id() );
            VERIFY_PARSER( pDef->get_body().empty(), "Link has body", pDef->get_id() );

            if( Parser::LinkInterfaceDef* pLinkInterfaceDef = db_cast< Parser::LinkInterfaceDef >( pDef ) )
            {
                Interface::LinkInterface* pLinkInterface = db_cast< Interface::LinkInterface >( pLink );
                VERIFY_PARSER( pLinkInterface, "Invalid link interface definition", pLinkInterfaceDef->get_id() );
                Interface::LinkTrait* pLinkTrait = database.construct< Interface::LinkTrait >(
                    Interface::LinkTrait::Args{ pLinkInterfaceDef->get_link_interface() } );
                // VERIFY_PARSER( !pLinkInterface->get_link_trait(), "Link interface has link trait", pDef->get_id() );
                pLinkInterface->set_link_trait( pLinkTrait );
            }

            Parser::Inheritance* pLinkTarget = pDef->get_target();

            using namespace InterfaceStage;
            const std::vector< std::string >& strings = pLinkTarget->get_strings();
            VERIFY_PARSER( !strings.empty(), "Invalid link target", pDef->get_id() );

            Interface::InheritanceTrait* pInheritance
                = database.construct< Interface::InheritanceTrait >( Interface::InheritanceTrait::Args{ pLinkTarget } );

            pLink->set_link_interface( pInheritance );
        }
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath treePath = m_environment.InterfaceStage_Tree( m_sourceFilePath );
        start( taskProgress, "Task_InterfaceTree", m_sourceFilePath.path(), treePath.path() );

        const task::FileHash parserStageASTHashCode
            = m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash, parserStageASTHashCode } );

        if( m_environment.restore( treePath, determinant ) )
        {
            m_environment.setBuildHashCode( treePath );
            cached( taskProgress );
            return;
        }

        using namespace InterfaceStage;
        Database database( m_environment, m_sourceFilePath );

        Parser::ObjectSourceRoot* pRoot = database.one< Parser::ObjectSourceRoot >( m_sourceFilePath );
        VERIFY_RTE( pRoot );

        Components::Component* pComponent = getComponent< Components::Component >( database, m_sourceFilePath );

        using namespace InterfaceStage::Interface;
        Root* pInterfaceRoot = database.construct< Root >( Root::Args{ ContextGroup::Args{ {} }, pRoot } );

        Name       currentName;
        NameResMap namedContexts;

        recurse( database, pComponent, pInterfaceRoot, pInterfaceRoot, pRoot->get_ast(), currentName, namedContexts );

        for( Interface::Namespace* pNamespace : database.many< Interface::Namespace >( m_sourceFilePath ) )
        {
            onNamespace( database, pNamespace );
        }
        for( Interface::Abstract* pAbstract : database.many< Interface::Abstract >( m_sourceFilePath ) )
        {
            onAbstract( database, pAbstract );
        }
        for( Interface::Action* pAction : database.many< Interface::Action >( m_sourceFilePath ) )
        {
            onAction( database, pAction );
        }
        for( Interface::Event* pEvent : database.many< Interface::Event >( m_sourceFilePath ) )
        {
            onEvent( database, pEvent );
        }
        for( Interface::Function* pFunction : database.many< Interface::Function >( m_sourceFilePath ) )
        {
            onFunction( database, pFunction );
        }
        for( Interface::Object* pObject : database.many< Interface::Object >( m_sourceFilePath ) )
        {
            onObject( database, pObject );
        }
        for( Interface::Link* pLink : database.many< Interface::Link >( m_sourceFilePath ) )
        {
            onLink( database, pLink );
        }
        for( Interface::Interupt* pInterupt : database.many< Interface::Interupt >( m_sourceFilePath ) )
        {
            onInterupt( database, pInterupt );
        }

        const task::FileHash fileHashCode = database.save_Tree_to_temp();
        m_environment.setBuildHashCode( treePath, fileHashCode );
        m_environment.temp_to_real( treePath );
        m_environment.stash( treePath, determinant );

        succeeded( taskProgress );
    }
    const mega::io::megaFilePath& m_sourceFilePath;
};

BaseTask::Ptr create_Task_InterfaceTree( const TaskArguments&          taskArguments,
                                         const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_InterfaceTree >( taskArguments, sourceFilePath );
}

} // namespace mega::compiler
