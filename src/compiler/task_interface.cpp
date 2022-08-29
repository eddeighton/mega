
#include "base_task.hpp"

#include "database/model/InterfaceStage.hxx"

namespace mega
{
namespace compiler
{

class Task_ObjectInterfaceGen : public BaseTask
{
public:
    Task_ObjectInterfaceGen( const TaskArguments& taskArguments, const mega::io::megaFilePath& sourceFilePath )
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
        for ( const Parser::Identifier* pID : ids )
        {
            name.push_back( pID->get_str() );
        }
        return name;
    }

    InterfaceStage::Interface::ContextGroup* findContextGroup( const Name& name, const NameResMap& namedContexts )
    {
        NameResMap::const_iterator iFind = namedContexts.find( name );
        if ( iFind != namedContexts.end() )
        {
            return iFind->second;
        }
        return nullptr;
    }

    template < typename TParserContextDefType, typename TInterfaceContextType, typename ConstructorLambda,
               typename AggregateLambda >
    void constructOrAggregate( InterfaceStage::Database&        database,
                               InterfaceStage::Interface::Root* pRoot,
                               TParserContextDefType*           pContextDef,
                               const Name&                      currentName,
                               NameResMap&                      namedContexts,
                               const ConstructorLambda&         constructorFunctor,
                               const AggregateLambda&           aggregateFunctor )
    {
        using namespace InterfaceStage;
        using namespace InterfaceStage::Interface;

        const Name             name                   = fromParserID( currentName, pContextDef->get_id()->get_ids() );
        TInterfaceContextType* pInterfaceContextGroup = nullptr;
        {
            if ( Interface::ContextGroup* pContextGroup = findContextGroup( name, namedContexts ) )
            {
                pInterfaceContextGroup = dynamic_database_cast< TInterfaceContextType >( pContextGroup );
                VERIFY_PARSER( pInterfaceContextGroup,
                               "IContext definition of identifier: " << name << " with mixed types",
                               pContextDef->get_id() );
                aggregateFunctor( pInterfaceContextGroup, pContextDef );
            }
            else
            {
                Interface::ContextGroup* pParent = pRoot;
                if ( name.size() > 1U )
                {
                    Name parentName = name;
                    parentName.pop_back();
                    pParent = findContextGroup( parentName, namedContexts );
                    VERIFY_PARSER( pParent, "Failed to locate parent for: " << name, pContextDef->get_id() );
                }
                pInterfaceContextGroup = constructorFunctor( database, name.back(), pParent, pContextDef );
                pParent->push_back_children( pInterfaceContextGroup );
                namedContexts.insert( std::make_pair( name, pInterfaceContextGroup ) );
            }
        }
        recurse( database, pRoot, pInterfaceContextGroup, pContextDef, name, namedContexts );
    }

