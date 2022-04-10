
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
    // struct Opaque : public mega::io::Object
    Opaque::Opaque( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Opaque::Opaque( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo )
          , str( str )
    {
    }
    void Opaque::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void Opaque::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
        
    // struct Dimension : public mega::io::Object
    Dimension::Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , cppTypeName( loader )
    {
    }
    Dimension::Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& isConst, const std::string& identifier, const std::string& type, const std::string& test, data::Ptr< data::AST::Opaque > cppTypeName)
        :   mega::io::Object( objectInfo )
          , isConst( isConst )
          , identifier( identifier )
          , type( type )
          , test( test )
          , cppTypeName( cppTypeName )
    {
    }
    void Dimension::load( mega::io::Loader& loader )
    {
        loader.load( isConst );
        loader.load( identifier );
        loader.load( type );
        loader.load( test );
        loader.load( cppTypeName );
    }
    void Dimension::store( mega::io::Storer& storer ) const
    {
        storer.store( isConst );
        storer.store( identifier );
        storer.store( type );
        storer.store( test );
        storer.store( cppTypeName );
    }
        
    // struct Root : public mega::io::Object
    Root::Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_Context( loader )
    {
    }
    void Root::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Context );
    }
    void Root::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Context );
    }
        
    // struct Context : public mega::io::Object
    Context::Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    Context::Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& identifier, const std::string& body)
        :   mega::io::Object( objectInfo )
          , identifier( identifier )
          , body( body )
    {
    }
    void Context::load( mega::io::Loader& loader )
    {
        loader.load( identifier );
        loader.load( body );
    }
    void Context::store( mega::io::Storer& storer ) const
    {
        storer.store( identifier );
        storer.store( body );
    }
        
    // struct FoobarRoot : public mega::io::Object
    FoobarRoot::FoobarRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_TestRoot( loader )
    {
    }
    void FoobarRoot::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_TestRoot );
    }
    void FoobarRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_TestRoot );
    }
        
    // struct TestRoot : public mega::io::Object
    TestRoot::TestRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_Root( loader )
    {
    }
    void TestRoot::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Root );
    }
    void TestRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Root );
    }
        
}
namespace Body
{
}
namespace Extra
{
}
namespace Tree
{
    // struct Dimension : public mega::io::Object
    Dimension::Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
          , p_AST_Dimension( loader )
    {
    }
    Dimension::Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& more)
        :   mega::io::Object( objectInfo )
          , p_AST_Dimension( loader )
          , more( more )
    {
    }
    void Dimension::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Dimension );
        loader.load( more );
    }
    void Dimension::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Dimension );
        storer.store( more );
    }
        
    // struct Root : public mega::io::Object
    Root::Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    }
    void Root::load( mega::io::Loader& loader )
    {
    }
    void Root::store( mega::io::Storer& storer ) const
    {
    }
        
}
namespace DependencyAnalysis
{
}
namespace ObjectCompile
{
}
namespace ObjectAnalysis
{
}
namespace Tree
{
}
namespace Buffers
{
}
namespace Graph
{
}
namespace Invocations
{
}


mega::io::Object* Factory::create( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
{
    switch( objectInfo.getType() )
    {
        case 0: return new Components::Component( loader, objectInfo );
        case 1: return new AST::Opaque( loader, objectInfo );
        case 2: return new AST::Dimension( loader, objectInfo );
        case 3: return new AST::Root( loader, objectInfo );
        case 4: return new AST::Context( loader, objectInfo );
        case 5: return new AST::FoobarRoot( loader, objectInfo );
        case 6: return new AST::TestRoot( loader, objectInfo );
        case 7: return new Tree::Dimension( loader, objectInfo );
        case 8: return new Tree::Root( loader, objectInfo );
        default:
            THROW_RTE( "Unrecognised object type ID" );
    }
}

}
