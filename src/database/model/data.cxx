
#include "database/model/data.hxx"

#include "nlohmann/json.hpp"

namespace nlohmann {
    template <typename T>
    struct adl_serializer< std::optional<T> > 
    {
        static void to_json(json& j, const std::optional<T>& opt) 
        {
            if (!opt.has_value()) 
            {
                j = nullptr;
            } 
            else 
            {
                j = opt.value();
            }
        }
        static void from_json(const json& j, std::optional<T>& opt) 
        {
            if (j.is_null()) 
            {
                opt = std::optional<T>();
            } 
            else 
            {
                opt = j.get<T>();
            }
        }
    };
}

namespace data
{

namespace Components
{
    // struct Components_Component : public mega::io::Object
    Components_Component::Components_Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Components::Components_Component >( loader, this ) )    {
    }
    Components_Component::Components_Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::ComponentType& type, const std::string& name, const boost::filesystem::path& src_dir, const boost::filesystem::path& build_dir, const std::vector< std::string >& cpp_flags, const std::vector< std::string >& cpp_defines, const std::vector< boost::filesystem::path >& include_directories, const std::vector< mega::io::megaFilePath >& dependencies, const std::vector< mega::io::megaFilePath >& mega_source_files, const std::vector< mega::io::cppFilePath >& cpp_source_files)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Components::Components_Component >( loader, this ) )          , type( type )
          , name( name )
          , src_dir( src_dir )
          , build_dir( build_dir )
          , cpp_flags( cpp_flags )
          , cpp_defines( cpp_defines )
          , include_directories( include_directories )
          , dependencies( dependencies )
          , mega_source_files( mega_source_files )
          , cpp_source_files( cpp_source_files )
    {
    }
    bool Components_Component::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Components::Components_Component > >{ data::Ptr< data::Components::Components_Component >( loader, const_cast< Components_Component* >( this ) ) };
    }
    void Components_Component::set_inheritance_pointer()
    {
    }
    void Components_Component::load( mega::io::Loader& loader )
    {
        loader.load( type );
        loader.load( name );
        loader.load( src_dir );
        loader.load( build_dir );
        loader.load( cpp_flags );
        loader.load( cpp_defines );
        loader.load( include_directories );
        loader.load( dependencies );
        loader.load( mega_source_files );
        loader.load( cpp_source_files );
    }
    void Components_Component::store( mega::io::Storer& storer ) const
    {
        storer.store( type );
        storer.store( name );
        storer.store( src_dir );
        storer.store( build_dir );
        storer.store( cpp_flags );
        storer.store( cpp_defines );
        storer.store( include_directories );
        storer.store( dependencies );
        storer.store( mega_source_files );
        storer.store( cpp_source_files );
    }
    void Components_Component::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Components_Component" },
                { "filetype" , "Components" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "type", type } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "name", name } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "src_dir", src_dir } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "build_dir", build_dir } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "cpp_flags", cpp_flags } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "cpp_defines", cpp_defines } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "include_directories", include_directories } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dependencies", dependencies } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "mega_source_files", mega_source_files } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "cpp_source_files", cpp_source_files } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace AST
{
    // struct Parser_Identifier : public mega::io::Object
    Parser_Identifier::Parser_Identifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Identifier >( loader, this ) )    {
    }
    Parser_Identifier::Parser_Identifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Identifier >( loader, this ) )          , str( str )
    {
    }
    bool Parser_Identifier::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Identifier > >{ data::Ptr< data::AST::Parser_Identifier >( loader, const_cast< Parser_Identifier* >( this ) ) };
    }
    void Parser_Identifier::set_inheritance_pointer()
    {
    }
    void Parser_Identifier::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void Parser_Identifier::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
    void Parser_Identifier::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_Identifier" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_ScopedIdentifier : public mega::io::Object
    Parser_ScopedIdentifier::Parser_ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ScopedIdentifier >( loader, this ) )    {
    }
    Parser_ScopedIdentifier::Parser_ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_Identifier > >& ids, const std::string& source_file, const std::size_t& line_number)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ScopedIdentifier >( loader, this ) )          , ids( ids )
          , source_file( source_file )
          , line_number( line_number )
    {
    }
    bool Parser_ScopedIdentifier::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ScopedIdentifier > >{ data::Ptr< data::AST::Parser_ScopedIdentifier >( loader, const_cast< Parser_ScopedIdentifier* >( this ) ) };
    }
    void Parser_ScopedIdentifier::set_inheritance_pointer()
    {
    }
    void Parser_ScopedIdentifier::load( mega::io::Loader& loader )
    {
        loader.load( ids );
        loader.load( source_file );
        loader.load( line_number );
    }
    void Parser_ScopedIdentifier::store( mega::io::Storer& storer ) const
    {
        storer.store( ids );
        storer.store( source_file );
        storer.store( line_number );
    }
    void Parser_ScopedIdentifier::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_ScopedIdentifier" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "ids", ids } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_file", source_file } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "line_number", line_number } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_ArgumentList : public mega::io::Object
    Parser_ArgumentList::Parser_ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ArgumentList >( loader, this ) )    {
    }
    Parser_ArgumentList::Parser_ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ArgumentList >( loader, this ) )          , str( str )
    {
    }
    bool Parser_ArgumentList::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ArgumentList >, data::Ptr< data::Tree::Interface_ArgumentListTrait > >{ data::Ptr< data::AST::Parser_ArgumentList >( loader, const_cast< Parser_ArgumentList* >( this ) ) };
    }
    void Parser_ArgumentList::set_inheritance_pointer()
    {
    }
    void Parser_ArgumentList::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void Parser_ArgumentList::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
    void Parser_ArgumentList::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_ArgumentList" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_ReturnType : public mega::io::Object
    Parser_ReturnType::Parser_ReturnType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ReturnType >( loader, this ) )    {
    }
    Parser_ReturnType::Parser_ReturnType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ReturnType >( loader, this ) )          , str( str )
    {
    }
    bool Parser_ReturnType::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ReturnType >, data::Ptr< data::Tree::Interface_ReturnTypeTrait > >{ data::Ptr< data::AST::Parser_ReturnType >( loader, const_cast< Parser_ReturnType* >( this ) ) };
    }
    void Parser_ReturnType::set_inheritance_pointer()
    {
    }
    void Parser_ReturnType::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void Parser_ReturnType::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
    void Parser_ReturnType::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_ReturnType" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Inheritance : public mega::io::Object
    Parser_Inheritance::Parser_Inheritance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Inheritance >( loader, this ) )    {
    }
    Parser_Inheritance::Parser_Inheritance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< std::string >& strings)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Inheritance >( loader, this ) )          , strings( strings )
    {
    }
    bool Parser_Inheritance::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Inheritance >, data::Ptr< data::Tree::Interface_InheritanceTrait > >{ data::Ptr< data::AST::Parser_Inheritance >( loader, const_cast< Parser_Inheritance* >( this ) ) };
    }
    void Parser_Inheritance::set_inheritance_pointer()
    {
    }
    void Parser_Inheritance::load( mega::io::Loader& loader )
    {
        loader.load( strings );
    }
    void Parser_Inheritance::store( mega::io::Storer& storer ) const
    {
        storer.store( strings );
    }
    void Parser_Inheritance::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_Inheritance" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "strings", strings } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Size : public mega::io::Object
    Parser_Size::Parser_Size( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Size >( loader, this ) )    {
    }
    Parser_Size::Parser_Size( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Size >( loader, this ) )          , str( str )
    {
    }
    bool Parser_Size::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Size >, data::Ptr< data::Tree::Interface_SizeTrait > >{ data::Ptr< data::AST::Parser_Size >( loader, const_cast< Parser_Size* >( this ) ) };
    }
    void Parser_Size::set_inheritance_pointer()
    {
    }
    void Parser_Size::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void Parser_Size::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
    void Parser_Size::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_Size" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Link : public mega::io::Object
    Parser_Link::Parser_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Link >( loader, this ) )    {
    }
    Parser_Link::Parser_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::CardinalityRange& linker, const mega::CardinalityRange& linkee, const bool& derive_from, const bool& derive_to, const mega::Ownership& ownership)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Link >( loader, this ) )          , linker( linker )
          , linkee( linkee )
          , derive_from( derive_from )
          , derive_to( derive_to )
          , ownership( ownership )
    {
    }
    bool Parser_Link::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Link >, data::Ptr< data::Tree::Interface_LinkTrait > >{ data::Ptr< data::AST::Parser_Link >( loader, const_cast< Parser_Link* >( this ) ) };
    }
    void Parser_Link::set_inheritance_pointer()
    {
    }
    void Parser_Link::load( mega::io::Loader& loader )
    {
        loader.load( linker );
        loader.load( linkee );
        loader.load( derive_from );
        loader.load( derive_to );
        loader.load( ownership );
    }
    void Parser_Link::store( mega::io::Storer& storer ) const
    {
        storer.store( linker );
        storer.store( linkee );
        storer.store( derive_from );
        storer.store( derive_to );
        storer.store( ownership );
    }
    void Parser_Link::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_Link" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "linker", linker } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "linkee", linkee } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "derive_from", derive_from } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "derive_to", derive_to } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "ownership", ownership } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Dimension : public mega::io::Object
    Parser_Dimension::Parser_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Dimension >( loader, this ) )          , id( loader )
    {
    }
    Parser_Dimension::Parser_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& isConst, const data::Ptr< data::AST::Parser_Identifier >& id, const std::string& type)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Dimension >( loader, this ) )          , isConst( isConst )
          , id( id )
          , type( type )
    {
    }
    bool Parser_Dimension::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Dimension >, data::Ptr< data::Tree::Interface_DimensionTrait > >{ data::Ptr< data::AST::Parser_Dimension >( loader, const_cast< Parser_Dimension* >( this ) ) };
    }
    void Parser_Dimension::set_inheritance_pointer()
    {
    }
    void Parser_Dimension::load( mega::io::Loader& loader )
    {
        loader.load( isConst );
        loader.load( id );
        loader.load( type );
    }
    void Parser_Dimension::store( mega::io::Storer& storer ) const
    {
        storer.store( isConst );
        storer.store( id );
        storer.store( type );
    }
    void Parser_Dimension::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_Dimension" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "isConst", isConst } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "type", type } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Include : public mega::io::Object
    Parser_Include::Parser_Include( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Include >( loader, this ) )    {
    }
    bool Parser_Include::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > >{ data::Ptr< data::AST::Parser_Include >( loader, const_cast< Parser_Include* >( this ) ) };
    }
    void Parser_Include::set_inheritance_pointer()
    {
    }
    void Parser_Include::load( mega::io::Loader& loader )
    {
    }
    void Parser_Include::store( mega::io::Storer& storer ) const
    {
    }
    void Parser_Include::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_Include" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Parser_SystemInclude : public mega::io::Object
    Parser_SystemInclude::Parser_SystemInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_SystemInclude >( loader, this ) )          , p_AST_Parser_Include( loader )
    {
    }
    Parser_SystemInclude::Parser_SystemInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_SystemInclude >( loader, this ) )          , p_AST_Parser_Include( loader )
          , str( str )
    {
    }
    bool Parser_SystemInclude::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > >{ data::Ptr< data::AST::Parser_SystemInclude >( loader, const_cast< Parser_SystemInclude* >( this ) ) };
    }
    void Parser_SystemInclude::set_inheritance_pointer()
    {
        p_AST_Parser_Include->m_inheritance = data::Ptr< data::AST::Parser_SystemInclude >( p_AST_Parser_Include, this );
    }
    void Parser_SystemInclude::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_Include );
        loader.load( str );
    }
    void Parser_SystemInclude::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_Include );
        storer.store( str );
    }
    void Parser_SystemInclude::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_SystemInclude" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_MegaInclude : public mega::io::Object
    Parser_MegaInclude::Parser_MegaInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_MegaInclude >( loader, this ) )          , p_AST_Parser_Include( loader )
          , root( loader )
    {
    }
    Parser_MegaInclude::Parser_MegaInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& megaSourceFilePath)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_MegaInclude >( loader, this ) )          , p_AST_Parser_Include( loader )
          , megaSourceFilePath( megaSourceFilePath )
    {
    }
    bool Parser_MegaInclude::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > >{ data::Ptr< data::AST::Parser_MegaInclude >( loader, const_cast< Parser_MegaInclude* >( this ) ) };
    }
    void Parser_MegaInclude::set_inheritance_pointer()
    {
        p_AST_Parser_Include->m_inheritance = data::Ptr< data::AST::Parser_MegaInclude >( p_AST_Parser_Include, this );
    }
    void Parser_MegaInclude::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_Include );
        loader.load( megaSourceFilePath );
        loader.load( root );
    }
    void Parser_MegaInclude::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_Include );
        storer.store( megaSourceFilePath );
        VERIFY_RTE_MSG( root.has_value(), "AST::Parser_MegaInclude.root has NOT been set" );
        storer.store( root );
    }
    void Parser_MegaInclude::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_MegaInclude" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "megaSourceFilePath", megaSourceFilePath } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "root", root.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_MegaIncludeInline : public mega::io::Object
    Parser_MegaIncludeInline::Parser_MegaIncludeInline( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_MegaIncludeInline >( loader, this ) )          , p_AST_Parser_MegaInclude( loader )
    {
    }
    bool Parser_MegaIncludeInline::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > >{ data::Ptr< data::AST::Parser_MegaIncludeInline >( loader, const_cast< Parser_MegaIncludeInline* >( this ) ) };
    }
    void Parser_MegaIncludeInline::set_inheritance_pointer()
    {
        p_AST_Parser_MegaInclude->m_inheritance = data::Ptr< data::AST::Parser_MegaIncludeInline >( p_AST_Parser_MegaInclude, this );
    }
    void Parser_MegaIncludeInline::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_MegaInclude );
    }
    void Parser_MegaIncludeInline::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_MegaInclude );
    }
    void Parser_MegaIncludeInline::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_MegaIncludeInline" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Parser_MegaIncludeNested : public mega::io::Object
    Parser_MegaIncludeNested::Parser_MegaIncludeNested( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_MegaIncludeNested >( loader, this ) )          , p_AST_Parser_MegaInclude( loader )
          , id( loader )
    {
    }
    Parser_MegaIncludeNested::Parser_MegaIncludeNested( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_ScopedIdentifier >& id)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_MegaIncludeNested >( loader, this ) )          , p_AST_Parser_MegaInclude( loader )
          , id( id )
    {
    }
    bool Parser_MegaIncludeNested::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > >{ data::Ptr< data::AST::Parser_MegaIncludeNested >( loader, const_cast< Parser_MegaIncludeNested* >( this ) ) };
    }
    void Parser_MegaIncludeNested::set_inheritance_pointer()
    {
        p_AST_Parser_MegaInclude->m_inheritance = data::Ptr< data::AST::Parser_MegaIncludeNested >( p_AST_Parser_MegaInclude, this );
    }
    void Parser_MegaIncludeNested::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_MegaInclude );
        loader.load( id );
    }
    void Parser_MegaIncludeNested::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_MegaInclude );
        storer.store( id );
    }
    void Parser_MegaIncludeNested::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_MegaIncludeNested" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_CPPInclude : public mega::io::Object
    Parser_CPPInclude::Parser_CPPInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_CPPInclude >( loader, this ) )          , p_AST_Parser_Include( loader )
    {
    }
    Parser_CPPInclude::Parser_CPPInclude( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& cppSourceFilePath)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_CPPInclude >( loader, this ) )          , p_AST_Parser_Include( loader )
          , cppSourceFilePath( cppSourceFilePath )
    {
    }
    bool Parser_CPPInclude::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Include >, data::Ptr< data::AST::Parser_SystemInclude >, data::Ptr< data::AST::Parser_MegaInclude >, data::Ptr< data::AST::Parser_MegaIncludeInline >, data::Ptr< data::AST::Parser_MegaIncludeNested >, data::Ptr< data::AST::Parser_CPPInclude > >{ data::Ptr< data::AST::Parser_CPPInclude >( loader, const_cast< Parser_CPPInclude* >( this ) ) };
    }
    void Parser_CPPInclude::set_inheritance_pointer()
    {
        p_AST_Parser_Include->m_inheritance = data::Ptr< data::AST::Parser_CPPInclude >( p_AST_Parser_Include, this );
    }
    void Parser_CPPInclude::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_Include );
        loader.load( cppSourceFilePath );
    }
    void Parser_CPPInclude::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_Include );
        storer.store( cppSourceFilePath );
    }
    void Parser_CPPInclude::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_CPPInclude" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "cppSourceFilePath", cppSourceFilePath } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Dependency : public mega::io::Object
    Parser_Dependency::Parser_Dependency( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Dependency >( loader, this ) )    {
    }
    Parser_Dependency::Parser_Dependency( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& str)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Dependency >( loader, this ) )          , str( str )
    {
    }
    bool Parser_Dependency::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Dependency > >{ data::Ptr< data::AST::Parser_Dependency >( loader, const_cast< Parser_Dependency* >( this ) ) };
    }
    void Parser_Dependency::set_inheritance_pointer()
    {
    }
    void Parser_Dependency::load( mega::io::Loader& loader )
    {
        loader.load( str );
    }
    void Parser_Dependency::store( mega::io::Storer& storer ) const
    {
        storer.store( str );
    }
    void Parser_Dependency::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_Dependency" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_ContextDef : public mega::io::Object
    Parser_ContextDef::Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ContextDef >( loader, this ) )          , p_Body_Parser_ContextDef( loader )
          , id( loader )
    {
    }
    Parser_ContextDef::Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_ScopedIdentifier >& id, const std::vector< data::Ptr< data::AST::Parser_ContextDef > >& children, const std::vector< data::Ptr< data::AST::Parser_Dimension > >& dimensions, const std::vector< data::Ptr< data::AST::Parser_Include > >& includes, const std::vector< data::Ptr< data::AST::Parser_Dependency > >& dependencies)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ContextDef >( loader, this ) )          , p_Body_Parser_ContextDef( loader )
          , id( id )
          , children( children )
          , dimensions( dimensions )
          , includes( includes )
          , dependencies( dependencies )
    {
    }
    bool Parser_ContextDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > >{ data::Ptr< data::AST::Parser_ContextDef >( loader, const_cast< Parser_ContextDef* >( this ) ) };
    }
    void Parser_ContextDef::set_inheritance_pointer()
    {
    }
    void Parser_ContextDef::load( mega::io::Loader& loader )
    {
        loader.load( p_Body_Parser_ContextDef );
        loader.load( id );
        loader.load( children );
        loader.load( dimensions );
        loader.load( includes );
        loader.load( dependencies );
    }
    void Parser_ContextDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Body_Parser_ContextDef );
        storer.store( id );
        storer.store( children );
        storer.store( dimensions );
        storer.store( includes );
        storer.store( dependencies );
    }
    void Parser_ContextDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_ContextDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "children", children } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "includes", includes } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dependencies", dependencies } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_NamespaceDef : public mega::io::Object
    Parser_NamespaceDef::Parser_NamespaceDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_NamespaceDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
    {
    }
    bool Parser_NamespaceDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > >{ data::Ptr< data::AST::Parser_NamespaceDef >( loader, const_cast< Parser_NamespaceDef* >( this ) ) };
    }
    void Parser_NamespaceDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_NamespaceDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_NamespaceDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
    }
    void Parser_NamespaceDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
    }
    void Parser_NamespaceDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_NamespaceDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Parser_AbstractDef : public mega::io::Object
    Parser_AbstractDef::Parser_AbstractDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_AbstractDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , inheritance( loader )
    {
    }
    Parser_AbstractDef::Parser_AbstractDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Inheritance >& inheritance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_AbstractDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , inheritance( inheritance )
    {
    }
    bool Parser_AbstractDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > >{ data::Ptr< data::AST::Parser_AbstractDef >( loader, const_cast< Parser_AbstractDef* >( this ) ) };
    }
    void Parser_AbstractDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_AbstractDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_AbstractDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
        loader.load( inheritance );
    }
    void Parser_AbstractDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
        storer.store( inheritance );
    }
    void Parser_AbstractDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_AbstractDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_ActionDef : public mega::io::Object
    Parser_ActionDef::Parser_ActionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ActionDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , size( loader )
          , inheritance( loader )
    {
    }
    Parser_ActionDef::Parser_ActionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Size >& size, const data::Ptr< data::AST::Parser_Inheritance >& inheritance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ActionDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , size( size )
          , inheritance( inheritance )
    {
    }
    bool Parser_ActionDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > >{ data::Ptr< data::AST::Parser_ActionDef >( loader, const_cast< Parser_ActionDef* >( this ) ) };
    }
    void Parser_ActionDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_ActionDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_ActionDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
        loader.load( size );
        loader.load( inheritance );
    }
    void Parser_ActionDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
        storer.store( size );
        storer.store( inheritance );
    }
    void Parser_ActionDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_ActionDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "size", size } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_EventDef : public mega::io::Object
    Parser_EventDef::Parser_EventDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_EventDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , size( loader )
          , inheritance( loader )
    {
    }
    Parser_EventDef::Parser_EventDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Size >& size, const data::Ptr< data::AST::Parser_Inheritance >& inheritance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_EventDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , size( size )
          , inheritance( inheritance )
    {
    }
    bool Parser_EventDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > >{ data::Ptr< data::AST::Parser_EventDef >( loader, const_cast< Parser_EventDef* >( this ) ) };
    }
    void Parser_EventDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_EventDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_EventDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
        loader.load( size );
        loader.load( inheritance );
    }
    void Parser_EventDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
        storer.store( size );
        storer.store( inheritance );
    }
    void Parser_EventDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_EventDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "size", size } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_FunctionDef : public mega::io::Object
    Parser_FunctionDef::Parser_FunctionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_FunctionDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , argumentList( loader )
          , returnType( loader )
    {
    }
    Parser_FunctionDef::Parser_FunctionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_ArgumentList >& argumentList, const data::Ptr< data::AST::Parser_ReturnType >& returnType)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_FunctionDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , argumentList( argumentList )
          , returnType( returnType )
    {
    }
    bool Parser_FunctionDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > >{ data::Ptr< data::AST::Parser_FunctionDef >( loader, const_cast< Parser_FunctionDef* >( this ) ) };
    }
    void Parser_FunctionDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_FunctionDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_FunctionDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
        loader.load( argumentList );
        loader.load( returnType );
    }
    void Parser_FunctionDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
        storer.store( argumentList );
        storer.store( returnType );
    }
    void Parser_FunctionDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_FunctionDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "argumentList", argumentList } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "returnType", returnType } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_ObjectDef : public mega::io::Object
    Parser_ObjectDef::Parser_ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ObjectDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , inheritance( loader )
    {
    }
    Parser_ObjectDef::Parser_ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Inheritance >& inheritance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ObjectDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , inheritance( inheritance )
    {
    }
    bool Parser_ObjectDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > >{ data::Ptr< data::AST::Parser_ObjectDef >( loader, const_cast< Parser_ObjectDef* >( this ) ) };
    }
    void Parser_ObjectDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_ObjectDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_ObjectDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
        loader.load( inheritance );
    }
    void Parser_ObjectDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
        storer.store( inheritance );
    }
    void Parser_ObjectDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_ObjectDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_LinkDef : public mega::io::Object
    Parser_LinkDef::Parser_LinkDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_LinkDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , link( loader )
          , target( loader )
    {
    }
    Parser_LinkDef::Parser_LinkDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Link >& link, const data::Ptr< data::AST::Parser_Inheritance >& target)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_LinkDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , link( link )
          , target( target )
    {
    }
    bool Parser_LinkDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > >{ data::Ptr< data::AST::Parser_LinkDef >( loader, const_cast< Parser_LinkDef* >( this ) ) };
    }
    void Parser_LinkDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_LinkDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_LinkDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
        loader.load( link );
        loader.load( target );
    }
    void Parser_LinkDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
        storer.store( link );
        storer.store( target );
    }
    void Parser_LinkDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_LinkDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "link", link } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "target", target } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_TableDef : public mega::io::Object
    Parser_TableDef::Parser_TableDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_TableDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , inheritance( loader )
    {
    }
    Parser_TableDef::Parser_TableDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Inheritance >& inheritance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_TableDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , inheritance( inheritance )
    {
    }
    bool Parser_TableDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ContextDef >, data::Ptr< data::AST::Parser_NamespaceDef >, data::Ptr< data::AST::Parser_AbstractDef >, data::Ptr< data::AST::Parser_ActionDef >, data::Ptr< data::AST::Parser_EventDef >, data::Ptr< data::AST::Parser_FunctionDef >, data::Ptr< data::AST::Parser_ObjectDef >, data::Ptr< data::AST::Parser_LinkDef >, data::Ptr< data::AST::Parser_TableDef > >{ data::Ptr< data::AST::Parser_TableDef >( loader, const_cast< Parser_TableDef* >( this ) ) };
    }
    void Parser_TableDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_TableDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_TableDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
        loader.load( inheritance );
    }
    void Parser_TableDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
        storer.store( inheritance );
    }
    void Parser_TableDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_TableDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_SourceRoot : public mega::io::Object
    Parser_SourceRoot::Parser_SourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_SourceRoot >( loader, this ) )          , component( loader )
          , ast( loader )
    {
    }
    Parser_SourceRoot::Parser_SourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& sourceFile, const data::Ptr< data::Components::Components_Component >& component, const data::Ptr< data::AST::Parser_ContextDef >& ast)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_SourceRoot >( loader, this ) )          , sourceFile( sourceFile )
          , component( component )
          , ast( ast )
    {
    }
    bool Parser_SourceRoot::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_SourceRoot >, data::Ptr< data::AST::Parser_IncludeRoot >, data::Ptr< data::AST::Parser_ObjectSourceRoot > >{ data::Ptr< data::AST::Parser_SourceRoot >( loader, const_cast< Parser_SourceRoot* >( this ) ) };
    }
    void Parser_SourceRoot::set_inheritance_pointer()
    {
    }
    void Parser_SourceRoot::load( mega::io::Loader& loader )
    {
        loader.load( sourceFile );
        loader.load( component );
        loader.load( ast );
    }
    void Parser_SourceRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( sourceFile );
        storer.store( component );
        storer.store( ast );
    }
    void Parser_SourceRoot::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_SourceRoot" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "sourceFile", sourceFile } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "component", component } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "ast", ast } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_IncludeRoot : public mega::io::Object
    Parser_IncludeRoot::Parser_IncludeRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_IncludeRoot >( loader, this ) )          , p_AST_Parser_SourceRoot( loader )
    {
    }
    bool Parser_IncludeRoot::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_SourceRoot >, data::Ptr< data::AST::Parser_IncludeRoot >, data::Ptr< data::AST::Parser_ObjectSourceRoot > >{ data::Ptr< data::AST::Parser_IncludeRoot >( loader, const_cast< Parser_IncludeRoot* >( this ) ) };
    }
    void Parser_IncludeRoot::set_inheritance_pointer()
    {
        p_AST_Parser_SourceRoot->m_inheritance = data::Ptr< data::AST::Parser_IncludeRoot >( p_AST_Parser_SourceRoot, this );
    }
    void Parser_IncludeRoot::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_SourceRoot );
    }
    void Parser_IncludeRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_SourceRoot );
    }
    void Parser_IncludeRoot::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_IncludeRoot" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Parser_ObjectSourceRoot : public mega::io::Object
    Parser_ObjectSourceRoot::Parser_ObjectSourceRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ObjectSourceRoot >( loader, this ) )          , p_AST_Parser_SourceRoot( loader )
    {
    }
    bool Parser_ObjectSourceRoot::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_SourceRoot >, data::Ptr< data::AST::Parser_IncludeRoot >, data::Ptr< data::AST::Parser_ObjectSourceRoot > >{ data::Ptr< data::AST::Parser_ObjectSourceRoot >( loader, const_cast< Parser_ObjectSourceRoot* >( this ) ) };
    }
    void Parser_ObjectSourceRoot::set_inheritance_pointer()
    {
        p_AST_Parser_SourceRoot->m_inheritance = data::Ptr< data::AST::Parser_ObjectSourceRoot >( p_AST_Parser_SourceRoot, this );
    }
    void Parser_ObjectSourceRoot::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_SourceRoot );
    }
    void Parser_ObjectSourceRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_SourceRoot );
    }
    void Parser_ObjectSourceRoot::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_ObjectSourceRoot" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
}
namespace Body
{
    // struct Parser_ContextDef : public mega::io::Object
    Parser_ContextDef::Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )    {
    }
    Parser_ContextDef::Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& body)
        :   mega::io::Object( objectInfo )          , body( body )
    {
    }
    bool Parser_ContextDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Parser_ContextDef::set_inheritance_pointer()
    {
    }
    void Parser_ContextDef::load( mega::io::Loader& loader )
    {
        loader.load( body );
    }
    void Parser_ContextDef::store( mega::io::Storer& storer ) const
    {
        storer.store( body );
    }
    void Parser_ContextDef::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Parser_ContextDef" },
                { "filetype" , "Body" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "body", body } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace Tree
{
    // struct Interface_DimensionTrait : public mega::io::Object
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_DimensionTrait >( loader, this ) )          , p_AST_Parser_Dimension( loader )
          , p_PerSourceSymbols_Interface_DimensionTrait( loader )
          , p_Concrete_Interface_DimensionTrait( loader )
          , p_Clang_Interface_DimensionTrait( loader )
          , parent( loader )
    {
    }
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_IContext >& parent)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_DimensionTrait >( loader, this ) )          , p_AST_Parser_Dimension( loader )
          , p_PerSourceSymbols_Interface_DimensionTrait( loader )
          , p_Concrete_Interface_DimensionTrait( loader )
          , p_Clang_Interface_DimensionTrait( loader )
          , parent( parent )
    {
    }
    bool Interface_DimensionTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Dimension >, data::Ptr< data::Tree::Interface_DimensionTrait > >{ data::Ptr< data::Tree::Interface_DimensionTrait >( loader, const_cast< Interface_DimensionTrait* >( this ) ) };
    }
    void Interface_DimensionTrait::set_inheritance_pointer()
    {
        p_AST_Parser_Dimension->m_inheritance = data::Ptr< data::Tree::Interface_DimensionTrait >( p_AST_Parser_Dimension, this );
    }
    void Interface_DimensionTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_Dimension );
        loader.load( parent );
    }
    void Interface_DimensionTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_Dimension );
        storer.store( parent );
    }
    void Interface_DimensionTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_DimensionTrait" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent", parent } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_InheritanceTrait : public mega::io::Object
    Interface_InheritanceTrait::Interface_InheritanceTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_InheritanceTrait >( loader, this ) )          , p_AST_Parser_Inheritance( loader )
          , p_Clang_Interface_InheritanceTrait( loader )
    {
    }
    bool Interface_InheritanceTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Inheritance >, data::Ptr< data::Tree::Interface_InheritanceTrait > >{ data::Ptr< data::Tree::Interface_InheritanceTrait >( loader, const_cast< Interface_InheritanceTrait* >( this ) ) };
    }
    void Interface_InheritanceTrait::set_inheritance_pointer()
    {
        p_AST_Parser_Inheritance->m_inheritance = data::Ptr< data::Tree::Interface_InheritanceTrait >( p_AST_Parser_Inheritance, this );
    }
    void Interface_InheritanceTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_Inheritance );
    }
    void Interface_InheritanceTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_Inheritance );
    }
    void Interface_InheritanceTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_InheritanceTrait" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Interface_LinkTrait : public mega::io::Object
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_LinkTrait >( loader, this ) )          , p_AST_Parser_Link( loader )
    {
    }
    bool Interface_LinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Link >, data::Ptr< data::Tree::Interface_LinkTrait > >{ data::Ptr< data::Tree::Interface_LinkTrait >( loader, const_cast< Interface_LinkTrait* >( this ) ) };
    }
    void Interface_LinkTrait::set_inheritance_pointer()
    {
        p_AST_Parser_Link->m_inheritance = data::Ptr< data::Tree::Interface_LinkTrait >( p_AST_Parser_Link, this );
    }
    void Interface_LinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_Link );
    }
    void Interface_LinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_Link );
    }
    void Interface_LinkTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_LinkTrait" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Interface_ReturnTypeTrait : public mega::io::Object
    Interface_ReturnTypeTrait::Interface_ReturnTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_ReturnTypeTrait >( loader, this ) )          , p_AST_Parser_ReturnType( loader )
          , p_Clang_Interface_ReturnTypeTrait( loader )
    {
    }
    bool Interface_ReturnTypeTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ReturnType >, data::Ptr< data::Tree::Interface_ReturnTypeTrait > >{ data::Ptr< data::Tree::Interface_ReturnTypeTrait >( loader, const_cast< Interface_ReturnTypeTrait* >( this ) ) };
    }
    void Interface_ReturnTypeTrait::set_inheritance_pointer()
    {
        p_AST_Parser_ReturnType->m_inheritance = data::Ptr< data::Tree::Interface_ReturnTypeTrait >( p_AST_Parser_ReturnType, this );
    }
    void Interface_ReturnTypeTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ReturnType );
    }
    void Interface_ReturnTypeTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ReturnType );
    }
    void Interface_ReturnTypeTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_ReturnTypeTrait" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Interface_ArgumentListTrait : public mega::io::Object
    Interface_ArgumentListTrait::Interface_ArgumentListTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_ArgumentListTrait >( loader, this ) )          , p_AST_Parser_ArgumentList( loader )
          , p_Clang_Interface_ArgumentListTrait( loader )
    {
    }
    bool Interface_ArgumentListTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_ArgumentList >, data::Ptr< data::Tree::Interface_ArgumentListTrait > >{ data::Ptr< data::Tree::Interface_ArgumentListTrait >( loader, const_cast< Interface_ArgumentListTrait* >( this ) ) };
    }
    void Interface_ArgumentListTrait::set_inheritance_pointer()
    {
        p_AST_Parser_ArgumentList->m_inheritance = data::Ptr< data::Tree::Interface_ArgumentListTrait >( p_AST_Parser_ArgumentList, this );
    }
    void Interface_ArgumentListTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ArgumentList );
    }
    void Interface_ArgumentListTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ArgumentList );
    }
    void Interface_ArgumentListTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_ArgumentListTrait" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Interface_SizeTrait : public mega::io::Object
    Interface_SizeTrait::Interface_SizeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_SizeTrait >( loader, this ) )          , p_AST_Parser_Size( loader )
          , p_Clang_Interface_SizeTrait( loader )
    {
    }
    bool Interface_SizeTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::AST::Parser_Size >, data::Ptr< data::Tree::Interface_SizeTrait > >{ data::Ptr< data::Tree::Interface_SizeTrait >( loader, const_cast< Interface_SizeTrait* >( this ) ) };
    }
    void Interface_SizeTrait::set_inheritance_pointer()
    {
        p_AST_Parser_Size->m_inheritance = data::Ptr< data::Tree::Interface_SizeTrait >( p_AST_Parser_Size, this );
    }
    void Interface_SizeTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_Size );
    }
    void Interface_SizeTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_Size );
    }
    void Interface_SizeTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_SizeTrait" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Interface_ContextGroup : public mega::io::Object
    Interface_ContextGroup::Interface_ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_ContextGroup >( loader, this ) )    {
    }
    Interface_ContextGroup::Interface_ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& children)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_ContextGroup >( loader, this ) )          , children( children )
    {
    }
    bool Interface_ContextGroup::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > >{ data::Ptr< data::Tree::Interface_ContextGroup >( loader, const_cast< Interface_ContextGroup* >( this ) ) };
    }
    void Interface_ContextGroup::set_inheritance_pointer()
    {
    }
    void Interface_ContextGroup::load( mega::io::Loader& loader )
    {
        loader.load( children );
    }
    void Interface_ContextGroup::store( mega::io::Storer& storer ) const
    {
        storer.store( children );
    }
    void Interface_ContextGroup::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_ContextGroup" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "children", children } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Root : public mega::io::Object
    Interface_Root::Interface_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Root >( loader, this ) )          , p_Tree_Interface_ContextGroup( loader )
          , root( loader )
    {
    }
    Interface_Root::Interface_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_ObjectSourceRoot >& root)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Root >( loader, this ) )          , p_Tree_Interface_ContextGroup( loader )
          , root( root )
    {
    }
    bool Interface_Root::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > >{ data::Ptr< data::Tree::Interface_Root >( loader, const_cast< Interface_Root* >( this ) ) };
    }
    void Interface_Root::set_inheritance_pointer()
    {
        p_Tree_Interface_ContextGroup->m_inheritance = data::Ptr< data::Tree::Interface_Root >( p_Tree_Interface_ContextGroup, this );
    }
    void Interface_Root::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_ContextGroup );
        loader.load( root );
    }
    void Interface_Root::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_ContextGroup );
        storer.store( root );
    }
    void Interface_Root::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_Root" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "root", root } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_IContext : public mega::io::Object
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_IContext >( loader, this ) )          , p_Tree_Interface_ContextGroup( loader )
          , p_PerSourceSymbols_Interface_IContext( loader )
          , p_Concrete_Interface_IContext( loader )
          , parent( loader )
    {
    }
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& identifier, const data::Ptr< data::Tree::Interface_ContextGroup >& parent)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_IContext >( loader, this ) )          , p_Tree_Interface_ContextGroup( loader )
          , p_PerSourceSymbols_Interface_IContext( loader )
          , p_Concrete_Interface_IContext( loader )
          , identifier( identifier )
          , parent( parent )
    {
    }
    bool Interface_IContext::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > >{ data::Ptr< data::Tree::Interface_IContext >( loader, const_cast< Interface_IContext* >( this ) ) };
    }
    void Interface_IContext::set_inheritance_pointer()
    {
        p_Tree_Interface_ContextGroup->m_inheritance = data::Ptr< data::Tree::Interface_IContext >( p_Tree_Interface_ContextGroup, this );
    }
    void Interface_IContext::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_ContextGroup );
        loader.load( identifier );
        loader.load( parent );
    }
    void Interface_IContext::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_ContextGroup );
        storer.store( identifier );
        storer.store( parent );
    }
    void Interface_IContext::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_IContext" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "identifier", identifier } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent", parent } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Namespace : public mega::io::Object
    Interface_Namespace::Interface_Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Namespace >( loader, this ) )          , p_Tree_Interface_IContext( loader )
    {
    }
    Interface_Namespace::Interface_Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& is_global, const std::vector< data::Ptr< data::AST::Parser_ContextDef > >& namespace_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Namespace >( loader, this ) )          , p_Tree_Interface_IContext( loader )
          , is_global( is_global )
          , namespace_defs( namespace_defs )
    {
    }
    bool Interface_Namespace::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > >{ data::Ptr< data::Tree::Interface_Namespace >( loader, const_cast< Interface_Namespace* >( this ) ) };
    }
    void Interface_Namespace::set_inheritance_pointer()
    {
        p_Tree_Interface_IContext->m_inheritance = data::Ptr< data::Tree::Interface_Namespace >( p_Tree_Interface_IContext, this );
    }
    void Interface_Namespace::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_IContext );
        loader.load( is_global );
        loader.load( namespace_defs );
        loader.load( dimension_traits );
    }
    void Interface_Namespace::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( is_global );
        storer.store( namespace_defs );
        VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Interface_Namespace.dimension_traits has NOT been set" );
        storer.store( dimension_traits );
    }
    void Interface_Namespace::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_Namespace" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "is_global", is_global } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "namespace_defs", namespace_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_traits", dimension_traits.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Abstract : public mega::io::Object
    Interface_Abstract::Interface_Abstract( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Abstract >( loader, this ) )          , p_Tree_Interface_IContext( loader )
    {
    }
    Interface_Abstract::Interface_Abstract( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_AbstractDef > >& abstract_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Abstract >( loader, this ) )          , p_Tree_Interface_IContext( loader )
          , abstract_defs( abstract_defs )
    {
    }
    bool Interface_Abstract::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > >{ data::Ptr< data::Tree::Interface_Abstract >( loader, const_cast< Interface_Abstract* >( this ) ) };
    }
    void Interface_Abstract::set_inheritance_pointer()
    {
        p_Tree_Interface_IContext->m_inheritance = data::Ptr< data::Tree::Interface_Abstract >( p_Tree_Interface_IContext, this );
    }
    void Interface_Abstract::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_IContext );
        loader.load( abstract_defs );
        loader.load( dimension_traits );
        loader.load( inheritance_trait );
    }
    void Interface_Abstract::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( abstract_defs );
        VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Interface_Abstract.dimension_traits has NOT been set" );
        storer.store( dimension_traits );
        VERIFY_RTE_MSG( inheritance_trait.has_value(), "Tree::Interface_Abstract.inheritance_trait has NOT been set" );
        storer.store( inheritance_trait );
    }
    void Interface_Abstract::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_Abstract" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "abstract_defs", abstract_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_traits", dimension_traits.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_trait", inheritance_trait.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Action : public mega::io::Object
    Interface_Action::Interface_Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Action >( loader, this ) )          , p_Tree_Interface_IContext( loader )
    {
    }
    Interface_Action::Interface_Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_ActionDef > >& action_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Action >( loader, this ) )          , p_Tree_Interface_IContext( loader )
          , action_defs( action_defs )
    {
    }
    bool Interface_Action::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > >{ data::Ptr< data::Tree::Interface_Action >( loader, const_cast< Interface_Action* >( this ) ) };
    }
    void Interface_Action::set_inheritance_pointer()
    {
        p_Tree_Interface_IContext->m_inheritance = data::Ptr< data::Tree::Interface_Action >( p_Tree_Interface_IContext, this );
    }
    void Interface_Action::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_IContext );
        loader.load( action_defs );
        loader.load( dimension_traits );
        loader.load( inheritance_trait );
        loader.load( size_trait );
    }
    void Interface_Action::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( action_defs );
        VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Interface_Action.dimension_traits has NOT been set" );
        storer.store( dimension_traits );
        VERIFY_RTE_MSG( inheritance_trait.has_value(), "Tree::Interface_Action.inheritance_trait has NOT been set" );
        storer.store( inheritance_trait );
        VERIFY_RTE_MSG( size_trait.has_value(), "Tree::Interface_Action.size_trait has NOT been set" );
        storer.store( size_trait );
    }
    void Interface_Action::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_Action" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "action_defs", action_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_traits", dimension_traits.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_trait", inheritance_trait.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "size_trait", size_trait.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Event : public mega::io::Object
    Interface_Event::Interface_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Event >( loader, this ) )          , p_Tree_Interface_IContext( loader )
    {
    }
    Interface_Event::Interface_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_EventDef > >& event_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Event >( loader, this ) )          , p_Tree_Interface_IContext( loader )
          , event_defs( event_defs )
    {
    }
    bool Interface_Event::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > >{ data::Ptr< data::Tree::Interface_Event >( loader, const_cast< Interface_Event* >( this ) ) };
    }
    void Interface_Event::set_inheritance_pointer()
    {
        p_Tree_Interface_IContext->m_inheritance = data::Ptr< data::Tree::Interface_Event >( p_Tree_Interface_IContext, this );
    }
    void Interface_Event::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_IContext );
        loader.load( event_defs );
        loader.load( dimension_traits );
        loader.load( inheritance_trait );
        loader.load( size_trait );
    }
    void Interface_Event::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( event_defs );
        VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Interface_Event.dimension_traits has NOT been set" );
        storer.store( dimension_traits );
        VERIFY_RTE_MSG( inheritance_trait.has_value(), "Tree::Interface_Event.inheritance_trait has NOT been set" );
        storer.store( inheritance_trait );
        VERIFY_RTE_MSG( size_trait.has_value(), "Tree::Interface_Event.size_trait has NOT been set" );
        storer.store( size_trait );
    }
    void Interface_Event::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_Event" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "event_defs", event_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_traits", dimension_traits.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_trait", inheritance_trait.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "size_trait", size_trait.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Function : public mega::io::Object
    Interface_Function::Interface_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Function >( loader, this ) )          , p_Tree_Interface_IContext( loader )
          , return_type_trait( loader )
          , arguments_trait( loader )
    {
    }
    Interface_Function::Interface_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_FunctionDef > >& function_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Function >( loader, this ) )          , p_Tree_Interface_IContext( loader )
          , function_defs( function_defs )
    {
    }
    bool Interface_Function::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > >{ data::Ptr< data::Tree::Interface_Function >( loader, const_cast< Interface_Function* >( this ) ) };
    }
    void Interface_Function::set_inheritance_pointer()
    {
        p_Tree_Interface_IContext->m_inheritance = data::Ptr< data::Tree::Interface_Function >( p_Tree_Interface_IContext, this );
    }
    void Interface_Function::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_IContext );
        loader.load( function_defs );
        loader.load( return_type_trait );
        loader.load( arguments_trait );
    }
    void Interface_Function::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( function_defs );
        VERIFY_RTE_MSG( return_type_trait.has_value(), "Tree::Interface_Function.return_type_trait has NOT been set" );
        storer.store( return_type_trait );
        VERIFY_RTE_MSG( arguments_trait.has_value(), "Tree::Interface_Function.arguments_trait has NOT been set" );
        storer.store( arguments_trait );
    }
    void Interface_Function::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_Function" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "function_defs", function_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "return_type_trait", return_type_trait.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "arguments_trait", arguments_trait.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Object : public mega::io::Object
    Interface_Object::Interface_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Object >( loader, this ) )          , p_Tree_Interface_IContext( loader )
    {
    }
    Interface_Object::Interface_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_ObjectDef > >& object_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Object >( loader, this ) )          , p_Tree_Interface_IContext( loader )
          , object_defs( object_defs )
    {
    }
    bool Interface_Object::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > >{ data::Ptr< data::Tree::Interface_Object >( loader, const_cast< Interface_Object* >( this ) ) };
    }
    void Interface_Object::set_inheritance_pointer()
    {
        p_Tree_Interface_IContext->m_inheritance = data::Ptr< data::Tree::Interface_Object >( p_Tree_Interface_IContext, this );
    }
    void Interface_Object::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_IContext );
        loader.load( object_defs );
        loader.load( dimension_traits );
        loader.load( inheritance_trait );
    }
    void Interface_Object::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( object_defs );
        VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Interface_Object.dimension_traits has NOT been set" );
        storer.store( dimension_traits );
        VERIFY_RTE_MSG( inheritance_trait.has_value(), "Tree::Interface_Object.inheritance_trait has NOT been set" );
        storer.store( inheritance_trait );
    }
    void Interface_Object::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_Object" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "object_defs", object_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_traits", dimension_traits.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_trait", inheritance_trait.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Link : public mega::io::Object
    Interface_Link::Interface_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Link >( loader, this ) )          , p_Tree_Interface_IContext( loader )
          , link_trait( loader )
          , link_target( loader )
    {
    }
    Interface_Link::Interface_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_LinkDef > >& link_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Link >( loader, this ) )          , p_Tree_Interface_IContext( loader )
          , link_defs( link_defs )
    {
    }
    bool Interface_Link::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > >{ data::Ptr< data::Tree::Interface_Link >( loader, const_cast< Interface_Link* >( this ) ) };
    }
    void Interface_Link::set_inheritance_pointer()
    {
        p_Tree_Interface_IContext->m_inheritance = data::Ptr< data::Tree::Interface_Link >( p_Tree_Interface_IContext, this );
    }
    void Interface_Link::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_IContext );
        loader.load( link_defs );
        loader.load( link_trait );
        loader.load( link_target );
    }
    void Interface_Link::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( link_defs );
        VERIFY_RTE_MSG( link_trait.has_value(), "Tree::Interface_Link.link_trait has NOT been set" );
        storer.store( link_trait );
        VERIFY_RTE_MSG( link_target.has_value(), "Tree::Interface_Link.link_target has NOT been set" );
        storer.store( link_target );
    }
    void Interface_Link::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_Link" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "link_defs", link_defs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "link_trait", link_trait.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "link_target", link_target.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Table : public mega::io::Object
    Interface_Table::Interface_Table( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Table >( loader, this ) )          , p_Tree_Interface_IContext( loader )
    {
    }
    Interface_Table::Interface_Table( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_TableDef > >& table_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Table >( loader, this ) )          , p_Tree_Interface_IContext( loader )
          , table_defs( table_defs )
    {
    }
    bool Interface_Table::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Tree::Interface_ContextGroup >, data::Ptr< data::Tree::Interface_Root >, data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_Namespace >, data::Ptr< data::Tree::Interface_Abstract >, data::Ptr< data::Tree::Interface_Action >, data::Ptr< data::Tree::Interface_Event >, data::Ptr< data::Tree::Interface_Function >, data::Ptr< data::Tree::Interface_Object >, data::Ptr< data::Tree::Interface_Link >, data::Ptr< data::Tree::Interface_Table > >{ data::Ptr< data::Tree::Interface_Table >( loader, const_cast< Interface_Table* >( this ) ) };
    }
    void Interface_Table::set_inheritance_pointer()
    {
        p_Tree_Interface_IContext->m_inheritance = data::Ptr< data::Tree::Interface_Table >( p_Tree_Interface_IContext, this );
    }
    void Interface_Table::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_IContext );
        loader.load( table_defs );
    }
    void Interface_Table::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( table_defs );
    }
    void Interface_Table::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_Table" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "table_defs", table_defs } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace DPGraph
{
    // struct Dependencies_Glob : public mega::io::Object
    Dependencies_Glob::Dependencies_Glob( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_Glob >( loader, this ) )    {
    }
    Dependencies_Glob::Dependencies_Glob( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const boost::filesystem::path& location, const std::string& glob)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_Glob >( loader, this ) )          , location( location )
          , glob( glob )
    {
    }
    bool Dependencies_Glob::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::DPGraph::Dependencies_Glob > >{ data::Ptr< data::DPGraph::Dependencies_Glob >( loader, const_cast< Dependencies_Glob* >( this ) ) };
    }
    void Dependencies_Glob::set_inheritance_pointer()
    {
    }
    void Dependencies_Glob::load( mega::io::Loader& loader )
    {
        loader.load( location );
        loader.load( glob );
    }
    void Dependencies_Glob::store( mega::io::Storer& storer ) const
    {
        storer.store( location );
        storer.store( glob );
    }
    void Dependencies_Glob::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Dependencies_Glob" },
                { "filetype" , "DPGraph" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "location", location } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "glob", glob } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Dependencies_ObjectDependencies : public mega::io::Object
    Dependencies_ObjectDependencies::Dependencies_ObjectDependencies( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_ObjectDependencies >( loader, this ) )    {
    }
    Dependencies_ObjectDependencies::Dependencies_ObjectDependencies( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::io::megaFilePath& source_file, const std::size_t& hash_code, const std::vector< data::Ptr< data::DPGraph::Dependencies_Glob > >& globs, const std::vector< boost::filesystem::path >& resolution)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_ObjectDependencies >( loader, this ) )          , source_file( source_file )
          , hash_code( hash_code )
          , globs( globs )
          , resolution( resolution )
    {
    }
    bool Dependencies_ObjectDependencies::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::DPGraph::Dependencies_ObjectDependencies > >{ data::Ptr< data::DPGraph::Dependencies_ObjectDependencies >( loader, const_cast< Dependencies_ObjectDependencies* >( this ) ) };
    }
    void Dependencies_ObjectDependencies::set_inheritance_pointer()
    {
    }
    void Dependencies_ObjectDependencies::load( mega::io::Loader& loader )
    {
        loader.load( source_file );
        loader.load( hash_code );
        loader.load( globs );
        loader.load( resolution );
    }
    void Dependencies_ObjectDependencies::store( mega::io::Storer& storer ) const
    {
        storer.store( source_file );
        storer.store( hash_code );
        storer.store( globs );
        storer.store( resolution );
    }
    void Dependencies_ObjectDependencies::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Dependencies_ObjectDependencies" },
                { "filetype" , "DPGraph" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_file", source_file } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "hash_code", hash_code } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "globs", globs } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "resolution", resolution } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Dependencies_Analysis : public mega::io::Object
    Dependencies_Analysis::Dependencies_Analysis( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_Analysis >( loader, this ) )    {
    }
    Dependencies_Analysis::Dependencies_Analysis( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::DPGraph::Dependencies_ObjectDependencies > >& objects)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_Analysis >( loader, this ) )          , objects( objects )
    {
    }
    bool Dependencies_Analysis::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::DPGraph::Dependencies_Analysis > >{ data::Ptr< data::DPGraph::Dependencies_Analysis >( loader, const_cast< Dependencies_Analysis* >( this ) ) };
    }
    void Dependencies_Analysis::set_inheritance_pointer()
    {
    }
    void Dependencies_Analysis::load( mega::io::Loader& loader )
    {
        loader.load( objects );
    }
    void Dependencies_Analysis::store( mega::io::Storer& storer ) const
    {
        storer.store( objects );
    }
    void Dependencies_Analysis::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Dependencies_Analysis" },
                { "filetype" , "DPGraph" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "objects", objects } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace SymbolTable
{
    // struct Symbols_Symbol : public mega::io::Object
    Symbols_Symbol::Symbols_Symbol( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_Symbol >( loader, this ) )    {
    }
    Symbols_Symbol::Symbols_Symbol( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& symbol, const std::int32_t& id, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& contexts, const std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > >& dimensions)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_Symbol >( loader, this ) )          , symbol( symbol )
          , id( id )
          , contexts( contexts )
          , dimensions( dimensions )
    {
    }
    bool Symbols_Symbol::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::SymbolTable::Symbols_Symbol > >{ data::Ptr< data::SymbolTable::Symbols_Symbol >( loader, const_cast< Symbols_Symbol* >( this ) ) };
    }
    void Symbols_Symbol::set_inheritance_pointer()
    {
    }
    void Symbols_Symbol::load( mega::io::Loader& loader )
    {
        loader.load( symbol );
        loader.load( id );
        loader.load( contexts );
        loader.load( dimensions );
    }
    void Symbols_Symbol::store( mega::io::Storer& storer ) const
    {
        storer.store( symbol );
        storer.store( id );
        storer.store( contexts );
        storer.store( dimensions );
    }
    void Symbols_Symbol::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Symbols_Symbol" },
                { "filetype" , "SymbolTable" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbol", symbol } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "contexts", contexts } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Symbols_SymbolSet : public mega::io::Object
    Symbols_SymbolSet::Symbols_SymbolSet( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_SymbolSet >( loader, this ) )    {
    }
    Symbols_SymbolSet::Symbols_SymbolSet( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::map< std::string, data::Ptr< data::SymbolTable::Symbols_Symbol > >& symbols, const mega::io::megaFilePath& source_file, const std::size_t& hash_code, const std::map< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::SymbolTable::Symbols_Symbol > >& context_symbols, const std::map< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::SymbolTable::Symbols_Symbol > >& dimension_symbols, const std::map< data::Ptr< data::Tree::Interface_IContext >, int32_t >& context_type_ids, const std::map< data::Ptr< data::Tree::Interface_DimensionTrait >, int32_t >& dimension_type_ids)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_SymbolSet >( loader, this ) )          , symbols( symbols )
          , source_file( source_file )
          , hash_code( hash_code )
          , context_symbols( context_symbols )
          , dimension_symbols( dimension_symbols )
          , context_type_ids( context_type_ids )
          , dimension_type_ids( dimension_type_ids )
    {
    }
    bool Symbols_SymbolSet::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::SymbolTable::Symbols_SymbolSet > >{ data::Ptr< data::SymbolTable::Symbols_SymbolSet >( loader, const_cast< Symbols_SymbolSet* >( this ) ) };
    }
    void Symbols_SymbolSet::set_inheritance_pointer()
    {
    }
    void Symbols_SymbolSet::load( mega::io::Loader& loader )
    {
        loader.load( symbols );
        loader.load( source_file );
        loader.load( hash_code );
        loader.load( context_symbols );
        loader.load( dimension_symbols );
        loader.load( context_type_ids );
        loader.load( dimension_type_ids );
    }
    void Symbols_SymbolSet::store( mega::io::Storer& storer ) const
    {
        storer.store( symbols );
        storer.store( source_file );
        storer.store( hash_code );
        storer.store( context_symbols );
        storer.store( dimension_symbols );
        storer.store( context_type_ids );
        storer.store( dimension_type_ids );
    }
    void Symbols_SymbolSet::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Symbols_SymbolSet" },
                { "filetype" , "SymbolTable" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbols", symbols } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_file", source_file } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "hash_code", hash_code } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "context_symbols", context_symbols } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_symbols", dimension_symbols } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "context_type_ids", context_type_ids } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_type_ids", dimension_type_ids } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Symbols_SymbolTable : public mega::io::Object
    Symbols_SymbolTable::Symbols_SymbolTable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_SymbolTable >( loader, this ) )    {
    }
    Symbols_SymbolTable::Symbols_SymbolTable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::map< mega::io::megaFilePath, data::Ptr< data::SymbolTable::Symbols_SymbolSet > >& symbol_sets, const std::map< std::string, data::Ptr< data::SymbolTable::Symbols_Symbol > >& symbols, const std::map< int32_t, data::Ptr< data::Tree::Interface_IContext > >& context_type_ids, const std::map< int32_t, data::Ptr< data::Tree::Interface_DimensionTrait > >& dimension_type_ids, const std::map< int32_t, data::Ptr< data::SymbolTable::Symbols_Symbol > >& symbol_id_map)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_SymbolTable >( loader, this ) )          , symbol_sets( symbol_sets )
          , symbols( symbols )
          , context_type_ids( context_type_ids )
          , dimension_type_ids( dimension_type_ids )
          , symbol_id_map( symbol_id_map )
    {
    }
    bool Symbols_SymbolTable::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::SymbolTable::Symbols_SymbolTable > >{ data::Ptr< data::SymbolTable::Symbols_SymbolTable >( loader, const_cast< Symbols_SymbolTable* >( this ) ) };
    }
    void Symbols_SymbolTable::set_inheritance_pointer()
    {
    }
    void Symbols_SymbolTable::load( mega::io::Loader& loader )
    {
        loader.load( symbol_sets );
        loader.load( symbols );
        loader.load( context_type_ids );
        loader.load( dimension_type_ids );
        loader.load( symbol_id_map );
    }
    void Symbols_SymbolTable::store( mega::io::Storer& storer ) const
    {
        storer.store( symbol_sets );
        storer.store( symbols );
        storer.store( context_type_ids );
        storer.store( dimension_type_ids );
        storer.store( symbol_id_map );
    }
    void Symbols_SymbolTable::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Symbols_SymbolTable" },
                { "filetype" , "SymbolTable" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbol_sets", symbol_sets } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbols", symbols } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "context_type_ids", context_type_ids } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_type_ids", dimension_type_ids } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbol_id_map", symbol_id_map } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace PerSourceSymbols
{
    // struct Interface_DimensionTrait : public mega::io::Object
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_DimensionTrait( loader )
    {
    }
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait, const std::int32_t& symbol)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_DimensionTrait( p_Tree_Interface_DimensionTrait )
          , symbol( symbol )
    {
    }
    bool Interface_DimensionTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_DimensionTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_DimensionTrait->p_PerSourceSymbols_Interface_DimensionTrait = data::Ptr< data::PerSourceSymbols::Interface_DimensionTrait >( p_Tree_Interface_DimensionTrait, this );
    }
    void Interface_DimensionTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_DimensionTrait );
        loader.load( symbol );
        loader.load( type_id );
    }
    void Interface_DimensionTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_DimensionTrait );
        storer.store( symbol );
        VERIFY_RTE_MSG( type_id.has_value(), "Tree::Interface_DimensionTrait.type_id has NOT been set" );
        storer.store( type_id );
    }
    void Interface_DimensionTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_DimensionTrait" },
                { "filetype" , "PerSourceSymbols" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbol", symbol } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "type_id", type_id.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_IContext : public mega::io::Object
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_IContext( loader )
    {
    }
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext, const std::int32_t& symbol)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_IContext( p_Tree_Interface_IContext )
          , symbol( symbol )
    {
    }
    bool Interface_IContext::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_IContext::set_inheritance_pointer()
    {
        p_Tree_Interface_IContext->p_PerSourceSymbols_Interface_IContext = data::Ptr< data::PerSourceSymbols::Interface_IContext >( p_Tree_Interface_IContext, this );
    }
    void Interface_IContext::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_IContext );
        loader.load( symbol );
        loader.load( type_id );
    }
    void Interface_IContext::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( symbol );
        VERIFY_RTE_MSG( type_id.has_value(), "Tree::Interface_IContext.type_id has NOT been set" );
        storer.store( type_id );
    }
    void Interface_IContext::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_IContext" },
                { "filetype" , "PerSourceSymbols" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbol", symbol } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "type_id", type_id.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace Clang
{
    // struct Interface_DimensionTrait : public mega::io::Object
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_DimensionTrait( loader )
    {
    }
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait, const std::string& canonical_type, const std::size_t& size, const bool& simple, const std::vector< data::Ptr< data::SymbolTable::Symbols_Symbol > >& symbols)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_DimensionTrait( p_Tree_Interface_DimensionTrait )
          , canonical_type( canonical_type )
          , size( size )
          , simple( simple )
          , symbols( symbols )
    {
    }
    bool Interface_DimensionTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_DimensionTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_DimensionTrait->p_Clang_Interface_DimensionTrait = data::Ptr< data::Clang::Interface_DimensionTrait >( p_Tree_Interface_DimensionTrait, this );
    }
    void Interface_DimensionTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_DimensionTrait );
        loader.load( canonical_type );
        loader.load( size );
        loader.load( simple );
        loader.load( symbols );
    }
    void Interface_DimensionTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_DimensionTrait );
        storer.store( canonical_type );
        storer.store( size );
        storer.store( simple );
        storer.store( symbols );
    }
    void Interface_DimensionTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_DimensionTrait" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "canonical_type", canonical_type } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "size", size } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "simple", simple } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbols", symbols } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_InheritanceTrait : public mega::io::Object
    Interface_InheritanceTrait::Interface_InheritanceTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_InheritanceTrait( loader )
    {
    }
    Interface_InheritanceTrait::Interface_InheritanceTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_InheritanceTrait > p_Tree_Interface_InheritanceTrait, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& contexts)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_InheritanceTrait( p_Tree_Interface_InheritanceTrait )
          , contexts( contexts )
    {
    }
    bool Interface_InheritanceTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_InheritanceTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_InheritanceTrait->p_Clang_Interface_InheritanceTrait = data::Ptr< data::Clang::Interface_InheritanceTrait >( p_Tree_Interface_InheritanceTrait, this );
    }
    void Interface_InheritanceTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_InheritanceTrait );
        loader.load( contexts );
    }
    void Interface_InheritanceTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_InheritanceTrait );
        storer.store( contexts );
    }
    void Interface_InheritanceTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_InheritanceTrait" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "contexts", contexts } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_ReturnTypeTrait : public mega::io::Object
    Interface_ReturnTypeTrait::Interface_ReturnTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_ReturnTypeTrait( loader )
    {
    }
    Interface_ReturnTypeTrait::Interface_ReturnTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_ReturnTypeTrait > p_Tree_Interface_ReturnTypeTrait, const std::string& canonical_type)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_ReturnTypeTrait( p_Tree_Interface_ReturnTypeTrait )
          , canonical_type( canonical_type )
    {
    }
    bool Interface_ReturnTypeTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_ReturnTypeTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_ReturnTypeTrait->p_Clang_Interface_ReturnTypeTrait = data::Ptr< data::Clang::Interface_ReturnTypeTrait >( p_Tree_Interface_ReturnTypeTrait, this );
    }
    void Interface_ReturnTypeTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_ReturnTypeTrait );
        loader.load( canonical_type );
    }
    void Interface_ReturnTypeTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_ReturnTypeTrait );
        storer.store( canonical_type );
    }
    void Interface_ReturnTypeTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_ReturnTypeTrait" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "canonical_type", canonical_type } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_ArgumentListTrait : public mega::io::Object
    Interface_ArgumentListTrait::Interface_ArgumentListTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_ArgumentListTrait( loader )
    {
    }
    Interface_ArgumentListTrait::Interface_ArgumentListTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_ArgumentListTrait > p_Tree_Interface_ArgumentListTrait, const std::vector< std::string >& canonical_types)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_ArgumentListTrait( p_Tree_Interface_ArgumentListTrait )
          , canonical_types( canonical_types )
    {
    }
    bool Interface_ArgumentListTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_ArgumentListTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_ArgumentListTrait->p_Clang_Interface_ArgumentListTrait = data::Ptr< data::Clang::Interface_ArgumentListTrait >( p_Tree_Interface_ArgumentListTrait, this );
    }
    void Interface_ArgumentListTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_ArgumentListTrait );
        loader.load( canonical_types );
    }
    void Interface_ArgumentListTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_ArgumentListTrait );
        storer.store( canonical_types );
    }
    void Interface_ArgumentListTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_ArgumentListTrait" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "canonical_types", canonical_types } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_SizeTrait : public mega::io::Object
    Interface_SizeTrait::Interface_SizeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_SizeTrait( loader )
    {
    }
    Interface_SizeTrait::Interface_SizeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_SizeTrait > p_Tree_Interface_SizeTrait, const std::size_t& size)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_SizeTrait( p_Tree_Interface_SizeTrait )
          , size( size )
    {
    }
    bool Interface_SizeTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_SizeTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_SizeTrait->p_Clang_Interface_SizeTrait = data::Ptr< data::Clang::Interface_SizeTrait >( p_Tree_Interface_SizeTrait, this );
    }
    void Interface_SizeTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_SizeTrait );
        loader.load( size );
    }
    void Interface_SizeTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_SizeTrait );
        storer.store( size );
    }
    void Interface_SizeTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_SizeTrait" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "size", size } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace Concrete
{
    // struct Interface_DimensionTrait : public mega::io::Object
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_DimensionTrait( loader )
    {
    }
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait, const std::optional< data::Ptr< data::Concrete::Concrete_Dimension > >& concrete)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_DimensionTrait( p_Tree_Interface_DimensionTrait )
          , concrete( concrete )
    {
    }
    bool Interface_DimensionTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_DimensionTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_DimensionTrait->p_Concrete_Interface_DimensionTrait = data::Ptr< data::Concrete::Interface_DimensionTrait >( p_Tree_Interface_DimensionTrait, this );
    }
    void Interface_DimensionTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_DimensionTrait );
        loader.load( concrete );
    }
    void Interface_DimensionTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_DimensionTrait );
        storer.store( concrete );
    }
    void Interface_DimensionTrait::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_DimensionTrait" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete", concrete } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_IContext : public mega::io::Object
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_IContext( loader )
    {
    }
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext, const std::optional< data::Ptr< data::Concrete::Concrete_Context > >& concrete)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_IContext( p_Tree_Interface_IContext )
          , concrete( concrete )
    {
    }
    bool Interface_IContext::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_IContext::set_inheritance_pointer()
    {
        p_Tree_Interface_IContext->p_Concrete_Interface_IContext = data::Ptr< data::Concrete::Interface_IContext >( p_Tree_Interface_IContext, this );
    }
    void Interface_IContext::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_IContext );
        loader.load( concrete );
    }
    void Interface_IContext::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( concrete );
    }
    void Interface_IContext::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Interface_IContext" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete", concrete } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Dimension : public mega::io::Object
    Concrete_Dimension::Concrete_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimension >( loader, this ) )          , parent( loader )
          , interface_dimension( loader )
    {
    }
    Concrete_Dimension::Concrete_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Context >& parent, const data::Ptr< data::Tree::Interface_DimensionTrait >& interface_dimension)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimension >( loader, this ) )          , parent( parent )
          , interface_dimension( interface_dimension )
    {
    }
    bool Concrete_Dimension::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Concrete::Concrete_Dimension > >{ data::Ptr< data::Concrete::Concrete_Dimension >( loader, const_cast< Concrete_Dimension* >( this ) ) };
    }
    void Concrete_Dimension::set_inheritance_pointer()
    {
    }
    void Concrete_Dimension::load( mega::io::Loader& loader )
    {
        loader.load( parent );
        loader.load( interface_dimension );
    }
    void Concrete_Dimension::store( mega::io::Storer& storer ) const
    {
        storer.store( parent );
        storer.store( interface_dimension );
    }
    void Concrete_Dimension::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimension" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent", parent } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_dimension", interface_dimension } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_ContextGroup : public mega::io::Object
    Concrete_ContextGroup::Concrete_ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_ContextGroup >( loader, this ) )    {
    }
    Concrete_ContextGroup::Concrete_ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Concrete::Concrete_Context > >& children)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_ContextGroup >( loader, this ) )          , children( children )
    {
    }
    bool Concrete_ContextGroup::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > >{ data::Ptr< data::Concrete::Concrete_ContextGroup >( loader, const_cast< Concrete_ContextGroup* >( this ) ) };
    }
    void Concrete_ContextGroup::set_inheritance_pointer()
    {
    }
    void Concrete_ContextGroup::load( mega::io::Loader& loader )
    {
        loader.load( children );
    }
    void Concrete_ContextGroup::store( mega::io::Storer& storer ) const
    {
        storer.store( children );
    }
    void Concrete_ContextGroup::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Concrete_ContextGroup" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "children", children } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Context : public mega::io::Object
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Context >( loader, this ) )          , p_Concrete_Concrete_ContextGroup( loader )
          , parent( loader )
          , interface( loader )
    {
    }
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_ContextGroup >& parent, const data::Ptr< data::Tree::Interface_IContext >& interface, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& inheritance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Context >( loader, this ) )          , p_Concrete_Concrete_ContextGroup( loader )
          , parent( parent )
          , interface( interface )
          , inheritance( inheritance )
    {
    }
    bool Concrete_Context::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > >{ data::Ptr< data::Concrete::Concrete_Context >( loader, const_cast< Concrete_Context* >( this ) ) };
    }
    void Concrete_Context::set_inheritance_pointer()
    {
        p_Concrete_Concrete_ContextGroup->m_inheritance = data::Ptr< data::Concrete::Concrete_Context >( p_Concrete_Concrete_ContextGroup, this );
    }
    void Concrete_Context::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_ContextGroup );
        loader.load( parent );
        loader.load( interface );
        loader.load( inheritance );
    }
    void Concrete_Context::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_ContextGroup );
        storer.store( parent );
        storer.store( interface );
        storer.store( inheritance );
    }
    void Concrete_Context::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Concrete_Context" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent", parent } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface", interface } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Namespace : public mega::io::Object
    Concrete_Namespace::Concrete_Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Namespace >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_namespace( loader )
    {
    }
    Concrete_Namespace::Concrete_Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Namespace >& interface_namespace, const std::vector< data::Ptr< data::Concrete::Concrete_Dimension > >& dimensions)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Namespace >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_namespace( interface_namespace )
          , dimensions( dimensions )
    {
    }
    bool Concrete_Namespace::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > >{ data::Ptr< data::Concrete::Concrete_Namespace >( loader, const_cast< Concrete_Namespace* >( this ) ) };
    }
    void Concrete_Namespace::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Concrete_Namespace >( p_Concrete_Concrete_Context, this );
    }
    void Concrete_Namespace::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Context );
        loader.load( interface_namespace );
        loader.load( dimensions );
    }
    void Concrete_Namespace::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( interface_namespace );
        storer.store( dimensions );
    }
    void Concrete_Namespace::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Concrete_Namespace" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_namespace", interface_namespace } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Action : public mega::io::Object
    Concrete_Action::Concrete_Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Action >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_action( loader )
    {
    }
    Concrete_Action::Concrete_Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Action >& interface_action, const std::vector< data::Ptr< data::Concrete::Concrete_Dimension > >& dimensions)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Action >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_action( interface_action )
          , dimensions( dimensions )
    {
    }
    bool Concrete_Action::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > >{ data::Ptr< data::Concrete::Concrete_Action >( loader, const_cast< Concrete_Action* >( this ) ) };
    }
    void Concrete_Action::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Concrete_Action >( p_Concrete_Concrete_Context, this );
    }
    void Concrete_Action::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Context );
        loader.load( interface_action );
        loader.load( dimensions );
    }
    void Concrete_Action::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( interface_action );
        storer.store( dimensions );
    }
    void Concrete_Action::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Concrete_Action" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_action", interface_action } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Event : public mega::io::Object
    Concrete_Event::Concrete_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Event >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_event( loader )
    {
    }
    Concrete_Event::Concrete_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Event >& interface_event, const std::vector< data::Ptr< data::Concrete::Concrete_Dimension > >& dimensions)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Event >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_event( interface_event )
          , dimensions( dimensions )
    {
    }
    bool Concrete_Event::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > >{ data::Ptr< data::Concrete::Concrete_Event >( loader, const_cast< Concrete_Event* >( this ) ) };
    }
    void Concrete_Event::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Concrete_Event >( p_Concrete_Concrete_Context, this );
    }
    void Concrete_Event::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Context );
        loader.load( interface_event );
        loader.load( dimensions );
    }
    void Concrete_Event::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( interface_event );
        storer.store( dimensions );
    }
    void Concrete_Event::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Concrete_Event" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_event", interface_event } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Function : public mega::io::Object
    Concrete_Function::Concrete_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Function >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_function( loader )
    {
    }
    Concrete_Function::Concrete_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Function >& interface_function)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Function >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_function( interface_function )
    {
    }
    bool Concrete_Function::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > >{ data::Ptr< data::Concrete::Concrete_Function >( loader, const_cast< Concrete_Function* >( this ) ) };
    }
    void Concrete_Function::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Concrete_Function >( p_Concrete_Concrete_Context, this );
    }
    void Concrete_Function::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Context );
        loader.load( interface_function );
    }
    void Concrete_Function::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( interface_function );
    }
    void Concrete_Function::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Concrete_Function" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_function", interface_function } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Object : public mega::io::Object
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Object >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_object( loader )
    {
    }
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Object >& interface_object, const std::vector< data::Ptr< data::Concrete::Concrete_Dimension > >& dimensions)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Object >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_object( interface_object )
          , dimensions( dimensions )
    {
    }
    bool Concrete_Object::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > >{ data::Ptr< data::Concrete::Concrete_Object >( loader, const_cast< Concrete_Object* >( this ) ) };
    }
    void Concrete_Object::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Concrete_Object >( p_Concrete_Concrete_Context, this );
    }
    void Concrete_Object::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Context );
        loader.load( interface_object );
        loader.load( dimensions );
    }
    void Concrete_Object::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( interface_object );
        storer.store( dimensions );
    }
    void Concrete_Object::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Concrete_Object" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_object", interface_object } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Link : public mega::io::Object
    Concrete_Link::Concrete_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Link >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_link( loader )
    {
    }
    Concrete_Link::Concrete_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Link >& interface_link)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Link >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_link( interface_link )
    {
    }
    bool Concrete_Link::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > >{ data::Ptr< data::Concrete::Concrete_Link >( loader, const_cast< Concrete_Link* >( this ) ) };
    }
    void Concrete_Link::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Concrete_Link >( p_Concrete_Concrete_Context, this );
    }
    void Concrete_Link::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Context );
        loader.load( interface_link );
    }
    void Concrete_Link::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( interface_link );
    }
    void Concrete_Link::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Concrete_Link" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_link", interface_link } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Table : public mega::io::Object
    Concrete_Table::Concrete_Table( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Table >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_table( loader )
    {
    }
    Concrete_Table::Concrete_Table( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Table >& interface_table)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Table >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_table( interface_table )
    {
    }
    bool Concrete_Table::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > >{ data::Ptr< data::Concrete::Concrete_Table >( loader, const_cast< Concrete_Table* >( this ) ) };
    }
    void Concrete_Table::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Concrete_Table >( p_Concrete_Concrete_Context, this );
    }
    void Concrete_Table::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Context );
        loader.load( interface_table );
    }
    void Concrete_Table::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( interface_table );
    }
    void Concrete_Table::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Concrete_Table" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_table", interface_table } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Root : public mega::io::Object
    Concrete_Root::Concrete_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Root >( loader, this ) )          , p_Concrete_Concrete_ContextGroup( loader )
          , interface_root( loader )
    {
    }
    Concrete_Root::Concrete_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Root >& interface_root)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Root >( loader, this ) )          , p_Concrete_Concrete_ContextGroup( loader )
          , interface_root( interface_root )
    {
    }
    bool Concrete_Root::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Concrete::Concrete_ContextGroup >, data::Ptr< data::Concrete::Concrete_Context >, data::Ptr< data::Concrete::Concrete_Namespace >, data::Ptr< data::Concrete::Concrete_Action >, data::Ptr< data::Concrete::Concrete_Event >, data::Ptr< data::Concrete::Concrete_Function >, data::Ptr< data::Concrete::Concrete_Object >, data::Ptr< data::Concrete::Concrete_Link >, data::Ptr< data::Concrete::Concrete_Table >, data::Ptr< data::Concrete::Concrete_Root > >{ data::Ptr< data::Concrete::Concrete_Root >( loader, const_cast< Concrete_Root* >( this ) ) };
    }
    void Concrete_Root::set_inheritance_pointer()
    {
        p_Concrete_Concrete_ContextGroup->m_inheritance = data::Ptr< data::Concrete::Concrete_Root >( p_Concrete_Concrete_ContextGroup, this );
    }
    void Concrete_Root::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_ContextGroup );
        loader.load( interface_root );
    }
    void Concrete_Root::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_ContextGroup );
        storer.store( interface_root );
    }
    void Concrete_Root::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Concrete_Root" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_root", interface_root } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace Model
{
    // struct HyperGraph_ObjectGraph : public mega::io::Object
    HyperGraph_ObjectGraph::HyperGraph_ObjectGraph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_ObjectGraph >( loader, this ) )    {
    }
    HyperGraph_ObjectGraph::HyperGraph_ObjectGraph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::io::megaFilePath& source_file, const std::size_t& hash_code)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_ObjectGraph >( loader, this ) )          , source_file( source_file )
          , hash_code( hash_code )
    {
    }
    bool HyperGraph_ObjectGraph::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Model::HyperGraph_ObjectGraph > >{ data::Ptr< data::Model::HyperGraph_ObjectGraph >( loader, const_cast< HyperGraph_ObjectGraph* >( this ) ) };
    }
    void HyperGraph_ObjectGraph::set_inheritance_pointer()
    {
    }
    void HyperGraph_ObjectGraph::load( mega::io::Loader& loader )
    {
        loader.load( source_file );
        loader.load( hash_code );
    }
    void HyperGraph_ObjectGraph::store( mega::io::Storer& storer ) const
    {
        storer.store( source_file );
        storer.store( hash_code );
    }
    void HyperGraph_ObjectGraph::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "HyperGraph_ObjectGraph" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_file", source_file } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "hash_code", hash_code } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct HyperGraph_Graph : public mega::io::Object
    HyperGraph_Graph::HyperGraph_Graph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_Graph >( loader, this ) )    {
    }
    HyperGraph_Graph::HyperGraph_Graph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Model::HyperGraph_ObjectGraph > >& objects)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_Graph >( loader, this ) )          , objects( objects )
    {
    }
    bool HyperGraph_Graph::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Model::HyperGraph_Graph > >{ data::Ptr< data::Model::HyperGraph_Graph >( loader, const_cast< HyperGraph_Graph* >( this ) ) };
    }
    void HyperGraph_Graph::set_inheritance_pointer()
    {
    }
    void HyperGraph_Graph::load( mega::io::Loader& loader )
    {
        loader.load( objects );
    }
    void HyperGraph_Graph::store( mega::io::Storer& storer ) const
    {
        storer.store( objects );
    }
    void HyperGraph_Graph::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "HyperGraph_Graph" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "objects", objects } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace Derivations
{
    // struct Derivation_ObjectMapping : public mega::io::Object
    Derivation_ObjectMapping::Derivation_ObjectMapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Derivations::Derivation_ObjectMapping >( loader, this ) )    {
    }
    Derivation_ObjectMapping::Derivation_ObjectMapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::io::megaFilePath& source_file, const std::size_t& hash_code, const std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Tree::Interface_IContext > >& inheritance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Derivations::Derivation_ObjectMapping >( loader, this ) )          , source_file( source_file )
          , hash_code( hash_code )
          , inheritance( inheritance )
    {
    }
    bool Derivation_ObjectMapping::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Derivations::Derivation_ObjectMapping > >{ data::Ptr< data::Derivations::Derivation_ObjectMapping >( loader, const_cast< Derivation_ObjectMapping* >( this ) ) };
    }
    void Derivation_ObjectMapping::set_inheritance_pointer()
    {
    }
    void Derivation_ObjectMapping::load( mega::io::Loader& loader )
    {
        loader.load( source_file );
        loader.load( hash_code );
        loader.load( inheritance );
    }
    void Derivation_ObjectMapping::store( mega::io::Storer& storer ) const
    {
        storer.store( source_file );
        storer.store( hash_code );
        storer.store( inheritance );
    }
    void Derivation_ObjectMapping::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Derivation_ObjectMapping" },
                { "filetype" , "Derivations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_file", source_file } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "hash_code", hash_code } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Derivation_Mapping : public mega::io::Object
    Derivation_Mapping::Derivation_Mapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Derivations::Derivation_Mapping >( loader, this ) )    {
    }
    Derivation_Mapping::Derivation_Mapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Derivations::Derivation_ObjectMapping > >& mappings)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Derivations::Derivation_Mapping >( loader, this ) )          , mappings( mappings )
    {
    }
    bool Derivation_Mapping::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Derivations::Derivation_Mapping > >{ data::Ptr< data::Derivations::Derivation_Mapping >( loader, const_cast< Derivation_Mapping* >( this ) ) };
    }
    void Derivation_Mapping::set_inheritance_pointer()
    {
    }
    void Derivation_Mapping::load( mega::io::Loader& loader )
    {
        loader.load( mappings );
        loader.load( inheritance );
    }
    void Derivation_Mapping::store( mega::io::Storer& storer ) const
    {
        storer.store( mappings );
        VERIFY_RTE_MSG( inheritance.has_value(), "Derivations::Derivation_Mapping.inheritance has NOT been set" );
        storer.store( inheritance );
    }
    void Derivation_Mapping::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Derivation_Mapping" },
                { "filetype" , "Derivations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "mappings", mappings } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}
namespace Operations
{
    // struct Invocations_Variables_Variable : public mega::io::Object
    Invocations_Variables_Variable::Invocations_Variables_Variable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Variable >( loader, this ) )    {
    }
    Invocations_Variables_Variable::Invocations_Variables_Variable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::optional< data::Ptr< data::Operations::Invocations_Variables_Variable > >& parent)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Variable >( loader, this ) )          , parent( parent )
    {
    }
    bool Invocations_Variables_Variable::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Variables_Variable >, data::Ptr< data::Operations::Invocations_Variables_Instance >, data::Ptr< data::Operations::Invocations_Variables_Reference >, data::Ptr< data::Operations::Invocations_Variables_Dimension >, data::Ptr< data::Operations::Invocations_Variables_Context > >{ data::Ptr< data::Operations::Invocations_Variables_Variable >( loader, const_cast< Invocations_Variables_Variable* >( this ) ) };
    }
    void Invocations_Variables_Variable::set_inheritance_pointer()
    {
    }
    void Invocations_Variables_Variable::load( mega::io::Loader& loader )
    {
        loader.load( parent );
    }
    void Invocations_Variables_Variable::store( mega::io::Storer& storer ) const
    {
        storer.store( parent );
    }
    void Invocations_Variables_Variable::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Variables_Variable" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent", parent } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Variables_Instance : public mega::io::Object
    Invocations_Variables_Instance::Invocations_Variables_Instance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Instance >( loader, this ) )          , p_Operations_Invocations_Variables_Variable( loader )
          , concrete( loader )
    {
    }
    Invocations_Variables_Instance::Invocations_Variables_Instance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Context >& concrete)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Instance >( loader, this ) )          , p_Operations_Invocations_Variables_Variable( loader )
          , concrete( concrete )
    {
    }
    bool Invocations_Variables_Instance::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Variables_Variable >, data::Ptr< data::Operations::Invocations_Variables_Instance >, data::Ptr< data::Operations::Invocations_Variables_Reference >, data::Ptr< data::Operations::Invocations_Variables_Dimension >, data::Ptr< data::Operations::Invocations_Variables_Context > >{ data::Ptr< data::Operations::Invocations_Variables_Instance >( loader, const_cast< Invocations_Variables_Instance* >( this ) ) };
    }
    void Invocations_Variables_Instance::set_inheritance_pointer()
    {
        p_Operations_Invocations_Variables_Variable->m_inheritance = data::Ptr< data::Operations::Invocations_Variables_Instance >( p_Operations_Invocations_Variables_Variable, this );
    }
    void Invocations_Variables_Instance::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Variables_Variable );
        loader.load( concrete );
    }
    void Invocations_Variables_Instance::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Variables_Variable );
        storer.store( concrete );
    }
    void Invocations_Variables_Instance::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Variables_Instance" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete", concrete } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Variables_Reference : public mega::io::Object
    Invocations_Variables_Reference::Invocations_Variables_Reference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Reference >( loader, this ) )          , p_Operations_Invocations_Variables_Variable( loader )
    {
    }
    Invocations_Variables_Reference::Invocations_Variables_Reference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Concrete::Concrete_Context > >& types)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Reference >( loader, this ) )          , p_Operations_Invocations_Variables_Variable( loader )
          , types( types )
    {
    }
    bool Invocations_Variables_Reference::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Variables_Variable >, data::Ptr< data::Operations::Invocations_Variables_Instance >, data::Ptr< data::Operations::Invocations_Variables_Reference >, data::Ptr< data::Operations::Invocations_Variables_Dimension >, data::Ptr< data::Operations::Invocations_Variables_Context > >{ data::Ptr< data::Operations::Invocations_Variables_Reference >( loader, const_cast< Invocations_Variables_Reference* >( this ) ) };
    }
    void Invocations_Variables_Reference::set_inheritance_pointer()
    {
        p_Operations_Invocations_Variables_Variable->m_inheritance = data::Ptr< data::Operations::Invocations_Variables_Reference >( p_Operations_Invocations_Variables_Variable, this );
    }
    void Invocations_Variables_Reference::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Variables_Variable );
        loader.load( types );
    }
    void Invocations_Variables_Reference::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Variables_Variable );
        storer.store( types );
    }
    void Invocations_Variables_Reference::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Variables_Reference" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "types", types } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Variables_Dimension : public mega::io::Object
    Invocations_Variables_Dimension::Invocations_Variables_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Dimension >( loader, this ) )          , p_Operations_Invocations_Variables_Reference( loader )
    {
    }
    bool Invocations_Variables_Dimension::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Variables_Variable >, data::Ptr< data::Operations::Invocations_Variables_Instance >, data::Ptr< data::Operations::Invocations_Variables_Reference >, data::Ptr< data::Operations::Invocations_Variables_Dimension >, data::Ptr< data::Operations::Invocations_Variables_Context > >{ data::Ptr< data::Operations::Invocations_Variables_Dimension >( loader, const_cast< Invocations_Variables_Dimension* >( this ) ) };
    }
    void Invocations_Variables_Dimension::set_inheritance_pointer()
    {
        p_Operations_Invocations_Variables_Reference->m_inheritance = data::Ptr< data::Operations::Invocations_Variables_Dimension >( p_Operations_Invocations_Variables_Reference, this );
    }
    void Invocations_Variables_Dimension::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Variables_Reference );
    }
    void Invocations_Variables_Dimension::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Variables_Reference );
    }
    void Invocations_Variables_Dimension::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Variables_Dimension" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Variables_Context : public mega::io::Object
    Invocations_Variables_Context::Invocations_Variables_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Context >( loader, this ) )          , p_Operations_Invocations_Variables_Reference( loader )
    {
    }
    bool Invocations_Variables_Context::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Variables_Variable >, data::Ptr< data::Operations::Invocations_Variables_Instance >, data::Ptr< data::Operations::Invocations_Variables_Reference >, data::Ptr< data::Operations::Invocations_Variables_Dimension >, data::Ptr< data::Operations::Invocations_Variables_Context > >{ data::Ptr< data::Operations::Invocations_Variables_Context >( loader, const_cast< Invocations_Variables_Context* >( this ) ) };
    }
    void Invocations_Variables_Context::set_inheritance_pointer()
    {
        p_Operations_Invocations_Variables_Reference->m_inheritance = data::Ptr< data::Operations::Invocations_Variables_Context >( p_Operations_Invocations_Variables_Reference, this );
    }
    void Invocations_Variables_Context::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Variables_Reference );
    }
    void Invocations_Variables_Context::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Variables_Reference );
    }
    void Invocations_Variables_Context::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Variables_Context" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Instructions_Instruction : public mega::io::Object
    Invocations_Instructions_Instruction::Invocations_Instructions_Instruction( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Instruction >( loader, this ) )    {
    }
    bool Invocations_Instructions_Instruction::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_Instruction >( loader, const_cast< Invocations_Instructions_Instruction* >( this ) ) };
    }
    void Invocations_Instructions_Instruction::set_inheritance_pointer()
    {
    }
    void Invocations_Instructions_Instruction::load( mega::io::Loader& loader )
    {
    }
    void Invocations_Instructions_Instruction::store( mega::io::Storer& storer ) const
    {
    }
    void Invocations_Instructions_Instruction::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_Instruction" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Instructions_InstructionGroup : public mega::io::Object
    Invocations_Instructions_InstructionGroup::Invocations_Instructions_InstructionGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >( loader, this ) )          , p_Operations_Invocations_Instructions_Instruction( loader )
    {
    }
    Invocations_Instructions_InstructionGroup::Invocations_Instructions_InstructionGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Operations::Invocations_Instructions_Instruction > >& children)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >( loader, this ) )          , p_Operations_Invocations_Instructions_Instruction( loader )
          , children( children )
    {
    }
    bool Invocations_Instructions_InstructionGroup::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >( loader, const_cast< Invocations_Instructions_InstructionGroup* >( this ) ) };
    }
    void Invocations_Instructions_InstructionGroup::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_Instruction->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >( p_Operations_Invocations_Instructions_Instruction, this );
    }
    void Invocations_Instructions_InstructionGroup::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_Instruction );
        loader.load( children );
    }
    void Invocations_Instructions_InstructionGroup::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_Instruction );
        storer.store( children );
    }
    void Invocations_Instructions_InstructionGroup::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_InstructionGroup" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "children", children } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_Root : public mega::io::Object
    Invocations_Instructions_Root::Invocations_Instructions_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Root >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , context( loader )
    {
    }
    Invocations_Instructions_Root::Invocations_Instructions_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Context >& context)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Root >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , context( context )
    {
    }
    bool Invocations_Instructions_Root::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_Root >( loader, const_cast< Invocations_Instructions_Root* >( this ) ) };
    }
    void Invocations_Instructions_Root::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_Root >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_Root::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( context );
    }
    void Invocations_Instructions_Root::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( context );
    }
    void Invocations_Instructions_Root::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_Root" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "context", context } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_ParentDerivation : public mega::io::Object
    Invocations_Instructions_ParentDerivation::Invocations_Instructions_ParentDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from( loader )
          , to( loader )
    {
    }
    Invocations_Instructions_ParentDerivation::Invocations_Instructions_ParentDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& from, const data::Ptr< data::Operations::Invocations_Variables_Instance >& to)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from( from )
          , to( to )
    {
    }
    bool Invocations_Instructions_ParentDerivation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >( loader, const_cast< Invocations_Instructions_ParentDerivation* >( this ) ) };
    }
    void Invocations_Instructions_ParentDerivation::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_ParentDerivation::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( from );
        loader.load( to );
    }
    void Invocations_Instructions_ParentDerivation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( from );
        storer.store( to );
    }
    void Invocations_Instructions_ParentDerivation::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_ParentDerivation" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "from", from } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "to", to } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_ChildDerivation : public mega::io::Object
    Invocations_Instructions_ChildDerivation::Invocations_Instructions_ChildDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from( loader )
          , to( loader )
    {
    }
    Invocations_Instructions_ChildDerivation::Invocations_Instructions_ChildDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& from, const data::Ptr< data::Operations::Invocations_Variables_Instance >& to)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from( from )
          , to( to )
    {
    }
    bool Invocations_Instructions_ChildDerivation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >( loader, const_cast< Invocations_Instructions_ChildDerivation* >( this ) ) };
    }
    void Invocations_Instructions_ChildDerivation::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_ChildDerivation::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( from );
        loader.load( to );
    }
    void Invocations_Instructions_ChildDerivation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( from );
        storer.store( to );
    }
    void Invocations_Instructions_ChildDerivation::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_ChildDerivation" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "from", from } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "to", to } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_EnumDerivation : public mega::io::Object
    Invocations_Instructions_EnumDerivation::Invocations_Instructions_EnumDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from( loader )
          , to( loader )
    {
    }
    Invocations_Instructions_EnumDerivation::Invocations_Instructions_EnumDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& from, const data::Ptr< data::Operations::Invocations_Variables_Instance >& to)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from( from )
          , to( to )
    {
    }
    bool Invocations_Instructions_EnumDerivation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >( loader, const_cast< Invocations_Instructions_EnumDerivation* >( this ) ) };
    }
    void Invocations_Instructions_EnumDerivation::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_EnumDerivation::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( from );
        loader.load( to );
    }
    void Invocations_Instructions_EnumDerivation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( from );
        storer.store( to );
    }
    void Invocations_Instructions_EnumDerivation::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_EnumDerivation" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "from", from } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "to", to } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_Enumeration : public mega::io::Object
    Invocations_Instructions_Enumeration::Invocations_Instructions_Enumeration( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Enumeration >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , instance( loader )
    {
    }
    Invocations_Instructions_Enumeration::Invocations_Instructions_Enumeration( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& instance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Enumeration >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , instance( instance )
    {
    }
    bool Invocations_Instructions_Enumeration::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_Enumeration >( loader, const_cast< Invocations_Instructions_Enumeration* >( this ) ) };
    }
    void Invocations_Instructions_Enumeration::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_Enumeration >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_Enumeration::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( instance );
    }
    void Invocations_Instructions_Enumeration::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( instance );
    }
    void Invocations_Instructions_Enumeration::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_Enumeration" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "instance", instance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_DimensionReferenceRead : public mega::io::Object
    Invocations_Instructions_DimensionReferenceRead::Invocations_Instructions_DimensionReferenceRead( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , instance( loader )
          , dimension_reference( loader )
          , concrete( loader )
    {
    }
    Invocations_Instructions_DimensionReferenceRead::Invocations_Instructions_DimensionReferenceRead( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& instance, const data::Ptr< data::Operations::Invocations_Variables_Dimension >& dimension_reference, const data::Ptr< data::Concrete::Concrete_Dimension >& concrete)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , instance( instance )
          , dimension_reference( dimension_reference )
          , concrete( concrete )
    {
    }
    bool Invocations_Instructions_DimensionReferenceRead::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >( loader, const_cast< Invocations_Instructions_DimensionReferenceRead* >( this ) ) };
    }
    void Invocations_Instructions_DimensionReferenceRead::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_DimensionReferenceRead::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( instance );
        loader.load( dimension_reference );
        loader.load( concrete );
    }
    void Invocations_Instructions_DimensionReferenceRead::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( instance );
        storer.store( dimension_reference );
        storer.store( concrete );
    }
    void Invocations_Instructions_DimensionReferenceRead::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_DimensionReferenceRead" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "instance", instance } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_reference", dimension_reference } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete", concrete } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_MonoReference : public mega::io::Object
    Invocations_Instructions_MonoReference::Invocations_Instructions_MonoReference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_MonoReference >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , reference( loader )
          , instance( loader )
    {
    }
    Invocations_Instructions_MonoReference::Invocations_Instructions_MonoReference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Reference >& reference, const data::Ptr< data::Operations::Invocations_Variables_Instance >& instance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_MonoReference >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , reference( reference )
          , instance( instance )
    {
    }
    bool Invocations_Instructions_MonoReference::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_MonoReference >( loader, const_cast< Invocations_Instructions_MonoReference* >( this ) ) };
    }
    void Invocations_Instructions_MonoReference::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_MonoReference >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_MonoReference::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( reference );
        loader.load( instance );
    }
    void Invocations_Instructions_MonoReference::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( reference );
        storer.store( instance );
    }
    void Invocations_Instructions_MonoReference::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_MonoReference" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "reference", reference } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "instance", instance } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_PolyReference : public mega::io::Object
    Invocations_Instructions_PolyReference::Invocations_Instructions_PolyReference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_PolyReference >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from_reference( loader )
    {
    }
    Invocations_Instructions_PolyReference::Invocations_Instructions_PolyReference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Reference >& from_reference)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_PolyReference >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from_reference( from_reference )
    {
    }
    bool Invocations_Instructions_PolyReference::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_PolyReference >( loader, const_cast< Invocations_Instructions_PolyReference* >( this ) ) };
    }
    void Invocations_Instructions_PolyReference::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_PolyReference >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_PolyReference::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( from_reference );
    }
    void Invocations_Instructions_PolyReference::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( from_reference );
    }
    void Invocations_Instructions_PolyReference::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_PolyReference" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "from_reference", from_reference } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_PolyCase : public mega::io::Object
    Invocations_Instructions_PolyCase::Invocations_Instructions_PolyCase( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_PolyCase >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , reference( loader )
          , to( loader )
    {
    }
    Invocations_Instructions_PolyCase::Invocations_Instructions_PolyCase( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Reference >& reference, const data::Ptr< data::Operations::Invocations_Variables_Instance >& to)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_PolyCase >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , reference( reference )
          , to( to )
    {
    }
    bool Invocations_Instructions_PolyCase::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_PolyCase >( loader, const_cast< Invocations_Instructions_PolyCase* >( this ) ) };
    }
    void Invocations_Instructions_PolyCase::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_PolyCase >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_PolyCase::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( reference );
        loader.load( to );
    }
    void Invocations_Instructions_PolyCase::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( reference );
        storer.store( to );
    }
    void Invocations_Instructions_PolyCase::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_PolyCase" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "reference", reference } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "to", to } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_Failure : public mega::io::Object
    Invocations_Instructions_Failure::Invocations_Instructions_Failure( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Failure >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
    {
    }
    bool Invocations_Instructions_Failure::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_Failure >( loader, const_cast< Invocations_Instructions_Failure* >( this ) ) };
    }
    void Invocations_Instructions_Failure::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_Failure >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_Failure::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
    }
    void Invocations_Instructions_Failure::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
    }
    void Invocations_Instructions_Failure::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_Failure" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Instructions_Elimination : public mega::io::Object
    Invocations_Instructions_Elimination::Invocations_Instructions_Elimination( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Elimination >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
    {
    }
    bool Invocations_Instructions_Elimination::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_Elimination >( loader, const_cast< Invocations_Instructions_Elimination* >( this ) ) };
    }
    void Invocations_Instructions_Elimination::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_Elimination >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_Elimination::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
    }
    void Invocations_Instructions_Elimination::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
    }
    void Invocations_Instructions_Elimination::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_Elimination" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Instructions_Prune : public mega::io::Object
    Invocations_Instructions_Prune::Invocations_Instructions_Prune( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Prune >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
    {
    }
    bool Invocations_Instructions_Prune::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Instructions_Prune >( loader, const_cast< Invocations_Instructions_Prune* >( this ) ) };
    }
    void Invocations_Instructions_Prune::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_Prune >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_Prune::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
    }
    void Invocations_Instructions_Prune::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
    }
    void Invocations_Instructions_Prune::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_Prune" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_Operation : public mega::io::Object
    Invocations_Operations_Operation::Invocations_Operations_Operation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Operation >( loader, this ) )          , p_Operations_Invocations_Instructions_Instruction( loader )
          , instance( loader )
    {
    }
    Invocations_Operations_Operation::Invocations_Operations_Operation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Instance >& instance, const std::vector< data::Ptr< data::Operations::Operations_InterfaceVariant > >& return_types, const std::vector< data::Ptr< data::Operations::Operations_InterfaceVariant > >& parameter_types)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Operation >( loader, this ) )          , p_Operations_Invocations_Instructions_Instruction( loader )
          , instance( instance )
          , return_types( return_types )
          , parameter_types( parameter_types )
    {
    }
    bool Invocations_Operations_Operation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_Operation >( loader, const_cast< Invocations_Operations_Operation* >( this ) ) };
    }
    void Invocations_Operations_Operation::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_Instruction->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_Operation >( p_Operations_Invocations_Instructions_Instruction, this );
    }
    void Invocations_Operations_Operation::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_Instruction );
        loader.load( instance );
        loader.load( return_types );
        loader.load( parameter_types );
    }
    void Invocations_Operations_Operation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_Instruction );
        storer.store( instance );
        storer.store( return_types );
        storer.store( parameter_types );
    }
    void Invocations_Operations_Operation::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_Operation" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "instance", instance } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "return_types", return_types } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "parameter_types", parameter_types } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Operations_BasicOperation : public mega::io::Object
    Invocations_Operations_BasicOperation::Invocations_Operations_BasicOperation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_BasicOperation >( loader, this ) )          , p_Operations_Invocations_Operations_Operation( loader )
          , interface( loader )
          , concrete_target( loader )
    {
    }
    Invocations_Operations_BasicOperation::Invocations_Operations_BasicOperation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_IContext >& interface, const data::Ptr< data::Concrete::Concrete_Context >& concrete_target)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_BasicOperation >( loader, this ) )          , p_Operations_Invocations_Operations_Operation( loader )
          , interface( interface )
          , concrete_target( concrete_target )
    {
    }
    bool Invocations_Operations_BasicOperation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_BasicOperation >( loader, const_cast< Invocations_Operations_BasicOperation* >( this ) ) };
    }
    void Invocations_Operations_BasicOperation::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_Operation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_BasicOperation >( p_Operations_Invocations_Operations_Operation, this );
    }
    void Invocations_Operations_BasicOperation::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_Operation );
        loader.load( interface );
        loader.load( concrete_target );
    }
    void Invocations_Operations_BasicOperation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_Operation );
        storer.store( interface );
        storer.store( concrete_target );
    }
    void Invocations_Operations_BasicOperation::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_BasicOperation" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface", interface } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete_target", concrete_target } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Operations_DimensionOperation : public mega::io::Object
    Invocations_Operations_DimensionOperation::Invocations_Operations_DimensionOperation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >( loader, this ) )          , p_Operations_Invocations_Operations_Operation( loader )
          , interface_dimension( loader )
          , concrete_dimension( loader )
    {
    }
    Invocations_Operations_DimensionOperation::Invocations_Operations_DimensionOperation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_DimensionTrait >& interface_dimension, const data::Ptr< data::Concrete::Concrete_Dimension >& concrete_dimension)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >( loader, this ) )          , p_Operations_Invocations_Operations_Operation( loader )
          , interface_dimension( interface_dimension )
          , concrete_dimension( concrete_dimension )
    {
    }
    bool Invocations_Operations_DimensionOperation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >( loader, const_cast< Invocations_Operations_DimensionOperation* >( this ) ) };
    }
    void Invocations_Operations_DimensionOperation::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_Operation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >( p_Operations_Invocations_Operations_Operation, this );
    }
    void Invocations_Operations_DimensionOperation::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_Operation );
        loader.load( interface_dimension );
        loader.load( concrete_dimension );
    }
    void Invocations_Operations_DimensionOperation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_Operation );
        storer.store( interface_dimension );
        storer.store( concrete_dimension );
    }
    void Invocations_Operations_DimensionOperation::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_DimensionOperation" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_dimension", interface_dimension } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete_dimension", concrete_dimension } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Operations_Call : public mega::io::Object
    Invocations_Operations_Call::Invocations_Operations_Call( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Call >( loader, this ) )          , p_Operations_Invocations_Operations_BasicOperation( loader )
    {
    }
    bool Invocations_Operations_Call::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_Call >( loader, const_cast< Invocations_Operations_Call* >( this ) ) };
    }
    void Invocations_Operations_Call::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_BasicOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_Call >( p_Operations_Invocations_Operations_BasicOperation, this );
    }
    void Invocations_Operations_Call::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Call::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Call::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_Call" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_Start : public mega::io::Object
    Invocations_Operations_Start::Invocations_Operations_Start( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Start >( loader, this ) )          , p_Operations_Invocations_Operations_BasicOperation( loader )
    {
    }
    bool Invocations_Operations_Start::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_Start >( loader, const_cast< Invocations_Operations_Start* >( this ) ) };
    }
    void Invocations_Operations_Start::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_BasicOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_Start >( p_Operations_Invocations_Operations_BasicOperation, this );
    }
    void Invocations_Operations_Start::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Start::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Start::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_Start" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_Stop : public mega::io::Object
    Invocations_Operations_Stop::Invocations_Operations_Stop( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Stop >( loader, this ) )          , p_Operations_Invocations_Operations_BasicOperation( loader )
    {
    }
    bool Invocations_Operations_Stop::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_Stop >( loader, const_cast< Invocations_Operations_Stop* >( this ) ) };
    }
    void Invocations_Operations_Stop::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_BasicOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_Stop >( p_Operations_Invocations_Operations_BasicOperation, this );
    }
    void Invocations_Operations_Stop::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Stop::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Stop::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_Stop" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_Pause : public mega::io::Object
    Invocations_Operations_Pause::Invocations_Operations_Pause( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Pause >( loader, this ) )          , p_Operations_Invocations_Operations_BasicOperation( loader )
    {
    }
    bool Invocations_Operations_Pause::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_Pause >( loader, const_cast< Invocations_Operations_Pause* >( this ) ) };
    }
    void Invocations_Operations_Pause::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_BasicOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_Pause >( p_Operations_Invocations_Operations_BasicOperation, this );
    }
    void Invocations_Operations_Pause::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Pause::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Pause::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_Pause" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_Resume : public mega::io::Object
    Invocations_Operations_Resume::Invocations_Operations_Resume( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Resume >( loader, this ) )          , p_Operations_Invocations_Operations_BasicOperation( loader )
    {
    }
    bool Invocations_Operations_Resume::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_Resume >( loader, const_cast< Invocations_Operations_Resume* >( this ) ) };
    }
    void Invocations_Operations_Resume::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_BasicOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_Resume >( p_Operations_Invocations_Operations_BasicOperation, this );
    }
    void Invocations_Operations_Resume::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Resume::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Resume::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_Resume" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_Done : public mega::io::Object
    Invocations_Operations_Done::Invocations_Operations_Done( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Done >( loader, this ) )          , p_Operations_Invocations_Operations_BasicOperation( loader )
    {
    }
    bool Invocations_Operations_Done::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_Done >( loader, const_cast< Invocations_Operations_Done* >( this ) ) };
    }
    void Invocations_Operations_Done::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_BasicOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_Done >( p_Operations_Invocations_Operations_BasicOperation, this );
    }
    void Invocations_Operations_Done::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Done::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Done::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_Done" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_WaitAction : public mega::io::Object
    Invocations_Operations_WaitAction::Invocations_Operations_WaitAction( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_WaitAction >( loader, this ) )          , p_Operations_Invocations_Operations_BasicOperation( loader )
    {
    }
    bool Invocations_Operations_WaitAction::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_WaitAction >( loader, const_cast< Invocations_Operations_WaitAction* >( this ) ) };
    }
    void Invocations_Operations_WaitAction::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_BasicOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_WaitAction >( p_Operations_Invocations_Operations_BasicOperation, this );
    }
    void Invocations_Operations_WaitAction::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_WaitAction::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_WaitAction::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_WaitAction" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_WaitDimension : public mega::io::Object
    Invocations_Operations_WaitDimension::Invocations_Operations_WaitDimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_WaitDimension >( loader, this ) )          , p_Operations_Invocations_Operations_DimensionOperation( loader )
    {
    }
    bool Invocations_Operations_WaitDimension::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_WaitDimension >( loader, const_cast< Invocations_Operations_WaitDimension* >( this ) ) };
    }
    void Invocations_Operations_WaitDimension::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_DimensionOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_WaitDimension >( p_Operations_Invocations_Operations_DimensionOperation, this );
    }
    void Invocations_Operations_WaitDimension::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_DimensionOperation );
    }
    void Invocations_Operations_WaitDimension::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_DimensionOperation );
    }
    void Invocations_Operations_WaitDimension::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_WaitDimension" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_GetAction : public mega::io::Object
    Invocations_Operations_GetAction::Invocations_Operations_GetAction( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_GetAction >( loader, this ) )          , p_Operations_Invocations_Operations_BasicOperation( loader )
    {
    }
    bool Invocations_Operations_GetAction::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_GetAction >( loader, const_cast< Invocations_Operations_GetAction* >( this ) ) };
    }
    void Invocations_Operations_GetAction::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_BasicOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_GetAction >( p_Operations_Invocations_Operations_BasicOperation, this );
    }
    void Invocations_Operations_GetAction::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_GetAction::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_GetAction::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_GetAction" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_GetDimension : public mega::io::Object
    Invocations_Operations_GetDimension::Invocations_Operations_GetDimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_GetDimension >( loader, this ) )          , p_Operations_Invocations_Operations_DimensionOperation( loader )
    {
    }
    bool Invocations_Operations_GetDimension::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_GetDimension >( loader, const_cast< Invocations_Operations_GetDimension* >( this ) ) };
    }
    void Invocations_Operations_GetDimension::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_DimensionOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_GetDimension >( p_Operations_Invocations_Operations_DimensionOperation, this );
    }
    void Invocations_Operations_GetDimension::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_DimensionOperation );
    }
    void Invocations_Operations_GetDimension::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_DimensionOperation );
    }
    void Invocations_Operations_GetDimension::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_GetDimension" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_Read : public mega::io::Object
    Invocations_Operations_Read::Invocations_Operations_Read( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Read >( loader, this ) )          , p_Operations_Invocations_Operations_DimensionOperation( loader )
    {
    }
    bool Invocations_Operations_Read::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_Read >( loader, const_cast< Invocations_Operations_Read* >( this ) ) };
    }
    void Invocations_Operations_Read::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_DimensionOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_Read >( p_Operations_Invocations_Operations_DimensionOperation, this );
    }
    void Invocations_Operations_Read::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_DimensionOperation );
    }
    void Invocations_Operations_Read::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_DimensionOperation );
    }
    void Invocations_Operations_Read::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_Read" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_Write : public mega::io::Object
    Invocations_Operations_Write::Invocations_Operations_Write( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Write >( loader, this ) )          , p_Operations_Invocations_Operations_DimensionOperation( loader )
    {
    }
    bool Invocations_Operations_Write::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_Write >( loader, const_cast< Invocations_Operations_Write* >( this ) ) };
    }
    void Invocations_Operations_Write::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_DimensionOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_Write >( p_Operations_Invocations_Operations_DimensionOperation, this );
    }
    void Invocations_Operations_Write::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_DimensionOperation );
    }
    void Invocations_Operations_Write::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_DimensionOperation );
    }
    void Invocations_Operations_Write::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_Write" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Operations_WriteLink : public mega::io::Object
    Invocations_Operations_WriteLink::Invocations_Operations_WriteLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_WriteLink >( loader, this ) )          , p_Operations_Invocations_Operations_DimensionOperation( loader )
          , dimension_reference( loader )
    {
    }
    Invocations_Operations_WriteLink::Invocations_Operations_WriteLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Dimension >& dimension_reference)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_WriteLink >( loader, this ) )          , p_Operations_Invocations_Operations_DimensionOperation( loader )
          , dimension_reference( dimension_reference )
    {
    }
    bool Invocations_Operations_WriteLink::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_WriteLink >( loader, const_cast< Invocations_Operations_WriteLink* >( this ) ) };
    }
    void Invocations_Operations_WriteLink::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_DimensionOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_WriteLink >( p_Operations_Invocations_Operations_DimensionOperation, this );
    }
    void Invocations_Operations_WriteLink::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_DimensionOperation );
        loader.load( dimension_reference );
    }
    void Invocations_Operations_WriteLink::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_DimensionOperation );
        storer.store( dimension_reference );
    }
    void Invocations_Operations_WriteLink::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_WriteLink" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_reference", dimension_reference } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Operations_Range : public mega::io::Object
    Invocations_Operations_Range::Invocations_Operations_Range( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Range >( loader, this ) )          , p_Operations_Invocations_Operations_BasicOperation( loader )
    {
    }
    bool Invocations_Operations_Range::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Invocations_Instructions_Instruction >, data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >, data::Ptr< data::Operations::Invocations_Instructions_Root >, data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >, data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >, data::Ptr< data::Operations::Invocations_Instructions_EnumDerivation >, data::Ptr< data::Operations::Invocations_Instructions_Enumeration >, data::Ptr< data::Operations::Invocations_Instructions_DimensionReferenceRead >, data::Ptr< data::Operations::Invocations_Instructions_MonoReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyReference >, data::Ptr< data::Operations::Invocations_Instructions_PolyCase >, data::Ptr< data::Operations::Invocations_Instructions_Failure >, data::Ptr< data::Operations::Invocations_Instructions_Elimination >, data::Ptr< data::Operations::Invocations_Instructions_Prune >, data::Ptr< data::Operations::Invocations_Operations_Operation >, data::Ptr< data::Operations::Invocations_Operations_BasicOperation >, data::Ptr< data::Operations::Invocations_Operations_DimensionOperation >, data::Ptr< data::Operations::Invocations_Operations_Call >, data::Ptr< data::Operations::Invocations_Operations_Start >, data::Ptr< data::Operations::Invocations_Operations_Stop >, data::Ptr< data::Operations::Invocations_Operations_Pause >, data::Ptr< data::Operations::Invocations_Operations_Resume >, data::Ptr< data::Operations::Invocations_Operations_Done >, data::Ptr< data::Operations::Invocations_Operations_WaitAction >, data::Ptr< data::Operations::Invocations_Operations_WaitDimension >, data::Ptr< data::Operations::Invocations_Operations_GetAction >, data::Ptr< data::Operations::Invocations_Operations_GetDimension >, data::Ptr< data::Operations::Invocations_Operations_Read >, data::Ptr< data::Operations::Invocations_Operations_Write >, data::Ptr< data::Operations::Invocations_Operations_WriteLink >, data::Ptr< data::Operations::Invocations_Operations_Range > >{ data::Ptr< data::Operations::Invocations_Operations_Range >( loader, const_cast< Invocations_Operations_Range* >( this ) ) };
    }
    void Invocations_Operations_Range::set_inheritance_pointer()
    {
        p_Operations_Invocations_Operations_BasicOperation->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_Range >( p_Operations_Invocations_Operations_BasicOperation, this );
    }
    void Invocations_Operations_Range::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Range::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Operations_BasicOperation );
    }
    void Invocations_Operations_Range::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Invocations_Operations_Range" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_InterfaceVariant : public mega::io::Object
    Operations_InterfaceVariant::Operations_InterfaceVariant( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_InterfaceVariant >( loader, this ) )    {
    }
    Operations_InterfaceVariant::Operations_InterfaceVariant( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::optional< data::Ptr< data::Tree::Interface_IContext > >& context, const std::optional< data::Ptr< data::Tree::Interface_DimensionTrait > >& dimension)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_InterfaceVariant >( loader, this ) )          , context( context )
          , dimension( dimension )
    {
    }
    bool Operations_InterfaceVariant::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Operations_InterfaceVariant > >{ data::Ptr< data::Operations::Operations_InterfaceVariant >( loader, const_cast< Operations_InterfaceVariant* >( this ) ) };
    }
    void Operations_InterfaceVariant::set_inheritance_pointer()
    {
    }
    void Operations_InterfaceVariant::load( mega::io::Loader& loader )
    {
        loader.load( context );
        loader.load( dimension );
    }
    void Operations_InterfaceVariant::store( mega::io::Storer& storer ) const
    {
        storer.store( context );
        storer.store( dimension );
    }
    void Operations_InterfaceVariant::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Operations_InterfaceVariant" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "context", context } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension", dimension } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_ConcreteVariant : public mega::io::Object
    Operations_ConcreteVariant::Operations_ConcreteVariant( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ConcreteVariant >( loader, this ) )    {
    }
    Operations_ConcreteVariant::Operations_ConcreteVariant( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::optional< data::Ptr< data::Concrete::Concrete_Context > >& context, const std::optional< data::Ptr< data::Concrete::Concrete_Dimension > >& dimension)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ConcreteVariant >( loader, this ) )          , context( context )
          , dimension( dimension )
    {
    }
    bool Operations_ConcreteVariant::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Operations_ConcreteVariant > >{ data::Ptr< data::Operations::Operations_ConcreteVariant >( loader, const_cast< Operations_ConcreteVariant* >( this ) ) };
    }
    void Operations_ConcreteVariant::set_inheritance_pointer()
    {
    }
    void Operations_ConcreteVariant::load( mega::io::Loader& loader )
    {
        loader.load( context );
        loader.load( dimension );
    }
    void Operations_ConcreteVariant::store( mega::io::Storer& storer ) const
    {
        storer.store( context );
        storer.store( dimension );
    }
    void Operations_ConcreteVariant::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Operations_ConcreteVariant" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "context", context } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension", dimension } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_Element : public mega::io::Object
    Operations_Element::Operations_Element( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Element >( loader, this ) )          , interface( loader )
          , concrete( loader )
    {
    }
    Operations_Element::Operations_Element( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Operations_InterfaceVariant >& interface, const data::Ptr< data::Operations::Operations_ConcreteVariant >& concrete)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Element >( loader, this ) )          , interface( interface )
          , concrete( concrete )
    {
    }
    bool Operations_Element::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Operations_Element > >{ data::Ptr< data::Operations::Operations_Element >( loader, const_cast< Operations_Element* >( this ) ) };
    }
    void Operations_Element::set_inheritance_pointer()
    {
    }
    void Operations_Element::load( mega::io::Loader& loader )
    {
        loader.load( interface );
        loader.load( concrete );
    }
    void Operations_Element::store( mega::io::Storer& storer ) const
    {
        storer.store( interface );
        storer.store( concrete );
    }
    void Operations_Element::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Operations_Element" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface", interface } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete", concrete } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_ElementVector : public mega::io::Object
    Operations_ElementVector::Operations_ElementVector( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ElementVector >( loader, this ) )    {
    }
    Operations_ElementVector::Operations_ElementVector( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Operations::Operations_Element > >& elements)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ElementVector >( loader, this ) )          , elements( elements )
    {
    }
    bool Operations_ElementVector::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Operations_ElementVector > >{ data::Ptr< data::Operations::Operations_ElementVector >( loader, const_cast< Operations_ElementVector* >( this ) ) };
    }
    void Operations_ElementVector::set_inheritance_pointer()
    {
    }
    void Operations_ElementVector::load( mega::io::Loader& loader )
    {
        loader.load( elements );
    }
    void Operations_ElementVector::store( mega::io::Storer& storer ) const
    {
        storer.store( elements );
    }
    void Operations_ElementVector::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Operations_ElementVector" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "elements", elements } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_Context : public mega::io::Object
    Operations_Context::Operations_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Context >( loader, this ) )    {
    }
    Operations_Context::Operations_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Operations::Operations_ElementVector > >& vectors)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Context >( loader, this ) )          , vectors( vectors )
    {
    }
    bool Operations_Context::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Operations_Context > >{ data::Ptr< data::Operations::Operations_Context >( loader, const_cast< Operations_Context* >( this ) ) };
    }
    void Operations_Context::set_inheritance_pointer()
    {
    }
    void Operations_Context::load( mega::io::Loader& loader )
    {
        loader.load( vectors );
    }
    void Operations_Context::store( mega::io::Storer& storer ) const
    {
        storer.store( vectors );
    }
    void Operations_Context::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Operations_Context" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "vectors", vectors } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_TypePath : public mega::io::Object
    Operations_TypePath::Operations_TypePath( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_TypePath >( loader, this ) )    {
    }
    Operations_TypePath::Operations_TypePath( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Operations::Operations_ElementVector > >& vectors)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_TypePath >( loader, this ) )          , vectors( vectors )
    {
    }
    bool Operations_TypePath::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Operations_TypePath > >{ data::Ptr< data::Operations::Operations_TypePath >( loader, const_cast< Operations_TypePath* >( this ) ) };
    }
    void Operations_TypePath::set_inheritance_pointer()
    {
    }
    void Operations_TypePath::load( mega::io::Loader& loader )
    {
        loader.load( vectors );
    }
    void Operations_TypePath::store( mega::io::Storer& storer ) const
    {
        storer.store( vectors );
    }
    void Operations_TypePath::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Operations_TypePath" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "vectors", vectors } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_NameRoot : public mega::io::Object
    Operations_NameRoot::Operations_NameRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_NameRoot >( loader, this ) )    {
    }
    Operations_NameRoot::Operations_NameRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Operations::Operations_Name > >& children)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_NameRoot >( loader, this ) )          , children( children )
    {
    }
    bool Operations_NameRoot::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Operations_NameRoot >, data::Ptr< data::Operations::Operations_Name > >{ data::Ptr< data::Operations::Operations_NameRoot >( loader, const_cast< Operations_NameRoot* >( this ) ) };
    }
    void Operations_NameRoot::set_inheritance_pointer()
    {
    }
    void Operations_NameRoot::load( mega::io::Loader& loader )
    {
        loader.load( children );
    }
    void Operations_NameRoot::store( mega::io::Storer& storer ) const
    {
        storer.store( children );
    }
    void Operations_NameRoot::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Operations_NameRoot" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "children", children } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_Name : public mega::io::Object
    Operations_Name::Operations_Name( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Name >( loader, this ) )          , p_Operations_Operations_NameRoot( loader )
          , element( loader )
    {
    }
    Operations_Name::Operations_Name( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Operations_Element >& element, const bool& is_member, const bool& is_reference)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Name >( loader, this ) )          , p_Operations_Operations_NameRoot( loader )
          , element( element )
          , is_member( is_member )
          , is_reference( is_reference )
    {
    }
    bool Operations_Name::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Operations_NameRoot >, data::Ptr< data::Operations::Operations_Name > >{ data::Ptr< data::Operations::Operations_Name >( loader, const_cast< Operations_Name* >( this ) ) };
    }
    void Operations_Name::set_inheritance_pointer()
    {
        p_Operations_Operations_NameRoot->m_inheritance = data::Ptr< data::Operations::Operations_Name >( p_Operations_Operations_NameRoot, this );
    }
    void Operations_Name::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_NameRoot );
        loader.load( element );
        loader.load( is_member );
        loader.load( is_reference );
    }
    void Operations_Name::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_NameRoot );
        storer.store( element );
        storer.store( is_member );
        storer.store( is_reference );
    }
    void Operations_Name::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Operations_Name" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "element", element } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "is_member", is_member } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "is_reference", is_reference } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_NameResolution : public mega::io::Object
    Operations_NameResolution::Operations_NameResolution( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_NameResolution >( loader, this ) )          , root_name( loader )
    {
    }
    Operations_NameResolution::Operations_NameResolution( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Operations_NameRoot >& root_name)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_NameResolution >( loader, this ) )          , root_name( root_name )
    {
    }
    bool Operations_NameResolution::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Operations_NameResolution > >{ data::Ptr< data::Operations::Operations_NameResolution >( loader, const_cast< Operations_NameResolution* >( this ) ) };
    }
    void Operations_NameResolution::set_inheritance_pointer()
    {
    }
    void Operations_NameResolution::load( mega::io::Loader& loader )
    {
        loader.load( root_name );
    }
    void Operations_NameResolution::store( mega::io::Storer& storer ) const
    {
        storer.store( root_name );
    }
    void Operations_NameResolution::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Operations_NameResolution" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "root_name", root_name } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_Invocation : public mega::io::Object
    Operations_Invocation::Operations_Invocation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Invocation >( loader, this ) )          , context( loader )
          , type_path( loader )
          , name_resolution( loader )
          , root_variable( loader )
          , root_instruction( loader )
    {
    }
    Operations_Invocation::Operations_Invocation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Operations_Context >& context, const data::Ptr< data::Operations::Operations_TypePath >& type_path, const mega::OperationID& operation, const std::string& name, const std::string& context_str, const std::string& type_path_str)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Invocation >( loader, this ) )          , context( context )
          , type_path( type_path )
          , operation( operation )
          , name( name )
          , context_str( context_str )
          , type_path_str( type_path_str )
    {
    }
    bool Operations_Invocation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Operations_Invocation > >{ data::Ptr< data::Operations::Operations_Invocation >( loader, const_cast< Operations_Invocation* >( this ) ) };
    }
    void Operations_Invocation::set_inheritance_pointer()
    {
    }
    void Operations_Invocation::load( mega::io::Loader& loader )
    {
        loader.load( context );
        loader.load( type_path );
        loader.load( operation );
        loader.load( name );
        loader.load( context_str );
        loader.load( type_path_str );
        loader.load( return_type_str );
        loader.load( name_resolution );
        loader.load( root_variable );
        loader.load( root_instruction );
        loader.load( return_types_context );
        loader.load( return_types_dimension );
        loader.load( homogeneous );
        loader.load( is_function_call );
    }
    void Operations_Invocation::store( mega::io::Storer& storer ) const
    {
        storer.store( context );
        storer.store( type_path );
        storer.store( operation );
        storer.store( name );
        storer.store( context_str );
        storer.store( type_path_str );
        VERIFY_RTE_MSG( return_type_str.has_value(), "Operations::Operations_Invocation.return_type_str has NOT been set" );
        storer.store( return_type_str );
        VERIFY_RTE_MSG( name_resolution.has_value(), "Operations::Operations_Invocation.name_resolution has NOT been set" );
        storer.store( name_resolution );
        VERIFY_RTE_MSG( root_variable.has_value(), "Operations::Operations_Invocation.root_variable has NOT been set" );
        storer.store( root_variable );
        VERIFY_RTE_MSG( root_instruction.has_value(), "Operations::Operations_Invocation.root_instruction has NOT been set" );
        storer.store( root_instruction );
        VERIFY_RTE_MSG( return_types_context.has_value(), "Operations::Operations_Invocation.return_types_context has NOT been set" );
        storer.store( return_types_context );
        VERIFY_RTE_MSG( return_types_dimension.has_value(), "Operations::Operations_Invocation.return_types_dimension has NOT been set" );
        storer.store( return_types_dimension );
        VERIFY_RTE_MSG( homogeneous.has_value(), "Operations::Operations_Invocation.homogeneous has NOT been set" );
        storer.store( homogeneous );
        VERIFY_RTE_MSG( is_function_call.has_value(), "Operations::Operations_Invocation.is_function_call has NOT been set" );
        storer.store( is_function_call );
    }
    void Operations_Invocation::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Operations_Invocation" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "context", context } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "type_path", type_path } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "operation", operation } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "name", name } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "context_str", context_str } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "type_path_str", type_path_str } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "return_type_str", return_type_str.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "name_resolution", name_resolution.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "root_variable", root_variable.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "root_instruction", root_instruction.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "return_types_context", return_types_context.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "return_types_dimension", return_types_dimension.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "homogeneous", homogeneous.value() } } );
            part[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "is_function_call", is_function_call.value() } } );
            part[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_Invocations : public mega::io::Object
    Operations_Invocations::Operations_Invocations( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Invocations >( loader, this ) )    {
    }
    Operations_Invocations::Operations_Invocations( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::map< mega::invocation::ID, data::Ptr< data::Operations::Operations_Invocation > >& invocations)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Invocations >( loader, this ) )          , invocations( invocations )
    {
    }
    bool Operations_Invocations::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == std::variant< data::Ptr< data::Operations::Operations_Invocations > >{ data::Ptr< data::Operations::Operations_Invocations >( loader, const_cast< Operations_Invocations* >( this ) ) };
    }
    void Operations_Invocations::set_inheritance_pointer()
    {
    }
    void Operations_Invocations::load( mega::io::Loader& loader )
    {
        loader.load( invocations );
    }
    void Operations_Invocations::store( mega::io::Storer& storer ) const
    {
        storer.store( invocations );
    }
    void Operations_Invocations::to_json( nlohmann::json& part ) const
    {
        part = nlohmann::json::object(
            { 
                { "partname", "Operations_Invocations" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "invocations", invocations } } );
            part[ "properties" ].push_back( property );
        }
    }
        
}