    struct isGlobalNamespace
    {
        bool operator()( InterfaceStage::Interface::ContextGroup* pParent )
        {
            using namespace InterfaceStage;
            using namespace InterfaceStage::Interface;

            if ( Root* pParentContext = dynamic_database_cast< Root >( pParent ) )
            {
                return true;
            }
            else if ( Namespace* pParentNamespace = dynamic_database_cast< Namespace >( pParent ) )
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
                  InterfaceStage::Interface::Root*         pRoot,
                  InterfaceStage::Interface::ContextGroup* pGroup,
                  InterfaceStage::Parser::ContextDef*      pContextDef,
                  const Name&                              currentName,
                  NameResMap&                              namedContexts )
    {
        using namespace InterfaceStage;
        using namespace InterfaceStage::Interface;

        VERIFY_RTE( pContextDef );

        for ( Parser::Include* pInclude : pContextDef->get_includes() )
        {
            if ( Parser::MegaIncludeNested* pNestedInclude
                 = dynamic_database_cast< Parser::MegaIncludeNested >( pInclude ) )
            {
                Parser::IncludeRoot* pIncludeRoot       = pNestedInclude->get_root();
                Parser::ContextDef*  pIncludeContextDef = pIncludeRoot->get_ast();
                // NOTE: use of ContextDef here instead of NamespaceDef due to how
                //  include root only has ContextDef*

                const Name name                   = fromParserID( currentName, pNestedInclude->get_id()->get_ids() );
                Namespace* pInterfaceContextGroup = nullptr;
                {
                    if ( Interface::ContextGroup* pContextGroup = findContextGroup( name, namedContexts ) )
                    {
                        pInterfaceContextGroup = dynamic_database_cast< Namespace >( pContextGroup );
                        VERIFY_PARSER( pInterfaceContextGroup,
                                       "IContext definition of identifier: " << name << " with mixed types",
                                       pContextDef->get_id() );
                        pInterfaceContextGroup->push_back_namespace_defs( pIncludeContextDef );
                    }
                    else
                    {
                        Interface::ContextGroup* pParent = pRoot;
                        if ( name.size() > 1U )
                        {
                            Name parentName = name;
                            parentName.pop_back();
                            pParent = findContextGroup( parentName, namedContexts );
                            VERIFY_PARSER( pParent, "Failed to locate parent for: " << name, pContextDef->get_id() );
                        }

                        const bool bIsGlobalNamespace = isGlobalNamespace()( pParent );

                        pInterfaceContextGroup = database.construct< Namespace >( Namespace::Args(
                            IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name.back(), pParent ),
                            bIsGlobalNamespace, { pIncludeContextDef } ) );
                        pParent->push_back_children( pInterfaceContextGroup );
                        namedContexts.insert( std::make_pair( name, pInterfaceContextGroup ) );
                    }
                }
                recurse( database, pRoot, pInterfaceContextGroup, pIncludeContextDef, name, namedContexts );
            }
            else if ( Parser::MegaIncludeInline* pInlineInclude
                      = dynamic_database_cast< Parser::MegaIncludeInline >( pInclude ) )
            {
                Parser::IncludeRoot* pIncludeRoot = pInlineInclude->get_root();
                recurse( database, pRoot, pGroup, pIncludeRoot->get_ast(), currentName, namedContexts );
            }
        }

