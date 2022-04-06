
#include "database/model/data.hxx"


namespace data
{

namespace Components
{
    // struct Component : public mega::io::Object
    Component::Component( const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
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
    Opaque::Opaque( const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
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
    Dimension::Dimension( const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
    } 

    void Dimension::load( mega::io::Loader& loader )
    {
        loader.load( isConst );
        loader.load( identifier );
        loader.load( type );
    }
    void Dimension::store( mega::io::Storer& storer ) const
    {
        storer.store( isConst );
        storer.store( identifier );
        storer.store( type );
    }
        
    // struct Context : public mega::io::Object
    Context::Context( const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
        pBody_Context = nullptr;
    } 

    void Context::load( mega::io::Loader& loader )
    {
        loader.load( identifier );
    }
    void Context::store( mega::io::Storer& storer ) const
    {
        storer.store( identifier );
    }
        
    // struct Root : public mega::io::Object
    Root::Root( const mega::io::ObjectInfo& objectInfo )
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
namespace Body
{
    // struct Context : public mega::io::Object
    Context::Context( const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )
    {
        pAST_Context = nullptr;
    } 

    void Context::load( mega::io::Loader& loader )
    {
        loader.load( body );
    }
    void Context::store( mega::io::Storer& storer ) const
    {
        storer.store( body );
    }
        
}
namespace Tree
{
    // struct Root : public mega::io::Object
    Root::Root( const mega::io::ObjectInfo& objectInfo )
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

}