mega::io::Object* Factory::create( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
{
    switch( objectInfo.getType() )
    {
        case 0: return new Components::Components_Component( loader, objectInfo );
        case 1: return new AST::Parser_Identifier( loader, objectInfo );
        case 2: return new AST::Parser_ScopedIdentifier( loader, objectInfo );
        case 3: return new AST::Parser_ArgumentList( loader, objectInfo );
        case 4: return new AST::Parser_ReturnType( loader, objectInfo );
        case 5: return new AST::Parser_Inheritance( loader, objectInfo );
        case 6: return new AST::Parser_Size( loader, objectInfo );
        case 7: return new AST::Parser_Link( loader, objectInfo );
        case 8: return new AST::Parser_Dimension( loader, objectInfo );
        case 9: return new AST::Parser_Include( loader, objectInfo );
        case 10: return new AST::Parser_SystemInclude( loader, objectInfo );
        case 11: return new AST::Parser_MegaInclude( loader, objectInfo );
        case 12: return new AST::Parser_MegaIncludeInline( loader, objectInfo );
        case 13: return new AST::Parser_MegaIncludeNested( loader, objectInfo );
        case 14: return new AST::Parser_CPPInclude( loader, objectInfo );
        case 15: return new AST::Parser_Dependency( loader, objectInfo );
        case 16: return new AST::Parser_ContextDef( loader, objectInfo );
        case 18: return new AST::Parser_NamespaceDef( loader, objectInfo );
        case 19: return new AST::Parser_AbstractDef( loader, objectInfo );
        case 20: return new AST::Parser_ActionDef( loader, objectInfo );
        case 21: return new AST::Parser_EventDef( loader, objectInfo );
        case 22: return new AST::Parser_FunctionDef( loader, objectInfo );
        case 23: return new AST::Parser_ObjectDef( loader, objectInfo );
        case 24: return new AST::Parser_LinkDef( loader, objectInfo );
        case 25: return new AST::Parser_TableDef( loader, objectInfo );
        case 26: return new AST::Parser_SourceRoot( loader, objectInfo );
        case 27: return new AST::Parser_IncludeRoot( loader, objectInfo );
        case 28: return new AST::Parser_ObjectSourceRoot( loader, objectInfo );
        case 17: return new Body::Parser_ContextDef( loader, objectInfo );
        case 29: return new Tree::Interface_DimensionTrait( loader, objectInfo );
        case 33: return new Tree::Interface_InheritanceTrait( loader, objectInfo );
        case 35: return new Tree::Interface_LinkTrait( loader, objectInfo );
        case 36: return new Tree::Interface_ReturnTypeTrait( loader, objectInfo );
        case 38: return new Tree::Interface_ArgumentListTrait( loader, objectInfo );
        case 40: return new Tree::Interface_SizeTrait( loader, objectInfo );
        case 42: return new Tree::Interface_ContextGroup( loader, objectInfo );
        case 43: return new Tree::Interface_Root( loader, objectInfo );
        case 44: return new Tree::Interface_IContext( loader, objectInfo );
        case 47: return new Tree::Interface_Namespace( loader, objectInfo );
        case 48: return new Tree::Interface_Abstract( loader, objectInfo );
        case 49: return new Tree::Interface_Action( loader, objectInfo );
        case 50: return new Tree::Interface_Event( loader, objectInfo );
        case 51: return new Tree::Interface_Function( loader, objectInfo );
        case 52: return new Tree::Interface_Object( loader, objectInfo );
        case 53: return new Tree::Interface_Link( loader, objectInfo );
        case 54: return new Tree::Interface_Table( loader, objectInfo );
        case 113: return new DPGraph::Dependencies_Glob( loader, objectInfo );
        case 114: return new DPGraph::Dependencies_ObjectDependencies( loader, objectInfo );
        case 115: return new DPGraph::Dependencies_Analysis( loader, objectInfo );
        case 116: return new SymbolTable::Symbols_Symbol( loader, objectInfo );
        case 117: return new SymbolTable::Symbols_SymbolSet( loader, objectInfo );
        case 118: return new SymbolTable::Symbols_SymbolTable( loader, objectInfo );
        case 30: return new PerSourceSymbols::Interface_DimensionTrait( loader, objectInfo );
        case 45: return new PerSourceSymbols::Interface_IContext( loader, objectInfo );
        case 32: return new Clang::Interface_DimensionTrait( loader, objectInfo );
        case 34: return new Clang::Interface_InheritanceTrait( loader, objectInfo );
        case 37: return new Clang::Interface_ReturnTypeTrait( loader, objectInfo );
        case 39: return new Clang::Interface_ArgumentListTrait( loader, objectInfo );
        case 41: return new Clang::Interface_SizeTrait( loader, objectInfo );
        case 31: return new Concrete::Interface_DimensionTrait( loader, objectInfo );
        case 46: return new Concrete::Interface_IContext( loader, objectInfo );
        case 91: return new Concrete::Concrete_Dimension( loader, objectInfo );
        case 92: return new Concrete::Concrete_ContextGroup( loader, objectInfo );
        case 93: return new Concrete::Concrete_Context( loader, objectInfo );
        case 94: return new Concrete::Concrete_Namespace( loader, objectInfo );
        case 95: return new Concrete::Concrete_Action( loader, objectInfo );
        case 96: return new Concrete::Concrete_Event( loader, objectInfo );
        case 97: return new Concrete::Concrete_Function( loader, objectInfo );
        case 98: return new Concrete::Concrete_Object( loader, objectInfo );
        case 99: return new Concrete::Concrete_Link( loader, objectInfo );
        case 100: return new Concrete::Concrete_Table( loader, objectInfo );
        case 101: return new Concrete::Concrete_Root( loader, objectInfo );
        case 119: return new Model::HyperGraph_ObjectGraph( loader, objectInfo );
        case 120: return new Model::HyperGraph_Graph( loader, objectInfo );
        case 121: return new Derivations::Derivation_ObjectMapping( loader, objectInfo );
        case 122: return new Derivations::Derivation_Mapping( loader, objectInfo );
        case 55: return new Operations::Invocations_Variables_Variable( loader, objectInfo );
        case 56: return new Operations::Invocations_Variables_Instance( loader, objectInfo );
        case 57: return new Operations::Invocations_Variables_Reference( loader, objectInfo );
        case 58: return new Operations::Invocations_Variables_Dimension( loader, objectInfo );
        case 59: return new Operations::Invocations_Variables_Context( loader, objectInfo );
        case 60: return new Operations::Invocations_Instructions_Instruction( loader, objectInfo );
        case 61: return new Operations::Invocations_Instructions_InstructionGroup( loader, objectInfo );
        case 62: return new Operations::Invocations_Instructions_Root( loader, objectInfo );
        case 63: return new Operations::Invocations_Instructions_ParentDerivation( loader, objectInfo );
        case 64: return new Operations::Invocations_Instructions_ChildDerivation( loader, objectInfo );
        case 65: return new Operations::Invocations_Instructions_EnumDerivation( loader, objectInfo );
        case 66: return new Operations::Invocations_Instructions_Enumeration( loader, objectInfo );
        case 67: return new Operations::Invocations_Instructions_DimensionReferenceRead( loader, objectInfo );
        case 68: return new Operations::Invocations_Instructions_MonoReference( loader, objectInfo );
        case 69: return new Operations::Invocations_Instructions_PolyReference( loader, objectInfo );
        case 70: return new Operations::Invocations_Instructions_PolyCase( loader, objectInfo );
        case 71: return new Operations::Invocations_Instructions_Failure( loader, objectInfo );
        case 72: return new Operations::Invocations_Instructions_Elimination( loader, objectInfo );
        case 73: return new Operations::Invocations_Instructions_Prune( loader, objectInfo );
        case 74: return new Operations::Invocations_Operations_Operation( loader, objectInfo );
        case 75: return new Operations::Invocations_Operations_BasicOperation( loader, objectInfo );
        case 76: return new Operations::Invocations_Operations_DimensionOperation( loader, objectInfo );
        case 77: return new Operations::Invocations_Operations_Call( loader, objectInfo );
        case 78: return new Operations::Invocations_Operations_Start( loader, objectInfo );
        case 79: return new Operations::Invocations_Operations_Stop( loader, objectInfo );
        case 80: return new Operations::Invocations_Operations_Pause( loader, objectInfo );
        case 81: return new Operations::Invocations_Operations_Resume( loader, objectInfo );
        case 82: return new Operations::Invocations_Operations_Done( loader, objectInfo );
        case 83: return new Operations::Invocations_Operations_WaitAction( loader, objectInfo );
        case 84: return new Operations::Invocations_Operations_WaitDimension( loader, objectInfo );
        case 85: return new Operations::Invocations_Operations_GetAction( loader, objectInfo );
        case 86: return new Operations::Invocations_Operations_GetDimension( loader, objectInfo );
        case 87: return new Operations::Invocations_Operations_Read( loader, objectInfo );
        case 88: return new Operations::Invocations_Operations_Write( loader, objectInfo );
        case 89: return new Operations::Invocations_Operations_WriteLink( loader, objectInfo );
        case 90: return new Operations::Invocations_Operations_Range( loader, objectInfo );
        case 102: return new Operations::Operations_InterfaceVariant( loader, objectInfo );
        case 103: return new Operations::Operations_ConcreteVariant( loader, objectInfo );
        case 104: return new Operations::Operations_Element( loader, objectInfo );
        case 105: return new Operations::Operations_ElementVector( loader, objectInfo );
        case 106: return new Operations::Operations_Context( loader, objectInfo );
        case 107: return new Operations::Operations_TypePath( loader, objectInfo );
        case 108: return new Operations::Operations_NameRoot( loader, objectInfo );
        case 109: return new Operations::Operations_Name( loader, objectInfo );
        case 110: return new Operations::Operations_NameResolution( loader, objectInfo );
        case 111: return new Operations::Operations_Invocation( loader, objectInfo );
        case 112: return new Operations::Operations_Invocations( loader, objectInfo );
        default:
            THROW_RTE( "Unrecognised object type ID" );
    }
}

}