        for ( Parser::ContextDef* pChildContext : pContextDef->get_children() )
        {
            if ( Parser::NamespaceDef* pNamespaceDef = dynamic_database_cast< Parser::NamespaceDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::NamespaceDef, Namespace >(
                    database, pRoot, pNamespaceDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::ContextDef* pNamespaceDef ) -> Namespace*
                    {
                        const bool bIsGlobalNamespace = isGlobalNamespace()( pParent );
                        return database.construct< Namespace >( Namespace::Args(
                            IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name, pParent ),
                            bIsGlobalNamespace, { pNamespaceDef } ) );
                    },
                    []( Namespace* pNamespace, Parser::ContextDef* pNamespaceDef )
                    { pNamespace->push_back_namespace_defs( pNamespaceDef ); } );
            }
            else if ( Parser::AbstractDef* pAbstractDef
                      = dynamic_database_cast< Parser::AbstractDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::AbstractDef, Abstract >(
                    database, pRoot, pAbstractDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::AbstractDef* pAbstractDef ) -> Abstract*
                    {
                        return database.construct< Abstract >( Abstract::Args(
                            IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name, pParent ),
                            { pAbstractDef } ) );
                    },
                    []( Abstract* pAbstract, Parser::AbstractDef* pAbstractDef )
                    { pAbstract->push_back_abstract_defs( pAbstractDef ); } );
            }
            else if ( Parser::ActionDef* pActionDef = dynamic_database_cast< Parser::ActionDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::ActionDef, Action >(
                    database, pRoot, pActionDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::ActionDef* pActionDef ) -> Action*
                    {
                        return database.construct< Action >( Action::Args(
                            IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name, pParent ),
                            { pActionDef } ) );
                    },
                    []( Action* pAction, Parser::ActionDef* pActionDef )
                    { pAction->push_back_action_defs( pActionDef ); } );
            }
            else if ( Parser::EventDef* pEventDef = dynamic_database_cast< Parser::EventDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::EventDef, Event >(
                    database, pRoot, pEventDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::EventDef* pEventDef ) -> Event*
                    {
                        return database.construct< Event >( Event::Args(
                            IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name, pParent ),
                            { pEventDef } ) );
                    },
                    []( Event* pEvent, Parser::EventDef* pEventDef ) { pEvent->push_back_event_defs( pEventDef ); } );
            }
            else if ( Parser::FunctionDef* pFunctionDef
                      = dynamic_database_cast< Parser::FunctionDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::FunctionDef, Function >(
                    database, pRoot, pFunctionDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::FunctionDef* pFunctionDef ) -> Function*
                    {
                        return database.construct< Function >( Function::Args(
                            IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name, pParent ),
                            { pFunctionDef } ) );
                    },
                    []( Function* pFunction, Parser::FunctionDef* pFunctionDef )
                    { pFunction->push_back_function_defs( pFunctionDef ); } );
            }
            else if ( Parser::ObjectDef* pObjectDef = dynamic_database_cast< Parser::ObjectDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::ObjectDef, Object >(
                    database, pRoot, pObjectDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::ObjectDef* pObjectDef ) -> Object*
                    {
                        return database.construct< Object >( Object::Args(
                            IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name, pParent ),
                            { pObjectDef } ) );
                    },
                    []( Object* pObject, Parser::ObjectDef* pObjectDef )
                    { pObject->push_back_object_defs( pObjectDef ); } );
            }
            else if ( Parser::LinkInterfaceDef* pLinkInterfaceDef
                      = dynamic_database_cast< Parser::LinkInterfaceDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::LinkInterfaceDef, LinkInterface >(
                    database, pRoot, pLinkInterfaceDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::LinkInterfaceDef* pLinkInterfaceDef ) -> LinkInterface*
                    {
                        return database.construct< LinkInterface >( LinkInterface::Args(
                            IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name, pParent ),
                            { pLinkInterfaceDef } ) );
                    },
                    []( LinkInterface* pLinkInterface, Parser::LinkInterfaceDef* pLinkInterfaceDef )
                    { pLinkInterface->push_back_link_interface_defs( pLinkInterfaceDef ); } );
            }
            else if ( Parser::LinkDef* pLinkDef = dynamic_database_cast< Parser::LinkDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::LinkDef, Link >(
                    database, pRoot, pLinkDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::LinkDef* pLinkDef ) -> Link*
                    {
                        return database.construct< Link >( Link::Args(
                            IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name, pParent ),
                            { pLinkDef } ) );
                    },
                    []( Link* pLink, Parser::LinkDef* pLinkDef ) { pLink->push_back_link_defs( pLinkDef ); } );
            }
            else if ( Parser::BufferDef* pBufferDef = dynamic_database_cast< Parser::BufferDef >( pChildContext ) )
            {
                constructOrAggregate< Parser::BufferDef, Buffer >(
                    database, pRoot, pBufferDef, currentName, namedContexts,
                    []( Database& database, const std::string& name, ContextGroup* pParent,
                        Parser::BufferDef* pBufferDef ) -> Buffer*
                    {
                        return database.construct< Buffer >( Buffer::Args(
                            IContext::Args( ContextGroup::Args( std::vector< IContext* >{} ), name, pParent ),
                            { pBufferDef } ) );
                    },
                    []( Buffer* pBuffer, Parser::BufferDef* pBufferDef )
                    { pBuffer->push_back_buffer_defs( pBufferDef ); } );
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

        for ( Parser::Dimension* pParserDim : pDef->get_dimensions() )
        {
            for ( Parser::Dimension* pExistingDimension : dimensions )
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
        if ( !strings.empty() )
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
        if ( !str.empty() )
        {
            VERIFY_PARSER( !size.has_value(), "Duplicate size specified", pContextDef->get_id() );
            size = database.construct< Interface::SizeTrait >( Interface::SizeTrait::Args{ pSizeDef } );
        }
    }

    void onNamespace( InterfaceStage::Database& database, InterfaceStage::Interface::Namespace* pNamespace )
    {
        using namespace InterfaceStage;
        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        {
            for ( Parser::ContextDef* pDef : pNamespace->get_namespace_defs() )
            {
                if ( pNamespace->get_is_global() )
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
        for ( Parser::AbstractDef* pDef : pAbstract->get_abstract_defs() )
        {
            collectDimensionTraits( database, pAbstract, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
        }

        pAbstract->set_dimension_traits( dimensions );
        pAbstract->set_inheritance_trait( inheritance );
    }
    void onAction( InterfaceStage::Database& database, InterfaceStage::Interface::Action* pAction )
    {
        using namespace InterfaceStage;

        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        std::optional< Interface::InheritanceTrait* >             inheritance;
        std::optional< Interface::SizeTrait* >                    size;
        for ( Parser::ActionDef* pDef : pAction->get_action_defs() )
        {
            collectDimensionTraits( database, pAction, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
            collectSizeTrait( database, pDef, size );
        }

        pAction->set_dimension_traits( dimensions );
        pAction->set_inheritance_trait( inheritance );
        pAction->set_size_trait( size );
    }
    void onEvent( InterfaceStage::Database& database, InterfaceStage::Interface::Event* pEvent )
    {
        using namespace InterfaceStage;
        std::vector< InterfaceStage::Interface::DimensionTrait* > dimensions;
        std::optional< Interface::InheritanceTrait* >             inheritance;
        std::optional< Interface::SizeTrait* >                    size;
        for ( Parser::EventDef* pDef : pEvent->get_event_defs() )
        {
            collectDimensionTraits( database, pEvent, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
            collectSizeTrait( database, pDef, size );
        }

        pEvent->set_dimension_traits( dimensions );
        pEvent->set_inheritance_trait( inheritance );
        pEvent->set_size_trait( size );
    }
    void onFunction( InterfaceStage::Database& database, InterfaceStage::Interface::Function* pFunction )
    {
        using namespace InterfaceStage;

        Interface::ArgumentListTrait* pArgumentListTrait = nullptr;
        Interface::ReturnTypeTrait*   pReturnTypeTrait   = nullptr;

        std::string strArguments, strReturnType;
        for ( Parser::FunctionDef* pDef : pFunction->get_function_defs() )
        {
            VERIFY_PARSER( pDef->get_dimensions().empty(), "Dimension has dimensions", pDef->get_id() );

            Parser::ArgumentList* pArguments = pDef->get_argumentList();
            // if ( !pArguments->get_str().empty() )
            {
                if ( !pArgumentListTrait )
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
                if ( !pReturnTypeTrait )
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
        for ( Parser::ObjectDef* pDef : pObject->get_object_defs() )
        {
            collectDimensionTraits( database, pObject, pDef, dimensions );
            collectInheritanceTrait( database, pDef, inheritance );
            VERIFY_PARSER( pDef->get_body().empty(), "Object has body", pDef->get_id() );
        }

        pObject->set_dimension_traits( dimensions );
        pObject->set_inheritance_trait( inheritance );
    }
    void onLinkInterface( InterfaceStage::Database& database, InterfaceStage::Interface::LinkInterface* pLinkInterface )
    {
        using namespace InterfaceStage;

        for ( Parser::LinkInterfaceDef* pDef : pLinkInterface->get_link_interface_defs() )
        {
            VERIFY_PARSER( pDef->get_dimensions().empty(), "Link interface has dimensions", pDef->get_id() );
            VERIFY_PARSER( pDef->get_body().empty(), "Link interface has body", pDef->get_id() );

            Parser::LinkInterface* pLinkSpec = pDef->get_link_interface();

            Interface::LinkTrait* pLinkTrait
                = database.construct< Interface::LinkTrait >( Interface::LinkTrait::Args{ pLinkSpec } );

            pLinkInterface->set_link_trait( pLinkTrait );

            Parser::Inheritance* pLinkTarget = pDef->get_target();

            using namespace InterfaceStage;
            const std::vector< std::string >& strings = pLinkTarget->get_strings();
            VERIFY_PARSER( !strings.empty(), "Invalid link target", pDef->get_id() );

            Interface::InheritanceTrait* pInheritance
                = database.construct< Interface::InheritanceTrait >( Interface::InheritanceTrait::Args{ pLinkTarget } );

            pLinkInterface->set_link_target( pInheritance );
        }
    }
    void onLink( InterfaceStage::Database& database, InterfaceStage::Interface::Link* pLink )
    {
        using namespace InterfaceStage;

        for ( Parser::LinkDef* pDef : pLink->get_link_defs() )
        {
            VERIFY_PARSER( pDef->get_dimensions().empty(), "Link has dimensions", pDef->get_id() );
            VERIFY_PARSER( pDef->get_body().empty(), "Link has body", pDef->get_id() );

            Parser::Inheritance* pLinkTarget = pDef->get_target();

            using namespace InterfaceStage;
            const std::vector< std::string >& strings = pLinkTarget->get_strings();
            VERIFY_PARSER( !strings.empty(), "Invalid link target", pDef->get_id() );

            Interface::InheritanceTrait* pInheritance
                = database.construct< Interface::InheritanceTrait >( Interface::InheritanceTrait::Args{ pLinkTarget } );

            pLink->set_link_target( pInheritance );
        }
    }
    void onBuffer( InterfaceStage::Database& database, InterfaceStage::Interface::Buffer* pBuffer )
    {
        THROW_RTE( "Buffer NOT IMPLEMENTED!" );
    }

    virtual void run( mega::pipeline::Progress& taskProgress )
    {
        const mega::io::CompilationFilePath treePath = m_environment.InterfaceStage_Tree( m_sourceFilePath );
        start( taskProgress, "Task_ObjectInterfaceGen", m_sourceFilePath.path(), treePath.path() );

        const task::FileHash parserStageASTHashCode
            = m_environment.getBuildHashCode( m_environment.ParserStage_AST( m_sourceFilePath ) );

        const task::DeterminantHash determinant( { m_toolChain.toolChainHash, parserStageASTHashCode } );

        if ( m_environment.restore( treePath, determinant ) )
        {
            m_environment.setBuildHashCode( treePath );
            cached( taskProgress );
            return;
        }

        using namespace InterfaceStage;
        Database database( m_environment, m_sourceFilePath );

        Parser::ObjectSourceRoot* pRoot = database.one< Parser::ObjectSourceRoot >( m_sourceFilePath );
        VERIFY_RTE( pRoot );

        using namespace InterfaceStage::Interface;
        Root* pInterfaceRoot = database.construct< Root >( Root::Args{ ContextGroup::Args{ {} }, pRoot } );

        Name       currentName;
        NameResMap namedContexts;

        recurse( database, pInterfaceRoot, pInterfaceRoot, pRoot->get_ast(), currentName, namedContexts );

        for ( Interface::Namespace* pNamespace : database.many< Interface::Namespace >( m_sourceFilePath ) )
        {
            onNamespace( database, pNamespace );
        }
        for ( Interface::Abstract* pAbstract : database.many< Interface::Abstract >( m_sourceFilePath ) )
        {
            onAbstract( database, pAbstract );
        }
        for ( Interface::Action* pAction : database.many< Interface::Action >( m_sourceFilePath ) )
        {
            onAction( database, pAction );
        }
        for ( Interface::Event* pEvent : database.many< Interface::Event >( m_sourceFilePath ) )
        {
            onEvent( database, pEvent );
        }
        for ( Interface::Function* pFunction : database.many< Interface::Function >( m_sourceFilePath ) )
        {
            onFunction( database, pFunction );
        }
        for ( Interface::Object* pObject : database.many< Interface::Object >( m_sourceFilePath ) )
        {
            onObject( database, pObject );
        }
        for ( Interface::LinkInterface* pLinkInterface : database.many< Interface::LinkInterface >( m_sourceFilePath ) )
        {
            onLinkInterface( database, pLinkInterface );
        }
        for ( Interface::Link* pLink : database.many< Interface::Link >( m_sourceFilePath ) )
        {
            onLink( database, pLink );
        }
        for ( Interface::Buffer* pBuffer : database.many< Interface::Buffer >( m_sourceFilePath ) )
        {
            onBuffer( database, pBuffer );
        }

        const task::FileHash fileHashCode = database.save_Tree_to_temp();
        m_environment.setBuildHashCode( treePath, fileHashCode );
        m_environment.temp_to_real( treePath );
        m_environment.stash( treePath, determinant );

        succeeded( taskProgress );
    }
    const mega::io::megaFilePath& m_sourceFilePath;
};

BaseTask::Ptr create_Task_ObjectInterfaceGen( const TaskArguments&          taskArguments,
                                              const mega::io::megaFilePath& sourceFilePath )
{
    return std::make_unique< Task_ObjectInterfaceGen >( taskArguments, sourceFilePath );
}

} // namespace compiler
} // namespace mega
