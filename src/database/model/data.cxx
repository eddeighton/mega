
#include "database/model/data.hxx"


namespace data
{

namespace Components
{
    // struct Component : public mega::io::Object
    Component::Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Component::Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& name, const boost::filesystem::path& directory, const std::vector< boost::filesystem::path >& includeDirectories, const std::vector< boost::filesystem::path >& sourceFiles)
        :   mega::io::Object( objectInfo )
          , name( name )
          , directory( directory )
          , includeDirectories( includeDirectories )
          , sourceFiles( sourceFiles )
    {
    }
    void Component::load( mega::io::Loader& loader )
    {
        loader.load( name );
        loader.load( directory );
        loader.load( includeDirectories );
        loader.load( sourceFiles );
    }
    void Component::store( mega::io::Storer& storer ) const
    {
        storer.store( name );
        storer.store( directory );
        storer.store( includeDirectories );
        storer.store( sourceFiles );
    }
        
}
namespace AST
{
    // struct Identifier : public mega::io::Object
    Identifier::Identifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Identifier::Identifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , str( str )
    {
    }
    void Identifier::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void Identifier::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
        
    // struct ScopedIdentifier : public mega::io::Object
    ScopedIdentifier::ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    ScopedIdentifier::ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Identifier > >& ids)
        :   mega::io::Object( objectInfo )
          , ids( ids )
    {
    }
    void ScopedIdentifier::load( mega::io::Loader& loader )
    {
        loader.load( ids );
    }
    void ScopedIdentifier::store( mega::io::Storer& storer ) const
    {
        storer.store( ids );
    }
        
    // struct ArgumentList : public mega::io::Object
    ArgumentList::ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    ArgumentList::ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , str( str )
    {
    }
    void ArgumentList::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void ArgumentList::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
        
    // struct ReturnType : public mega::io::Object
    ReturnType::ReturnType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    ReturnType::ReturnType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , str( str )
    {
    }
    void ReturnType::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void ReturnType::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
        
    // struct Inheritance : public mega::io::Object
    Inheritance::Inheritance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Inheritance::Inheritance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< std::string >& strings)
        :   mega::io::Object( objectInfo )
          , strings( strings )
    {
    }
    void Inheritance::load( mega::io::Loader& loader )
    {
        loader.load( strings );
    }
    void Inheritance::store( mega::io::Storer& storer ) const
    {
        storer.store( strings );
    }
        
    // struct Size : public mega::io::Object
    Size::Size( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Size::Size( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , str( str )
    {
    }
    void Size::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void Size::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
        
    // struct Dimension : public mega::io::Object
    Dimension::Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , id( loader )
    {
    }
    Dimension::Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& isConst, data::Ptr< data::AST::Identifier > id, const std::string& type)
        :   mega::io::Object( objectInfo )
          , isConst( isConst )
          , id( id )
          , type( type )
    {
    }
    void Dimension::load( mega::io::Loader& loader )
    {
        loader.load( isConst );
        loader.load( id );
        loader.load( type );
    }
    void Dimension::store( mega::io::Storer& storer ) const
    {
        storer.store( isConst );
        storer.store( id );
        storer.store( type );
    }
        
    // struct Include : public mega::io::Object
    Include::Include( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , id( loader )
    {
    }
    Include::Include( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, data::Ptr< data::AST::Identifier > id)
        :   mega::io::Object( objectInfo )
          , id( id )
    {
    }
    void Include::load( mega::io::Loader& loader )
    {
        loader.load( id );
    }
    void Include::store( mega::io::Storer& storer ) const
    {
        storer.store( id );
    }
        
    // struct SystemInclude : public mega::io::Object
    SystemInclude::SystemInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
    {
    }
    SystemInclude::SystemInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
          , str( str )
    {
    }
    void SystemInclude::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Include );
        loader.load( str );
    }
    void SystemInclude::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Include );
        storer.store( str );
    }
        
    // struct MegaInclude : public mega::io::Object
    MegaInclude::MegaInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
          , root( loader )
    {
    }
    MegaInclude::MegaInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& megaSourceFilePath, data::Ptr< data::AST::IncludeRoot > root)
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
          , megaSourceFilePath( megaSourceFilePath )
          , root( root )
    {
    }
    void MegaInclude::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Include );
        loader.load( megaSourceFilePath );
        loader.load( root );
    }
    void MegaInclude::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Include );
        storer.store( megaSourceFilePath );
        storer.store( root );
    }
        
    // struct CPPInclude : public mega::io::Object
    CPPInclude::CPPInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
    {
    }
    CPPInclude::CPPInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& cppSourceFilePath)
        :   mega::io::Object( objectInfo )
          , p_AST_Include( loader )
          , cppSourceFilePath( cppSourceFilePath )
    {
    }
    void CPPInclude::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Include );
        loader.load( cppSourceFilePath );
    }
    void CPPInclude::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Include );
        storer.store( cppSourceFilePath );
    }
        
    // struct Dependency : public mega::io::Object
    Dependency::Dependency( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , id( loader )
    {
    }
    Dependency::Dependency( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, data::Ptr< data::AST::Identifier > id, const std::string& str)
        :   mega::io::Object( objectInfo )
          , id( id )
          , str( str )
    {
    }
    void Dependency::load( mega::io::Loader& loader )
    {
        loader.load( id );
        loader.load( str );
    }
    void Dependency::store( mega::io::Storer& storer ) const
    {
        storer.store( id );
        storer.store( str );
    }
        
    // struct ContextDef : public mega::io::Object
    ContextDef::ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    ContextDef::ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::ContextDef > >& children, const std::vector< data::Ptr< data::AST::Dimension > >& dimensions, const std::vector< data::Ptr< data::AST::Include > >& includes, const std::vector< data::Ptr< data::AST::Dependency > >& dependencies, const std::string& body)
        :   mega::io::Object( objectInfo )
          , children( children )
          , dimensions( dimensions )
          , includes( includes )
          , dependencies( dependencies )
          , body( body )
    {
    }
    void ContextDef::load( mega::io::Loader& loader )
    {
        loader.load( children );
        loader.load( dimensions );
        loader.load( includes );
        loader.load( dependencies );
        loader.load( body );
    }
    void ContextDef::store( mega::io::Storer& storer ) const
    {
        storer.store( children );
        storer.store( dimensions );
        storer.store( includes );
        storer.store( dependencies );
        storer.store( body );
    }
        
    // struct AbstractDef : public mega::io::Object
    AbstractDef::AbstractDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( loader )
    {
    }
    AbstractDef::AbstractDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, data::Ptr< data::AST::ScopedIdentifier > id)
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( id )
    {
    }
    void AbstractDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
        loader.load( id );
    }
    void AbstractDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
        storer.store( id );
    }
        
    // struct ActionDef : public mega::io::Object
    ActionDef::ActionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( loader )
          , size( loader )
          , inheritance( loader )
    {
    }
    ActionDef::ActionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, data::Ptr< data::AST::ScopedIdentifier > id, data::Ptr< data::AST::Size > size, data::Ptr< data::AST::Inheritance > inheritance)
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( id )
          , size( size )
          , inheritance( inheritance )
    {
    }
    void ActionDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
        loader.load( id );
        loader.load( size );
        loader.load( inheritance );
    }
    void ActionDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
        storer.store( id );
        storer.store( size );
        storer.store( inheritance );
    }
        
    // struct EventDef : public mega::io::Object
    EventDef::EventDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( loader )
          , size( loader )
          , inheritance( loader )
    {
    }
    EventDef::EventDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, data::Ptr< data::AST::ScopedIdentifier > id, data::Ptr< data::AST::Size > size, data::Ptr< data::AST::Inheritance > inheritance)
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( id )
          , size( size )
          , inheritance( inheritance )
    {
    }
    void EventDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
        loader.load( id );
        loader.load( size );
        loader.load( inheritance );
    }
    void EventDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
        storer.store( id );
        storer.store( size );
        storer.store( inheritance );
    }
        
    // struct FunctionDef : public mega::io::Object
    FunctionDef::FunctionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( loader )
          , argumentList( loader )
          , returnType( loader )
    {
    }
    FunctionDef::FunctionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, data::Ptr< data::AST::ScopedIdentifier > id, data::Ptr< data::AST::ArgumentList > argumentList, data::Ptr< data::AST::ReturnType > returnType)
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( id )
          , argumentList( argumentList )
          , returnType( returnType )
    {
    }
    void FunctionDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
        loader.load( id );
        loader.load( argumentList );
        loader.load( returnType );
    }
    void FunctionDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
        storer.store( id );
        storer.store( argumentList );
        storer.store( returnType );
    }
        
    // struct ObjectDef : public mega::io::Object
    ObjectDef::ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( loader )
          , size( loader )
          , inheritance( loader )
    {
    }
    ObjectDef::ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, data::Ptr< data::AST::ScopedIdentifier > id, data::Ptr< data::AST::Size > size, data::Ptr< data::AST::Inheritance > inheritance)
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( id )
          , size( size )
          , inheritance( inheritance )
    {
    }
    void ObjectDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
        loader.load( id );
        loader.load( size );
        loader.load( inheritance );
    }
    void ObjectDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
        storer.store( id );
        storer.store( size );
        storer.store( inheritance );
    }
        
    // struct LinkDef : public mega::io::Object
    LinkDef::LinkDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( loader )
          , size( loader )
          , inheritance( loader )
    {
    }
    LinkDef::LinkDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, data::Ptr< data::AST::ScopedIdentifier > id, data::Ptr< data::AST::Size > size, data::Ptr< data::AST::Inheritance > inheritance)
        :   mega::io::Object( objectInfo )
          , p_AST_ContextDef( loader )
          , id( id )
          , size( size )
          , inheritance( inheritance )
    {
    }
    void LinkDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_ContextDef );
        loader.load( id );
        loader.load( size );
        loader.load( inheritance );
    }
    void LinkDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_ContextDef );
        storer.store( id );
        storer.store( size );
        storer.store( inheritance );
    }
        
    // struct SourceRoot : public mega::io::Object
    SourceRoot::SourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , component( loader )
          , ast( loader )
    {
    }
    SourceRoot::SourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& sourceFile, data::Ptr< data::Components::Component > component, data::Ptr< data::AST::ContextDef > ast)
        :   mega::io::Object( objectInfo )
          , sourceFile( sourceFile )
          , component( component )
          , ast( ast )
    {
    }
    void SourceRoot::load( mega::io::Loader& loader )
    {
        loader.load( sourceFile );
        loader.load( component );
        loader.load( ast );
    }
    void SourceRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( sourceFile );
        storer.store( component );
        storer.store( ast );
    }
        
    // struct IncludeRoot : public mega::io::Object
    IncludeRoot::IncludeRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_SourceRoot( loader )
          , include( loader )
    {
    }
    IncludeRoot::IncludeRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, data::Ptr< data::AST::MegaInclude > include)
        :   mega::io::Object( objectInfo )
          , p_AST_SourceRoot( loader )
          , include( include )
    {
    }
    void IncludeRoot::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_SourceRoot );
        loader.load( include );
    }
    void IncludeRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_SourceRoot );
        storer.store( include );
    }
        
    // struct ObjectSourceRoot : public mega::io::Object
    ObjectSourceRoot::ObjectSourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_SourceRoot( loader )
    {
    }
    void ObjectSourceRoot::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_SourceRoot );
    }
    void ObjectSourceRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_SourceRoot );
    }
        
}
namespace Body
{
}


mega::io::Object* Factory::create( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
{
    switch( objectInfo.getType() )
    {
        case 0: return new Components::Component( loader, objectInfo );
        case 1: return new AST::Identifier( loader, objectInfo );
        case 2: return new AST::ScopedIdentifier( loader, objectInfo );
        case 3: return new AST::ArgumentList( loader, objectInfo );
        case 4: return new AST::ReturnType( loader, objectInfo );
        case 5: return new AST::Inheritance( loader, objectInfo );
        case 6: return new AST::Size( loader, objectInfo );
        case 7: return new AST::Dimension( loader, objectInfo );
        case 8: return new AST::Include( loader, objectInfo );
        case 9: return new AST::SystemInclude( loader, objectInfo );
        case 10: return new AST::MegaInclude( loader, objectInfo );
        case 11: return new AST::CPPInclude( loader, objectInfo );
        case 12: return new AST::Dependency( loader, objectInfo );
        case 13: return new AST::ContextDef( loader, objectInfo );
        case 14: return new AST::AbstractDef( loader, objectInfo );
        case 15: return new AST::ActionDef( loader, objectInfo );
        case 16: return new AST::EventDef( loader, objectInfo );
        case 17: return new AST::FunctionDef( loader, objectInfo );
        case 18: return new AST::ObjectDef( loader, objectInfo );
        case 19: return new AST::LinkDef( loader, objectInfo );
        case 20: return new AST::SourceRoot( loader, objectInfo );
        case 21: return new AST::IncludeRoot( loader, objectInfo );
        case 22: return new AST::ObjectSourceRoot( loader, objectInfo );
        default:
            THROW_RTE( "Unrecognised object type ID" );
    }
}

}
