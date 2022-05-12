
#include "database/model/data.hxx"

#include "nlohmann/json.hpp"

namespace nlohmann
{
    template < typename T > struct adl_serializer< std::optional< T > >
    {
        static void to_json( json &j, const std::optional< T > &opt )
        {
            if( !opt.has_value() )
            {
                j = nullptr;
            }
            else
            {
                j = opt.value();
            }
        }
        static void from_json( const json &j, std::optional< T > &opt )
        {
            if( j.is_null() )
            {
                opt = std::optional< T >();
            }
            else
            {
                opt = j.get< T >();
            }
        }
    };
} // namespace nlohmann

namespace data
{

    namespace Components
    {
        // struct Component : public mega::io::Object
        Component::Component( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Components::Component >( loader, this ) )
        {
        }
        Component::Component( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &name,
                              const boost::filesystem::path &directory, const std::vector< std::string > &cpp_flags,
                              const std::vector< std::string > &cpp_defines, const std::vector< boost::filesystem::path > &includeDirectories,
                              const std::vector< boost::filesystem::path > &sourceFiles )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Components::Component >( loader, this ) ), name( name ), directory( directory ),
              cpp_flags( cpp_flags ), cpp_defines( cpp_defines ), includeDirectories( includeDirectories ), sourceFiles( sourceFiles )
        {
        }
        bool Component::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Components::Component > >{
                                        data::Ptr< data::Components::Component >( loader, const_cast< Component * >( this ) ) };
        }
        void Component::set_inheritance_pointer() {}
        void Component::load( mega::io::Loader &loader )
        {
            loader.load( name );
            loader.load( directory );
            loader.load( cpp_flags );
            loader.load( cpp_defines );
            loader.load( includeDirectories );
            loader.load( sourceFiles );
        }
        void Component::store( mega::io::Storer &storer ) const
        {
            storer.store( name );
            storer.store( directory );
            storer.store( cpp_flags );
            storer.store( cpp_defines );
            storer.store( includeDirectories );
            storer.store( sourceFiles );
        }
        void Component::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Component" },
                                             { "filetype", "Components" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "name", name } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "directory", directory } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "cpp_flags", cpp_flags } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "cpp_defines", cpp_defines } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "includeDirectories", includeDirectories } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "sourceFiles", sourceFiles } } );
                part[ "properties" ].push_back( property );
            }
        }

    } // namespace Components
    namespace AST
    {
        // struct Identifier : public mega::io::Object
        Identifier::Identifier( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Identifier >( loader, this ) )
        {
        }
        Identifier::Identifier( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Identifier >( loader, this ) ), str( str )
        {
        }
        bool Identifier::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::AST::Identifier > >{ data::Ptr< data::AST::Identifier >( loader, const_cast< Identifier * >( this ) ) };
        }
        void Identifier::set_inheritance_pointer() {}
        void Identifier::load( mega::io::Loader &loader ) { loader.load( str ); }
        void Identifier::store( mega::io::Storer &storer ) const { storer.store( str ); }
        void Identifier::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Identifier" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "str", str } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct ScopedIdentifier : public mega::io::Object
        ScopedIdentifier::ScopedIdentifier( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ScopedIdentifier >( loader, this ) )
        {
        }
        ScopedIdentifier::ScopedIdentifier( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                                            const std::vector< data::Ptr< data::AST::Identifier > > &ids, const std::string &source_file,
                                            const std::size_t &line_number )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ScopedIdentifier >( loader, this ) ), ids( ids ), source_file( source_file ),
              line_number( line_number )
        {
        }
        bool ScopedIdentifier::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ScopedIdentifier > >{
                                        data::Ptr< data::AST::ScopedIdentifier >( loader, const_cast< ScopedIdentifier * >( this ) ) };
        }
        void ScopedIdentifier::set_inheritance_pointer() {}
        void ScopedIdentifier::load( mega::io::Loader &loader )
        {
            loader.load( ids );
            loader.load( source_file );
            loader.load( line_number );
        }
        void ScopedIdentifier::store( mega::io::Storer &storer ) const
        {
            storer.store( ids );
            storer.store( source_file );
            storer.store( line_number );
        }
        void ScopedIdentifier::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ScopedIdentifier" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "ids", ids } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "source_file", source_file } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "line_number", line_number } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct ArgumentList : public mega::io::Object
        ArgumentList::ArgumentList( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ArgumentList >( loader, this ) )
        {
        }
        ArgumentList::ArgumentList( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ArgumentList >( loader, this ) ), str( str )
        {
        }
        bool ArgumentList::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ArgumentList >, data::Ptr< data::Tree::ArgumentListTrait > >{
                                        data::Ptr< data::AST::ArgumentList >( loader, const_cast< ArgumentList * >( this ) ) };
        }
        void ArgumentList::set_inheritance_pointer() {}
        void ArgumentList::load( mega::io::Loader &loader ) { loader.load( str ); }
        void ArgumentList::store( mega::io::Storer &storer ) const { storer.store( str ); }
        void ArgumentList::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ArgumentList" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "str", str } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct ReturnType : public mega::io::Object
        ReturnType::ReturnType( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ReturnType >( loader, this ) )
        {
        }
        ReturnType::ReturnType( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ReturnType >( loader, this ) ), str( str )
        {
        }
        bool ReturnType::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ReturnType >, data::Ptr< data::Tree::ReturnTypeTrait > >{
                                        data::Ptr< data::AST::ReturnType >( loader, const_cast< ReturnType * >( this ) ) };
        }
        void ReturnType::set_inheritance_pointer() {}
        void ReturnType::load( mega::io::Loader &loader ) { loader.load( str ); }
        void ReturnType::store( mega::io::Storer &storer ) const { storer.store( str ); }
        void ReturnType::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ReturnType" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "str", str } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Inheritance : public mega::io::Object
        Inheritance::Inheritance( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Inheritance >( loader, this ) )
        {
        }
        Inheritance::Inheritance( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< std::string > &strings )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Inheritance >( loader, this ) ), strings( strings )
        {
        }
        bool Inheritance::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::Inheritance >, data::Ptr< data::Tree::InheritanceTrait > >{
                                        data::Ptr< data::AST::Inheritance >( loader, const_cast< Inheritance * >( this ) ) };
        }
        void Inheritance::set_inheritance_pointer() {}
        void Inheritance::load( mega::io::Loader &loader ) { loader.load( strings ); }
        void Inheritance::store( mega::io::Storer &storer ) const { storer.store( strings ); }
        void Inheritance::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Inheritance" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "strings", strings } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Size : public mega::io::Object
        Size::Size( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Size >( loader, this ) )
        {
        }
        Size::Size( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Size >( loader, this ) ), str( str )
        {
        }
        bool Size::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::Size >, data::Ptr< data::Tree::SizeTrait > >{
                                        data::Ptr< data::AST::Size >( loader, const_cast< Size * >( this ) ) };
        }
        void Size::set_inheritance_pointer() {}
        void Size::load( mega::io::Loader &loader ) { loader.load( str ); }
        void Size::store( mega::io::Storer &storer ) const { storer.store( str ); }
        void Size::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Size" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "str", str } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Dimension : public mega::io::Object
        Dimension::Dimension( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Dimension >( loader, this ) ), id( loader )
        {
        }
        Dimension::Dimension( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const bool &isConst,
                              const data::Ptr< data::AST::Identifier > &id, const std::string &type )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Dimension >( loader, this ) ), isConst( isConst ), id( id ), type( type )
        {
        }
        bool Dimension::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::Dimension >, data::Ptr< data::Tree::DimensionTrait > >{
                                        data::Ptr< data::AST::Dimension >( loader, const_cast< Dimension * >( this ) ) };
        }
        void Dimension::set_inheritance_pointer() {}
        void Dimension::load( mega::io::Loader &loader )
        {
            loader.load( isConst );
            loader.load( id );
            loader.load( type );
        }
        void Dimension::store( mega::io::Storer &storer ) const
        {
            storer.store( isConst );
            storer.store( id );
            storer.store( type );
        }
        void Dimension::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Dimension" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "isConst", isConst } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "id", id } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "type", type } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Include : public mega::io::Object
        Include::Include( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Include >( loader, this ) )
        {
        }
        bool Include::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                                 data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >{
                       data::Ptr< data::AST::Include >( loader, const_cast< Include * >( this ) ) };
        }
        void Include::set_inheritance_pointer() {}
        void Include::load( mega::io::Loader &loader ) {}
        void Include::store( mega::io::Storer &storer ) const {}
        void Include::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Include" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
        }

        // struct SystemInclude : public mega::io::Object
        SystemInclude::SystemInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::SystemInclude >( loader, this ) ), p_AST_Include( loader )
        {
        }
        SystemInclude::SystemInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::SystemInclude >( loader, this ) ), p_AST_Include( loader ), str( str )
        {
        }
        bool SystemInclude::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                                 data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >{
                       data::Ptr< data::AST::SystemInclude >( loader, const_cast< SystemInclude * >( this ) ) };
        }
        void SystemInclude::set_inheritance_pointer() { p_AST_Include->m_inheritance = data::Ptr< data::AST::SystemInclude >( p_AST_Include, this ); }
        void SystemInclude::load( mega::io::Loader &loader )
        {
            loader.load( p_AST_Include );
            loader.load( str );
        }
        void SystemInclude::store( mega::io::Storer &storer ) const
        {
            storer.store( p_AST_Include );
            storer.store( str );
        }
        void SystemInclude::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "SystemInclude" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "str", str } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct MegaInclude : public mega::io::Object
        MegaInclude::MegaInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::MegaInclude >( loader, this ) ), p_AST_Include( loader ), root( loader )
        {
        }
        MegaInclude::MegaInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const boost::filesystem::path &megaSourceFilePath )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::MegaInclude >( loader, this ) ), p_AST_Include( loader ),
              megaSourceFilePath( megaSourceFilePath )
        {
        }
        bool MegaInclude::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                                 data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >{
                       data::Ptr< data::AST::MegaInclude >( loader, const_cast< MegaInclude * >( this ) ) };
        }
        void MegaInclude::set_inheritance_pointer() { p_AST_Include->m_inheritance = data::Ptr< data::AST::MegaInclude >( p_AST_Include, this ); }
        void MegaInclude::load( mega::io::Loader &loader )
        {
            loader.load( p_AST_Include );
            loader.load( megaSourceFilePath );
            loader.load( root );
        }
        void MegaInclude::store( mega::io::Storer &storer ) const
        {
            storer.store( p_AST_Include );
            storer.store( megaSourceFilePath );
            VERIFY_RTE_MSG( root.has_value(), "AST::MegaInclude.root has NOT been set" );
            storer.store( root );
        }
        void MegaInclude::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "MegaInclude" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "megaSourceFilePath", megaSourceFilePath } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "root", root.value() } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct MegaIncludeInline : public mega::io::Object
        MegaIncludeInline::MegaIncludeInline( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::MegaIncludeInline >( loader, this ) ), p_AST_MegaInclude( loader )
        {
        }
        bool MegaIncludeInline::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                                 data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >{
                       data::Ptr< data::AST::MegaIncludeInline >( loader, const_cast< MegaIncludeInline * >( this ) ) };
        }
        void MegaIncludeInline::set_inheritance_pointer()
        {
            p_AST_MegaInclude->m_inheritance = data::Ptr< data::AST::MegaIncludeInline >( p_AST_MegaInclude, this );
        }
        void MegaIncludeInline::load( mega::io::Loader &loader ) { loader.load( p_AST_MegaInclude ); }
        void MegaIncludeInline::store( mega::io::Storer &storer ) const { storer.store( p_AST_MegaInclude ); }
        void MegaIncludeInline::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "MegaIncludeInline" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
        }

        // struct MegaIncludeNested : public mega::io::Object
        MegaIncludeNested::MegaIncludeNested( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::MegaIncludeNested >( loader, this ) ), p_AST_MegaInclude( loader ),
              id( loader )
        {
        }
        MegaIncludeNested::MegaIncludeNested( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                                              const data::Ptr< data::AST::ScopedIdentifier > &id )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::MegaIncludeNested >( loader, this ) ), p_AST_MegaInclude( loader ), id( id )
        {
        }
        bool MegaIncludeNested::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                                 data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >{
                       data::Ptr< data::AST::MegaIncludeNested >( loader, const_cast< MegaIncludeNested * >( this ) ) };
        }
        void MegaIncludeNested::set_inheritance_pointer()
        {
            p_AST_MegaInclude->m_inheritance = data::Ptr< data::AST::MegaIncludeNested >( p_AST_MegaInclude, this );
        }
        void MegaIncludeNested::load( mega::io::Loader &loader )
        {
            loader.load( p_AST_MegaInclude );
            loader.load( id );
        }
        void MegaIncludeNested::store( mega::io::Storer &storer ) const
        {
            storer.store( p_AST_MegaInclude );
            storer.store( id );
        }
        void MegaIncludeNested::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "MegaIncludeNested" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "id", id } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct CPPInclude : public mega::io::Object
        CPPInclude::CPPInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::CPPInclude >( loader, this ) ), p_AST_Include( loader )
        {
        }
        CPPInclude::CPPInclude( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const boost::filesystem::path &cppSourceFilePath )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::CPPInclude >( loader, this ) ), p_AST_Include( loader ),
              cppSourceFilePath( cppSourceFilePath )
        {
        }
        bool CPPInclude::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::AST::Include >, data::Ptr< data::AST::SystemInclude >, data::Ptr< data::AST::MegaInclude >,
                                 data::Ptr< data::AST::MegaIncludeInline >, data::Ptr< data::AST::MegaIncludeNested >, data::Ptr< data::AST::CPPInclude > >{
                       data::Ptr< data::AST::CPPInclude >( loader, const_cast< CPPInclude * >( this ) ) };
        }
        void CPPInclude::set_inheritance_pointer() { p_AST_Include->m_inheritance = data::Ptr< data::AST::CPPInclude >( p_AST_Include, this ); }
        void CPPInclude::load( mega::io::Loader &loader )
        {
            loader.load( p_AST_Include );
            loader.load( cppSourceFilePath );
        }
        void CPPInclude::store( mega::io::Storer &storer ) const
        {
            storer.store( p_AST_Include );
            storer.store( cppSourceFilePath );
        }
        void CPPInclude::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "CPPInclude" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "cppSourceFilePath", cppSourceFilePath } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Dependency : public mega::io::Object
        Dependency::Dependency( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Dependency >( loader, this ) )
        {
        }
        Dependency::Dependency( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &str )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Dependency >( loader, this ) ), str( str )
        {
        }
        bool Dependency::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::AST::Dependency > >{ data::Ptr< data::AST::Dependency >( loader, const_cast< Dependency * >( this ) ) };
        }
        void Dependency::set_inheritance_pointer() {}
        void Dependency::load( mega::io::Loader &loader ) { loader.load( str ); }
        void Dependency::store( mega::io::Storer &storer ) const { storer.store( str ); }
        void Dependency::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Dependency" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "str", str } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct ContextDef : public mega::io::Object
        ContextDef::ContextDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ContextDef >( loader, this ) ), p_Body_ContextDef( loader ), id( loader )
        {
        }
        ContextDef::ContextDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::ScopedIdentifier > &id,
                                const std::vector< data::Ptr< data::AST::ContextDef > > &children,
                                const std::vector< data::Ptr< data::AST::Dimension > >  &dimensions,
                                const std::vector< data::Ptr< data::AST::Include > >    &includes,
                                const std::vector< data::Ptr< data::AST::Dependency > > &dependencies )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ContextDef >( loader, this ) ), p_Body_ContextDef( loader ), id( id ),
              children( children ), dimensions( dimensions ), includes( includes ), dependencies( dependencies )
        {
        }
        bool ContextDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                                                  data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                                                  data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >{
                                        data::Ptr< data::AST::ContextDef >( loader, const_cast< ContextDef * >( this ) ) };
        }
        void ContextDef::set_inheritance_pointer() {}
        void ContextDef::load( mega::io::Loader &loader )
        {
            loader.load( p_Body_ContextDef );
            loader.load( id );
            loader.load( children );
            loader.load( dimensions );
            loader.load( includes );
            loader.load( dependencies );
        }
        void ContextDef::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Body_ContextDef );
            storer.store( id );
            storer.store( children );
            storer.store( dimensions );
            storer.store( includes );
            storer.store( dependencies );
        }
        void ContextDef::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ContextDef" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "id", id } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "children", children } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimensions", dimensions } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "includes", includes } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dependencies", dependencies } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct NamespaceDef : public mega::io::Object
        NamespaceDef::NamespaceDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::NamespaceDef >( loader, this ) ), p_AST_ContextDef( loader )
        {
        }
        bool NamespaceDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                                                  data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                                                  data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >{
                                        data::Ptr< data::AST::NamespaceDef >( loader, const_cast< NamespaceDef * >( this ) ) };
        }
        void NamespaceDef::set_inheritance_pointer() { p_AST_ContextDef->m_inheritance = data::Ptr< data::AST::NamespaceDef >( p_AST_ContextDef, this ); }
        void NamespaceDef::load( mega::io::Loader &loader ) { loader.load( p_AST_ContextDef ); }
        void NamespaceDef::store( mega::io::Storer &storer ) const { storer.store( p_AST_ContextDef ); }
        void NamespaceDef::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "NamespaceDef" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
        }

        // struct AbstractDef : public mega::io::Object
        AbstractDef::AbstractDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::AbstractDef >( loader, this ) ), p_AST_ContextDef( loader ),
              inheritance( loader )
        {
        }
        AbstractDef::AbstractDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::Inheritance > &inheritance )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::AbstractDef >( loader, this ) ), p_AST_ContextDef( loader ),
              inheritance( inheritance )
        {
        }
        bool AbstractDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                                                  data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                                                  data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >{
                                        data::Ptr< data::AST::AbstractDef >( loader, const_cast< AbstractDef * >( this ) ) };
        }
        void AbstractDef::set_inheritance_pointer() { p_AST_ContextDef->m_inheritance = data::Ptr< data::AST::AbstractDef >( p_AST_ContextDef, this ); }
        void AbstractDef::load( mega::io::Loader &loader )
        {
            loader.load( p_AST_ContextDef );
            loader.load( inheritance );
        }
        void AbstractDef::store( mega::io::Storer &storer ) const
        {
            storer.store( p_AST_ContextDef );
            storer.store( inheritance );
        }
        void AbstractDef::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "AbstractDef" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "inheritance", inheritance } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct ActionDef : public mega::io::Object
        ActionDef::ActionDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ActionDef >( loader, this ) ), p_AST_ContextDef( loader ), size( loader ),
              inheritance( loader )
        {
        }
        ActionDef::ActionDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::Size > &size,
                              const data::Ptr< data::AST::Inheritance > &inheritance )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ActionDef >( loader, this ) ), p_AST_ContextDef( loader ), size( size ),
              inheritance( inheritance )
        {
        }
        bool ActionDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                                                  data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                                                  data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >{
                                        data::Ptr< data::AST::ActionDef >( loader, const_cast< ActionDef * >( this ) ) };
        }
        void ActionDef::set_inheritance_pointer() { p_AST_ContextDef->m_inheritance = data::Ptr< data::AST::ActionDef >( p_AST_ContextDef, this ); }
        void ActionDef::load( mega::io::Loader &loader )
        {
            loader.load( p_AST_ContextDef );
            loader.load( size );
            loader.load( inheritance );
        }
        void ActionDef::store( mega::io::Storer &storer ) const
        {
            storer.store( p_AST_ContextDef );
            storer.store( size );
            storer.store( inheritance );
        }
        void ActionDef::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ActionDef" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "size", size } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "inheritance", inheritance } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct EventDef : public mega::io::Object
        EventDef::EventDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::EventDef >( loader, this ) ), p_AST_ContextDef( loader ), size( loader ),
              inheritance( loader )
        {
        }
        EventDef::EventDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::Size > &size,
                            const data::Ptr< data::AST::Inheritance > &inheritance )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::EventDef >( loader, this ) ), p_AST_ContextDef( loader ), size( size ),
              inheritance( inheritance )
        {
        }
        bool EventDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                                                  data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                                                  data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >{
                                        data::Ptr< data::AST::EventDef >( loader, const_cast< EventDef * >( this ) ) };
        }
        void EventDef::set_inheritance_pointer() { p_AST_ContextDef->m_inheritance = data::Ptr< data::AST::EventDef >( p_AST_ContextDef, this ); }
        void EventDef::load( mega::io::Loader &loader )
        {
            loader.load( p_AST_ContextDef );
            loader.load( size );
            loader.load( inheritance );
        }
        void EventDef::store( mega::io::Storer &storer ) const
        {
            storer.store( p_AST_ContextDef );
            storer.store( size );
            storer.store( inheritance );
        }
        void EventDef::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "EventDef" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "size", size } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "inheritance", inheritance } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct FunctionDef : public mega::io::Object
        FunctionDef::FunctionDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::FunctionDef >( loader, this ) ), p_AST_ContextDef( loader ),
              argumentList( loader ), returnType( loader )
        {
        }
        FunctionDef::FunctionDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::ArgumentList > &argumentList,
                                  const data::Ptr< data::AST::ReturnType > &returnType )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::FunctionDef >( loader, this ) ), p_AST_ContextDef( loader ),
              argumentList( argumentList ), returnType( returnType )
        {
        }
        bool FunctionDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                                                  data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                                                  data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >{
                                        data::Ptr< data::AST::FunctionDef >( loader, const_cast< FunctionDef * >( this ) ) };
        }
        void FunctionDef::set_inheritance_pointer() { p_AST_ContextDef->m_inheritance = data::Ptr< data::AST::FunctionDef >( p_AST_ContextDef, this ); }
        void FunctionDef::load( mega::io::Loader &loader )
        {
            loader.load( p_AST_ContextDef );
            loader.load( argumentList );
            loader.load( returnType );
        }
        void FunctionDef::store( mega::io::Storer &storer ) const
        {
            storer.store( p_AST_ContextDef );
            storer.store( argumentList );
            storer.store( returnType );
        }
        void FunctionDef::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "FunctionDef" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "argumentList", argumentList } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "returnType", returnType } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct ObjectDef : public mega::io::Object
        ObjectDef::ObjectDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ObjectDef >( loader, this ) ), p_AST_ContextDef( loader ),
              inheritance( loader )
        {
        }
        ObjectDef::ObjectDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::Inheritance > &inheritance )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ObjectDef >( loader, this ) ), p_AST_ContextDef( loader ),
              inheritance( inheritance )
        {
        }
        bool ObjectDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                                                  data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                                                  data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >{
                                        data::Ptr< data::AST::ObjectDef >( loader, const_cast< ObjectDef * >( this ) ) };
        }
        void ObjectDef::set_inheritance_pointer() { p_AST_ContextDef->m_inheritance = data::Ptr< data::AST::ObjectDef >( p_AST_ContextDef, this ); }
        void ObjectDef::load( mega::io::Loader &loader )
        {
            loader.load( p_AST_ContextDef );
            loader.load( inheritance );
        }
        void ObjectDef::store( mega::io::Storer &storer ) const
        {
            storer.store( p_AST_ContextDef );
            storer.store( inheritance );
        }
        void ObjectDef::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ObjectDef" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "inheritance", inheritance } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct LinkDef : public mega::io::Object
        LinkDef::LinkDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::LinkDef >( loader, this ) ), p_AST_ContextDef( loader ), target( loader )
        {
        }
        LinkDef::LinkDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const mega::CardinalityRange &linker,
                          const mega::CardinalityRange &linkee, const bool &derive_from, const bool &derive_to,
                          const data::Ptr< data::AST::Inheritance > &target )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::LinkDef >( loader, this ) ), p_AST_ContextDef( loader ), linker( linker ),
              linkee( linkee ), derive_from( derive_from ), derive_to( derive_to ), target( target )
        {
        }
        bool LinkDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                                                  data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                                                  data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >{
                                        data::Ptr< data::AST::LinkDef >( loader, const_cast< LinkDef * >( this ) ) };
        }
        void LinkDef::set_inheritance_pointer() { p_AST_ContextDef->m_inheritance = data::Ptr< data::AST::LinkDef >( p_AST_ContextDef, this ); }
        void LinkDef::load( mega::io::Loader &loader )
        {
            loader.load( p_AST_ContextDef );
            loader.load( linker );
            loader.load( linkee );
            loader.load( derive_from );
            loader.load( derive_to );
            loader.load( target );
        }
        void LinkDef::store( mega::io::Storer &storer ) const
        {
            storer.store( p_AST_ContextDef );
            storer.store( linker );
            storer.store( linkee );
            storer.store( derive_from );
            storer.store( derive_to );
            storer.store( target );
        }
        void LinkDef::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "LinkDef" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "linker", linker } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "linkee", linkee } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "derive_from", derive_from } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "derive_to", derive_to } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "target", target } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct TableDef : public mega::io::Object
        TableDef::TableDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::TableDef >( loader, this ) ), p_AST_ContextDef( loader ),
              inheritance( loader )
        {
        }
        TableDef::TableDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::Inheritance > &inheritance )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::TableDef >( loader, this ) ), p_AST_ContextDef( loader ),
              inheritance( inheritance )
        {
        }
        bool TableDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ContextDef >, data::Ptr< data::AST::NamespaceDef >, data::Ptr< data::AST::AbstractDef >,
                                                  data::Ptr< data::AST::ActionDef >, data::Ptr< data::AST::EventDef >, data::Ptr< data::AST::FunctionDef >,
                                                  data::Ptr< data::AST::ObjectDef >, data::Ptr< data::AST::LinkDef >, data::Ptr< data::AST::TableDef > >{
                                        data::Ptr< data::AST::TableDef >( loader, const_cast< TableDef * >( this ) ) };
        }
        void TableDef::set_inheritance_pointer() { p_AST_ContextDef->m_inheritance = data::Ptr< data::AST::TableDef >( p_AST_ContextDef, this ); }
        void TableDef::load( mega::io::Loader &loader )
        {
            loader.load( p_AST_ContextDef );
            loader.load( inheritance );
        }
        void TableDef::store( mega::io::Storer &storer ) const
        {
            storer.store( p_AST_ContextDef );
            storer.store( inheritance );
        }
        void TableDef::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "TableDef" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "inheritance", inheritance } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct SourceRoot : public mega::io::Object
        SourceRoot::SourceRoot( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::SourceRoot >( loader, this ) ), component( loader ), ast( loader )
        {
        }
        SourceRoot::SourceRoot( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const boost::filesystem::path &sourceFile,
                                const data::Ptr< data::Components::Component > &component, const data::Ptr< data::AST::ContextDef > &ast )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::SourceRoot >( loader, this ) ), sourceFile( sourceFile ),
              component( component ), ast( ast )
        {
        }
        bool SourceRoot::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::AST::SourceRoot >, data::Ptr< data::AST::IncludeRoot >, data::Ptr< data::AST::ObjectSourceRoot > >{
                       data::Ptr< data::AST::SourceRoot >( loader, const_cast< SourceRoot * >( this ) ) };
        }
        void SourceRoot::set_inheritance_pointer() {}
        void SourceRoot::load( mega::io::Loader &loader )
        {
            loader.load( sourceFile );
            loader.load( component );
            loader.load( ast );
        }
        void SourceRoot::store( mega::io::Storer &storer ) const
        {
            storer.store( sourceFile );
            storer.store( component );
            storer.store( ast );
        }
        void SourceRoot::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "SourceRoot" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "sourceFile", sourceFile } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "component", component } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "ast", ast } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct IncludeRoot : public mega::io::Object
        IncludeRoot::IncludeRoot( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::IncludeRoot >( loader, this ) ), p_AST_SourceRoot( loader )
        {
        }
        bool IncludeRoot::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::AST::SourceRoot >, data::Ptr< data::AST::IncludeRoot >, data::Ptr< data::AST::ObjectSourceRoot > >{
                       data::Ptr< data::AST::IncludeRoot >( loader, const_cast< IncludeRoot * >( this ) ) };
        }
        void IncludeRoot::set_inheritance_pointer() { p_AST_SourceRoot->m_inheritance = data::Ptr< data::AST::IncludeRoot >( p_AST_SourceRoot, this ); }
        void IncludeRoot::load( mega::io::Loader &loader ) { loader.load( p_AST_SourceRoot ); }
        void IncludeRoot::store( mega::io::Storer &storer ) const { storer.store( p_AST_SourceRoot ); }
        void IncludeRoot::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "IncludeRoot" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
        }

        // struct ObjectSourceRoot : public mega::io::Object
        ObjectSourceRoot::ObjectSourceRoot( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::ObjectSourceRoot >( loader, this ) ), p_AST_SourceRoot( loader )
        {
        }
        bool ObjectSourceRoot::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::AST::SourceRoot >, data::Ptr< data::AST::IncludeRoot >, data::Ptr< data::AST::ObjectSourceRoot > >{
                       data::Ptr< data::AST::ObjectSourceRoot >( loader, const_cast< ObjectSourceRoot * >( this ) ) };
        }
        void ObjectSourceRoot::set_inheritance_pointer()
        {
            p_AST_SourceRoot->m_inheritance = data::Ptr< data::AST::ObjectSourceRoot >( p_AST_SourceRoot, this );
        }
        void ObjectSourceRoot::load( mega::io::Loader &loader ) { loader.load( p_AST_SourceRoot ); }
        void ObjectSourceRoot::store( mega::io::Storer &storer ) const { storer.store( p_AST_SourceRoot ); }
        void ObjectSourceRoot::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ObjectSourceRoot" },
                                             { "filetype", "AST" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
        }

    } // namespace AST
    namespace Body
    {
        // struct ContextDef : public mega::io::Object
        ContextDef::ContextDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo ) : mega::io::Object( objectInfo ) {}
        ContextDef::ContextDef( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &body )
            : mega::io::Object( objectInfo ), body( body )
        {
        }
        bool ContextDef::test_inheritance_pointer( ObjectPartLoader &loader ) const { return false; }
        void ContextDef::set_inheritance_pointer() {}
        void ContextDef::load( mega::io::Loader &loader ) { loader.load( body ); }
        void ContextDef::store( mega::io::Storer &storer ) const { storer.store( body ); }
        void ContextDef::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ContextDef" },
                                             { "filetype", "Body" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "body", body } } );
                part[ "properties" ].push_back( property );
            }
        }

    } // namespace Body
    namespace Tree
    {
        // struct DimensionTrait : public mega::io::Object
        DimensionTrait::DimensionTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::DimensionTrait >( loader, this ) ), p_AST_Dimension( loader ),
              p_PerSourceSymbols_DimensionTrait( loader )
        {
        }
        bool DimensionTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::Dimension >, data::Ptr< data::Tree::DimensionTrait > >{
                                        data::Ptr< data::Tree::DimensionTrait >( loader, const_cast< DimensionTrait * >( this ) ) };
        }
        void DimensionTrait::set_inheritance_pointer() { p_AST_Dimension->m_inheritance = data::Ptr< data::Tree::DimensionTrait >( p_AST_Dimension, this ); }
        void DimensionTrait::load( mega::io::Loader &loader ) { loader.load( p_AST_Dimension ); }
        void DimensionTrait::store( mega::io::Storer &storer ) const { storer.store( p_AST_Dimension ); }
        void DimensionTrait::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "DimensionTrait" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
        }

        // struct InheritanceTrait : public mega::io::Object
        InheritanceTrait::InheritanceTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::InheritanceTrait >( loader, this ) ), p_AST_Inheritance( loader ),
              p_Clang_InheritanceTrait( loader )
        {
        }
        bool InheritanceTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::Inheritance >, data::Ptr< data::Tree::InheritanceTrait > >{
                                        data::Ptr< data::Tree::InheritanceTrait >( loader, const_cast< InheritanceTrait * >( this ) ) };
        }
        void InheritanceTrait::set_inheritance_pointer()
        {
            p_AST_Inheritance->m_inheritance = data::Ptr< data::Tree::InheritanceTrait >( p_AST_Inheritance, this );
        }
        void InheritanceTrait::load( mega::io::Loader &loader ) { loader.load( p_AST_Inheritance ); }
        void InheritanceTrait::store( mega::io::Storer &storer ) const { storer.store( p_AST_Inheritance ); }
        void InheritanceTrait::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "InheritanceTrait" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
        }

        // struct ReturnTypeTrait : public mega::io::Object
        ReturnTypeTrait::ReturnTypeTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::ReturnTypeTrait >( loader, this ) ), p_AST_ReturnType( loader )
        {
        }
        bool ReturnTypeTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ReturnType >, data::Ptr< data::Tree::ReturnTypeTrait > >{
                                        data::Ptr< data::Tree::ReturnTypeTrait >( loader, const_cast< ReturnTypeTrait * >( this ) ) };
        }
        void ReturnTypeTrait::set_inheritance_pointer()
        {
            p_AST_ReturnType->m_inheritance = data::Ptr< data::Tree::ReturnTypeTrait >( p_AST_ReturnType, this );
        }
        void ReturnTypeTrait::load( mega::io::Loader &loader ) { loader.load( p_AST_ReturnType ); }
        void ReturnTypeTrait::store( mega::io::Storer &storer ) const { storer.store( p_AST_ReturnType ); }
        void ReturnTypeTrait::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ReturnTypeTrait" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
        }

        // struct ArgumentListTrait : public mega::io::Object
        ArgumentListTrait::ArgumentListTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::ArgumentListTrait >( loader, this ) ), p_AST_ArgumentList( loader )
        {
        }
        bool ArgumentListTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::ArgumentList >, data::Ptr< data::Tree::ArgumentListTrait > >{
                                        data::Ptr< data::Tree::ArgumentListTrait >( loader, const_cast< ArgumentListTrait * >( this ) ) };
        }
        void ArgumentListTrait::set_inheritance_pointer()
        {
            p_AST_ArgumentList->m_inheritance = data::Ptr< data::Tree::ArgumentListTrait >( p_AST_ArgumentList, this );
        }
        void ArgumentListTrait::load( mega::io::Loader &loader ) { loader.load( p_AST_ArgumentList ); }
        void ArgumentListTrait::store( mega::io::Storer &storer ) const { storer.store( p_AST_ArgumentList ); }
        void ArgumentListTrait::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ArgumentListTrait" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
        }

        // struct SizeTrait : public mega::io::Object
        SizeTrait::SizeTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::SizeTrait >( loader, this ) ), p_AST_Size( loader )
        {
        }
        bool SizeTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::AST::Size >, data::Ptr< data::Tree::SizeTrait > >{
                                        data::Ptr< data::Tree::SizeTrait >( loader, const_cast< SizeTrait * >( this ) ) };
        }
        void SizeTrait::set_inheritance_pointer() { p_AST_Size->m_inheritance = data::Ptr< data::Tree::SizeTrait >( p_AST_Size, this ); }
        void SizeTrait::load( mega::io::Loader &loader ) { loader.load( p_AST_Size ); }
        void SizeTrait::store( mega::io::Storer &storer ) const { storer.store( p_AST_Size ); }
        void SizeTrait::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "SizeTrait" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
        }

        // struct ContextGroup : public mega::io::Object
        ContextGroup::ContextGroup( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::ContextGroup >( loader, this ) )
        {
        }
        ContextGroup::ContextGroup( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                                    const std::vector< data::Ptr< data::Tree::Context > > &children )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::ContextGroup >( loader, this ) ), children( children )
        {
        }
        bool ContextGroup::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::Context >,
                                                  data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                                                  data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >,
                                                  data::Ptr< data::Tree::Link >, data::Ptr< data::Tree::Table > >{
                                        data::Ptr< data::Tree::ContextGroup >( loader, const_cast< ContextGroup * >( this ) ) };
        }
        void ContextGroup::set_inheritance_pointer() {}
        void ContextGroup::load( mega::io::Loader &loader ) { loader.load( children ); }
        void ContextGroup::store( mega::io::Storer &storer ) const { storer.store( children ); }
        void ContextGroup::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ContextGroup" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "children", children } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Root : public mega::io::Object
        Root::Root( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Root >( loader, this ) ), p_Tree_ContextGroup( loader ), root( loader )
        {
        }
        Root::Root( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::AST::ObjectSourceRoot > &root )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Root >( loader, this ) ), p_Tree_ContextGroup( loader ), root( root )
        {
        }
        bool Root::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::Context >,
                                                  data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                                                  data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >,
                                                  data::Ptr< data::Tree::Link >, data::Ptr< data::Tree::Table > >{
                                        data::Ptr< data::Tree::Root >( loader, const_cast< Root * >( this ) ) };
        }
        void Root::set_inheritance_pointer() { p_Tree_ContextGroup->m_inheritance = data::Ptr< data::Tree::Root >( p_Tree_ContextGroup, this ); }
        void Root::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_ContextGroup );
            loader.load( root );
        }
        void Root::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_ContextGroup );
            storer.store( root );
        }
        void Root::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Root" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "root", root } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Context : public mega::io::Object
        Context::Context( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Context >( loader, this ) ), p_Tree_ContextGroup( loader ),
              p_PerSourceSymbols_Context( loader ), parent( loader )
        {
        }
        Context::Context( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &identifier,
                          const data::Ptr< data::Tree::ContextGroup > &parent )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Context >( loader, this ) ), p_Tree_ContextGroup( loader ),
              p_PerSourceSymbols_Context( loader ), identifier( identifier ), parent( parent )
        {
        }
        bool Context::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::Context >,
                                                  data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                                                  data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >,
                                                  data::Ptr< data::Tree::Link >, data::Ptr< data::Tree::Table > >{
                                        data::Ptr< data::Tree::Context >( loader, const_cast< Context * >( this ) ) };
        }
        void Context::set_inheritance_pointer() { p_Tree_ContextGroup->m_inheritance = data::Ptr< data::Tree::Context >( p_Tree_ContextGroup, this ); }
        void Context::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_ContextGroup );
            loader.load( identifier );
            loader.load( parent );
        }
        void Context::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_ContextGroup );
            storer.store( identifier );
            storer.store( parent );
        }
        void Context::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Context" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "identifier", identifier } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "parent", parent } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Namespace : public mega::io::Object
        Namespace::Namespace( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Namespace >( loader, this ) ), p_Tree_Context( loader )
        {
        }
        Namespace::Namespace( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const bool &is_global,
                              const std::vector< data::Ptr< data::AST::ContextDef > > &namespace_defs )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Namespace >( loader, this ) ), p_Tree_Context( loader ),
              is_global( is_global ), namespace_defs( namespace_defs )
        {
        }
        bool Namespace::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::Context >,
                                                  data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                                                  data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >,
                                                  data::Ptr< data::Tree::Link >, data::Ptr< data::Tree::Table > >{
                                        data::Ptr< data::Tree::Namespace >( loader, const_cast< Namespace * >( this ) ) };
        }
        void Namespace::set_inheritance_pointer() { p_Tree_Context->m_inheritance = data::Ptr< data::Tree::Namespace >( p_Tree_Context, this ); }
        void Namespace::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_Context );
            loader.load( is_global );
            loader.load( namespace_defs );
            loader.load( dimension_traits );
        }
        void Namespace::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_Context );
            storer.store( is_global );
            storer.store( namespace_defs );
            VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Namespace.dimension_traits has NOT been set" );
            storer.store( dimension_traits );
        }
        void Namespace::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Namespace" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "is_global", is_global } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "namespace_defs", namespace_defs } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimension_traits", dimension_traits.value() } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Abstract : public mega::io::Object
        Abstract::Abstract( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Abstract >( loader, this ) ), p_Tree_Context( loader )
        {
        }
        Abstract::Abstract( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                            const std::vector< data::Ptr< data::AST::AbstractDef > > &abstract_defs )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Abstract >( loader, this ) ), p_Tree_Context( loader ),
              abstract_defs( abstract_defs )
        {
        }
        bool Abstract::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::Context >,
                                                  data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                                                  data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >,
                                                  data::Ptr< data::Tree::Link >, data::Ptr< data::Tree::Table > >{
                                        data::Ptr< data::Tree::Abstract >( loader, const_cast< Abstract * >( this ) ) };
        }
        void Abstract::set_inheritance_pointer() { p_Tree_Context->m_inheritance = data::Ptr< data::Tree::Abstract >( p_Tree_Context, this ); }
        void Abstract::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_Context );
            loader.load( abstract_defs );
            loader.load( dimension_traits );
            loader.load( inheritance_trait );
        }
        void Abstract::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_Context );
            storer.store( abstract_defs );
            VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Abstract.dimension_traits has NOT been set" );
            storer.store( dimension_traits );
            VERIFY_RTE_MSG( inheritance_trait.has_value(), "Tree::Abstract.inheritance_trait has NOT been set" );
            storer.store( inheritance_trait );
        }
        void Abstract::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Abstract" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "abstract_defs", abstract_defs } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimension_traits", dimension_traits.value() } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "inheritance_trait", inheritance_trait.value() } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Action : public mega::io::Object
        Action::Action( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Action >( loader, this ) ), p_Tree_Context( loader )
        {
        }
        Action::Action( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::AST::ActionDef > > &action_defs )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Action >( loader, this ) ), p_Tree_Context( loader ),
              action_defs( action_defs )
        {
        }
        bool Action::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::Context >,
                                                  data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                                                  data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >,
                                                  data::Ptr< data::Tree::Link >, data::Ptr< data::Tree::Table > >{
                                        data::Ptr< data::Tree::Action >( loader, const_cast< Action * >( this ) ) };
        }
        void Action::set_inheritance_pointer() { p_Tree_Context->m_inheritance = data::Ptr< data::Tree::Action >( p_Tree_Context, this ); }
        void Action::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_Context );
            loader.load( action_defs );
            loader.load( dimension_traits );
            loader.load( inheritance_trait );
            loader.load( size_trait );
        }
        void Action::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_Context );
            storer.store( action_defs );
            VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Action.dimension_traits has NOT been set" );
            storer.store( dimension_traits );
            VERIFY_RTE_MSG( inheritance_trait.has_value(), "Tree::Action.inheritance_trait has NOT been set" );
            storer.store( inheritance_trait );
            VERIFY_RTE_MSG( size_trait.has_value(), "Tree::Action.size_trait has NOT been set" );
            storer.store( size_trait );
        }
        void Action::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Action" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "action_defs", action_defs } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimension_traits", dimension_traits.value() } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "inheritance_trait", inheritance_trait.value() } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "size_trait", size_trait.value() } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Event : public mega::io::Object
        Event::Event( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Event >( loader, this ) ), p_Tree_Context( loader )
        {
        }
        Event::Event( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::AST::EventDef > > &event_defs )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Event >( loader, this ) ), p_Tree_Context( loader ),
              event_defs( event_defs )
        {
        }
        bool Event::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::Context >,
                                                  data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                                                  data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >,
                                                  data::Ptr< data::Tree::Link >, data::Ptr< data::Tree::Table > >{
                                        data::Ptr< data::Tree::Event >( loader, const_cast< Event * >( this ) ) };
        }
        void Event::set_inheritance_pointer() { p_Tree_Context->m_inheritance = data::Ptr< data::Tree::Event >( p_Tree_Context, this ); }
        void Event::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_Context );
            loader.load( event_defs );
            loader.load( dimension_traits );
            loader.load( inheritance_trait );
            loader.load( size_trait );
        }
        void Event::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_Context );
            storer.store( event_defs );
            VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Event.dimension_traits has NOT been set" );
            storer.store( dimension_traits );
            VERIFY_RTE_MSG( inheritance_trait.has_value(), "Tree::Event.inheritance_trait has NOT been set" );
            storer.store( inheritance_trait );
            VERIFY_RTE_MSG( size_trait.has_value(), "Tree::Event.size_trait has NOT been set" );
            storer.store( size_trait );
        }
        void Event::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Event" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "event_defs", event_defs } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimension_traits", dimension_traits.value() } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "inheritance_trait", inheritance_trait.value() } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "size_trait", size_trait.value() } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Function : public mega::io::Object
        Function::Function( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Function >( loader, this ) ), p_Tree_Context( loader ),
              return_type_trait( loader ), arguments_trait( loader )
        {
        }
        Function::Function( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                            const std::vector< data::Ptr< data::AST::FunctionDef > > &function_defs )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Function >( loader, this ) ), p_Tree_Context( loader ),
              function_defs( function_defs )
        {
        }
        bool Function::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::Context >,
                                                  data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                                                  data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >,
                                                  data::Ptr< data::Tree::Link >, data::Ptr< data::Tree::Table > >{
                                        data::Ptr< data::Tree::Function >( loader, const_cast< Function * >( this ) ) };
        }
        void Function::set_inheritance_pointer() { p_Tree_Context->m_inheritance = data::Ptr< data::Tree::Function >( p_Tree_Context, this ); }
        void Function::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_Context );
            loader.load( function_defs );
            loader.load( return_type_trait );
            loader.load( arguments_trait );
        }
        void Function::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_Context );
            storer.store( function_defs );
            VERIFY_RTE_MSG( return_type_trait.has_value(), "Tree::Function.return_type_trait has NOT been set" );
            storer.store( return_type_trait );
            VERIFY_RTE_MSG( arguments_trait.has_value(), "Tree::Function.arguments_trait has NOT been set" );
            storer.store( arguments_trait );
        }
        void Function::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Function" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "function_defs", function_defs } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "return_type_trait", return_type_trait.value() } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "arguments_trait", arguments_trait.value() } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Object : public mega::io::Object
        Object::Object( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Object >( loader, this ) ), p_Tree_Context( loader )
        {
        }
        Object::Object( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::AST::ObjectDef > > &object_defs )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Object >( loader, this ) ), p_Tree_Context( loader ),
              object_defs( object_defs )
        {
        }
        bool Object::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::Context >,
                                                  data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                                                  data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >,
                                                  data::Ptr< data::Tree::Link >, data::Ptr< data::Tree::Table > >{
                                        data::Ptr< data::Tree::Object >( loader, const_cast< Object * >( this ) ) };
        }
        void Object::set_inheritance_pointer() { p_Tree_Context->m_inheritance = data::Ptr< data::Tree::Object >( p_Tree_Context, this ); }
        void Object::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_Context );
            loader.load( object_defs );
            loader.load( dimension_traits );
            loader.load( inheritance_trait );
        }
        void Object::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_Context );
            storer.store( object_defs );
            VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Object.dimension_traits has NOT been set" );
            storer.store( dimension_traits );
            VERIFY_RTE_MSG( inheritance_trait.has_value(), "Tree::Object.inheritance_trait has NOT been set" );
            storer.store( inheritance_trait );
        }
        void Object::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Object" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "object_defs", object_defs } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimension_traits", dimension_traits.value() } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "inheritance_trait", inheritance_trait.value() } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Link : public mega::io::Object
        Link::Link( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Link >( loader, this ) ), p_Tree_Context( loader )
        {
        }
        Link::Link( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::AST::LinkDef > > &link_defs )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Link >( loader, this ) ), p_Tree_Context( loader ), link_defs( link_defs )
        {
        }
        bool Link::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::Context >,
                                                  data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                                                  data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >,
                                                  data::Ptr< data::Tree::Link >, data::Ptr< data::Tree::Table > >{
                                        data::Ptr< data::Tree::Link >( loader, const_cast< Link * >( this ) ) };
        }
        void Link::set_inheritance_pointer() { p_Tree_Context->m_inheritance = data::Ptr< data::Tree::Link >( p_Tree_Context, this ); }
        void Link::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_Context );
            loader.load( link_defs );
        }
        void Link::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_Context );
            storer.store( link_defs );
        }
        void Link::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Link" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "link_defs", link_defs } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Table : public mega::io::Object
        Table::Table( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Table >( loader, this ) ), p_Tree_Context( loader )
        {
        }
        Table::Table( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::AST::TableDef > > &table_defs )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Table >( loader, this ) ), p_Tree_Context( loader ),
              table_defs( table_defs )
        {
        }
        bool Table::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Tree::ContextGroup >, data::Ptr< data::Tree::Root >, data::Ptr< data::Tree::Context >,
                                                  data::Ptr< data::Tree::Namespace >, data::Ptr< data::Tree::Abstract >, data::Ptr< data::Tree::Action >,
                                                  data::Ptr< data::Tree::Event >, data::Ptr< data::Tree::Function >, data::Ptr< data::Tree::Object >,
                                                  data::Ptr< data::Tree::Link >, data::Ptr< data::Tree::Table > >{
                                        data::Ptr< data::Tree::Table >( loader, const_cast< Table * >( this ) ) };
        }
        void Table::set_inheritance_pointer() { p_Tree_Context->m_inheritance = data::Ptr< data::Tree::Table >( p_Tree_Context, this ); }
        void Table::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_Context );
            loader.load( table_defs );
        }
        void Table::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_Context );
            storer.store( table_defs );
        }
        void Table::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Table" },
                                             { "filetype", "Tree" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "table_defs", table_defs } } );
                part[ "properties" ].push_back( property );
            }
        }

    } // namespace Tree
    namespace DPGraph
    {
        // struct Glob : public mega::io::Object
        Glob::Glob( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Glob >( loader, this ) )
        {
        }
        Glob::Glob( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const boost::filesystem::path &location, const std::string &glob )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Glob >( loader, this ) ), location( location ), glob( glob )
        {
        }
        bool Glob::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::DPGraph::Glob > >{ data::Ptr< data::DPGraph::Glob >( loader, const_cast< Glob * >( this ) ) };
        }
        void Glob::set_inheritance_pointer() {}
        void Glob::load( mega::io::Loader &loader )
        {
            loader.load( location );
            loader.load( glob );
        }
        void Glob::store( mega::io::Storer &storer ) const
        {
            storer.store( location );
            storer.store( glob );
        }
        void Glob::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Glob" },
                                             { "filetype", "DPGraph" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "location", location } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "glob", glob } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct ObjectDependencies : public mega::io::Object
        ObjectDependencies::ObjectDependencies( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::ObjectDependencies >( loader, this ) )
        {
        }
        ObjectDependencies::ObjectDependencies( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const mega::io::megaFilePath &source_file,
                                                const std::size_t &hash_code, const std::vector< data::Ptr< data::DPGraph::Glob > > &globs,
                                                const std::vector< boost::filesystem::path > &resolution )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::ObjectDependencies >( loader, this ) ), source_file( source_file ),
              hash_code( hash_code ), globs( globs ), resolution( resolution )
        {
        }
        bool ObjectDependencies::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::DPGraph::ObjectDependencies > >{
                                        data::Ptr< data::DPGraph::ObjectDependencies >( loader, const_cast< ObjectDependencies * >( this ) ) };
        }
        void ObjectDependencies::set_inheritance_pointer() {}
        void ObjectDependencies::load( mega::io::Loader &loader )
        {
            loader.load( source_file );
            loader.load( hash_code );
            loader.load( globs );
            loader.load( resolution );
        }
        void ObjectDependencies::store( mega::io::Storer &storer ) const
        {
            storer.store( source_file );
            storer.store( hash_code );
            storer.store( globs );
            storer.store( resolution );
        }
        void ObjectDependencies::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "ObjectDependencies" },
                                             { "filetype", "DPGraph" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "source_file", source_file } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "hash_code", hash_code } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "globs", globs } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "resolution", resolution } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Analysis : public mega::io::Object
        Analysis::Analysis( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Analysis >( loader, this ) )
        {
        }
        Analysis::Analysis( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                            const std::vector< data::Ptr< data::DPGraph::ObjectDependencies > > &objects )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Analysis >( loader, this ) ), objects( objects )
        {
        }
        bool Analysis::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::DPGraph::Analysis > >{ data::Ptr< data::DPGraph::Analysis >( loader, const_cast< Analysis * >( this ) ) };
        }
        void Analysis::set_inheritance_pointer() {}
        void Analysis::load( mega::io::Loader &loader ) { loader.load( objects ); }
        void Analysis::store( mega::io::Storer &storer ) const { storer.store( objects ); }
        void Analysis::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Analysis" },
                                             { "filetype", "DPGraph" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "objects", objects } } );
                part[ "properties" ].push_back( property );
            }
        }

    } // namespace DPGraph
    namespace SymbolTable
    {
        // struct Symbol : public mega::io::Object
        Symbol::Symbol( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbol >( loader, this ) )
        {
        }
        Symbol::Symbol( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::string &symbol, const std::int32_t &id,
                        const std::vector< data::Ptr< data::Tree::Context > >        &contexts,
                        const std::vector< data::Ptr< data::Tree::DimensionTrait > > &dimensions )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbol >( loader, this ) ), symbol( symbol ), id( id ),
              contexts( contexts ), dimensions( dimensions )
        {
        }
        bool Symbol::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::SymbolTable::Symbol > >{ data::Ptr< data::SymbolTable::Symbol >( loader, const_cast< Symbol * >( this ) ) };
        }
        void Symbol::set_inheritance_pointer() {}
        void Symbol::load( mega::io::Loader &loader )
        {
            loader.load( symbol );
            loader.load( id );
            loader.load( contexts );
            loader.load( dimensions );
        }
        void Symbol::store( mega::io::Storer &storer ) const
        {
            storer.store( symbol );
            storer.store( id );
            storer.store( contexts );
            storer.store( dimensions );
        }
        void Symbol::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Symbol" },
                                             { "filetype", "SymbolTable" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "symbol", symbol } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "id", id } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "contexts", contexts } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimensions", dimensions } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct SymbolSet : public mega::io::Object
        SymbolSet::SymbolSet( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::SymbolSet >( loader, this ) )
        {
        }
        SymbolSet::SymbolSet( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                              const std::map< std::string, data::Ptr< data::SymbolTable::Symbol > > &symbols, const mega::io::megaFilePath &source_file,
                              const std::size_t                                                                                 &hash_code,
                              const std::map< data::Ptr< data::Tree::Context >, data::Ptr< data::SymbolTable::Symbol > >        &context_symbols,
                              const std::map< data::Ptr< data::Tree::DimensionTrait >, data::Ptr< data::SymbolTable::Symbol > > &dimension_symbols,
                              const std::map< data::Ptr< data::Tree::Context >, int32_t >                                       &context_type_ids,
                              const std::map< data::Ptr< data::Tree::DimensionTrait >, int32_t >                                &dimension_type_ids )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::SymbolSet >( loader, this ) ), symbols( symbols ),
              source_file( source_file ), hash_code( hash_code ), context_symbols( context_symbols ), dimension_symbols( dimension_symbols ),
              context_type_ids( context_type_ids ), dimension_type_ids( dimension_type_ids )
        {
        }
        bool SymbolSet::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::SymbolTable::SymbolSet > >{
                                        data::Ptr< data::SymbolTable::SymbolSet >( loader, const_cast< SymbolSet * >( this ) ) };
        }
        void SymbolSet::set_inheritance_pointer() {}
        void SymbolSet::load( mega::io::Loader &loader )
        {
            loader.load( symbols );
            loader.load( source_file );
            loader.load( hash_code );
            loader.load( context_symbols );
            loader.load( dimension_symbols );
            loader.load( context_type_ids );
            loader.load( dimension_type_ids );
        }
        void SymbolSet::store( mega::io::Storer &storer ) const
        {
            storer.store( symbols );
            storer.store( source_file );
            storer.store( hash_code );
            storer.store( context_symbols );
            storer.store( dimension_symbols );
            storer.store( context_type_ids );
            storer.store( dimension_type_ids );
        }
        void SymbolSet::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "SymbolSet" },
                                             { "filetype", "SymbolTable" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "symbols", symbols } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "source_file", source_file } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "hash_code", hash_code } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "context_symbols", context_symbols } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimension_symbols", dimension_symbols } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "context_type_ids", context_type_ids } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimension_type_ids", dimension_type_ids } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct SymbolTable : public mega::io::Object
        SymbolTable::SymbolTable( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::SymbolTable >( loader, this ) )
        {
        }
        SymbolTable::SymbolTable( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                                  const std::map< mega::io::megaFilePath, data::Ptr< data::SymbolTable::SymbolSet > > &symbol_sets,
                                  const std::map< std::string, data::Ptr< data::SymbolTable::Symbol > >               &symbols,
                                  const std::map< int32_t, data::Ptr< data::Tree::Context > >                         &context_type_ids,
                                  const std::map< int32_t, data::Ptr< data::Tree::DimensionTrait > >                  &dimension_type_ids )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::SymbolTable >( loader, this ) ), symbol_sets( symbol_sets ),
              symbols( symbols ), context_type_ids( context_type_ids ), dimension_type_ids( dimension_type_ids )
        {
        }
        bool SymbolTable::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::SymbolTable::SymbolTable > >{
                                        data::Ptr< data::SymbolTable::SymbolTable >( loader, const_cast< SymbolTable * >( this ) ) };
        }
        void SymbolTable::set_inheritance_pointer() {}
        void SymbolTable::load( mega::io::Loader &loader )
        {
            loader.load( symbol_sets );
            loader.load( symbols );
            loader.load( context_type_ids );
            loader.load( dimension_type_ids );
        }
        void SymbolTable::store( mega::io::Storer &storer ) const
        {
            storer.store( symbol_sets );
            storer.store( symbols );
            storer.store( context_type_ids );
            storer.store( dimension_type_ids );
        }
        void SymbolTable::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "SymbolTable" },
                                             { "filetype", "SymbolTable" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "symbol_sets", symbol_sets } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "symbols", symbols } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "context_type_ids", context_type_ids } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimension_type_ids", dimension_type_ids } } );
                part[ "properties" ].push_back( property );
            }
        }

    } // namespace SymbolTable
    namespace PerSourceSymbols
    {
        // struct DimensionTrait : public mega::io::Object
        DimensionTrait::DimensionTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), p_Tree_DimensionTrait( loader )
        {
        }
        DimensionTrait::DimensionTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, Ptr< Tree::DimensionTrait > p_Tree_DimensionTrait,
                                        const std::int32_t &symbol )
            : mega::io::Object( objectInfo ), p_Tree_DimensionTrait( p_Tree_DimensionTrait ), symbol( symbol )
        {
        }
        bool DimensionTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const { return false; }
        void DimensionTrait::set_inheritance_pointer()
        {
            p_Tree_DimensionTrait->p_PerSourceSymbols_DimensionTrait = data::Ptr< data::PerSourceSymbols::DimensionTrait >( p_Tree_DimensionTrait, this );
        }
        void DimensionTrait::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_DimensionTrait );
            loader.load( symbol );
            loader.load( type_id );
        }
        void DimensionTrait::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_DimensionTrait );
            storer.store( symbol );
            VERIFY_RTE_MSG( type_id.has_value(), "Tree::DimensionTrait.type_id has NOT been set" );
            storer.store( type_id );
        }
        void DimensionTrait::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "DimensionTrait" },
                                             { "filetype", "PerSourceSymbols" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "symbol", symbol } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "type_id", type_id.value() } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Context : public mega::io::Object
        Context::Context( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo ) : mega::io::Object( objectInfo ), p_Tree_Context( loader ) {}
        Context::Context( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, Ptr< Tree::Context > p_Tree_Context, const std::int32_t &symbol )
            : mega::io::Object( objectInfo ), p_Tree_Context( p_Tree_Context ), symbol( symbol )
        {
        }
        bool Context::test_inheritance_pointer( ObjectPartLoader &loader ) const { return false; }
        void Context::set_inheritance_pointer()
        {
            p_Tree_Context->p_PerSourceSymbols_Context = data::Ptr< data::PerSourceSymbols::Context >( p_Tree_Context, this );
        }
        void Context::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_Context );
            loader.load( symbol );
            loader.load( type_id );
        }
        void Context::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_Context );
            storer.store( symbol );
            VERIFY_RTE_MSG( type_id.has_value(), "Tree::Context.type_id has NOT been set" );
            storer.store( type_id );
        }
        void Context::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Context" },
                                             { "filetype", "PerSourceSymbols" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "symbol", symbol } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "type_id", type_id.value() } } );
                part[ "properties" ].push_back( property );
            }
        }

    } // namespace PerSourceSymbols
    namespace Clang
    {
        // struct InheritanceTrait : public mega::io::Object
        InheritanceTrait::InheritanceTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), p_Tree_InheritanceTrait( loader )
        {
        }
        InheritanceTrait::InheritanceTrait( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                                            Ptr< Tree::InheritanceTrait >                          p_Tree_InheritanceTrait,
                                            const std::vector< data::Ptr< data::Tree::Context > > &contexts )
            : mega::io::Object( objectInfo ), p_Tree_InheritanceTrait( p_Tree_InheritanceTrait ), contexts( contexts )
        {
        }
        bool InheritanceTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const { return false; }
        void InheritanceTrait::set_inheritance_pointer()
        {
            p_Tree_InheritanceTrait->p_Clang_InheritanceTrait = data::Ptr< data::Clang::InheritanceTrait >( p_Tree_InheritanceTrait, this );
        }
        void InheritanceTrait::load( mega::io::Loader &loader )
        {
            loader.load( p_Tree_InheritanceTrait );
            loader.load( contexts );
        }
        void InheritanceTrait::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Tree_InheritanceTrait );
            storer.store( contexts );
        }
        void InheritanceTrait::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "InheritanceTrait" },
                                             { "filetype", "Clang" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "contexts", contexts } } );
                part[ "properties" ].push_back( property );
            }
        }

    } // namespace Clang
    namespace Concrete
    {
        // struct Dimension : public mega::io::Object
        Dimension::Dimension( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Dimension >( loader, this ) ), interface_dimension( loader )
        {
        }
        Dimension::Dimension( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo,
                              const data::Ptr< data::Tree::DimensionTrait > &interface_dimension )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Dimension >( loader, this ) ),
              interface_dimension( interface_dimension )
        {
        }
        bool Dimension::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance == std::variant< data::Ptr< data::Concrete::Dimension > >{
                                        data::Ptr< data::Concrete::Dimension >( loader, const_cast< Dimension * >( this ) ) };
        }
        void Dimension::set_inheritance_pointer() {}
        void Dimension::load( mega::io::Loader &loader ) { loader.load( interface_dimension ); }
        void Dimension::store( mega::io::Storer &storer ) const { storer.store( interface_dimension ); }
        void Dimension::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Dimension" },
                                             { "filetype", "Concrete" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "interface_dimension", interface_dimension } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Context : public mega::io::Object
        Context::Context( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Context >( loader, this ) )
        {
        }
        Context::Context( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const std::vector< data::Ptr< data::Tree::Context > > &inheritance,
                          const std::vector< data::Ptr< data::Concrete::Context > > &children )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Context >( loader, this ) ), inheritance( inheritance ),
              children( children )
        {
        }
        bool Context::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Action >, data::Ptr< data::Concrete::Event >,
                                 data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >, data::Ptr< data::Concrete::Link > >{
                       data::Ptr< data::Concrete::Context >( loader, const_cast< Context * >( this ) ) };
        }
        void Context::set_inheritance_pointer() {}
        void Context::load( mega::io::Loader &loader )
        {
            loader.load( inheritance );
            loader.load( children );
        }
        void Context::store( mega::io::Storer &storer ) const
        {
            storer.store( inheritance );
            storer.store( children );
        }
        void Context::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Context" },
                                             { "filetype", "Concrete" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "inheritance", inheritance } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "children", children } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Action : public mega::io::Object
        Action::Action( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Action >( loader, this ) ), p_Concrete_Context( loader ),
              interface_action( loader )
        {
        }
        Action::Action( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Action > &interface_action,
                        const std::vector< data::Ptr< data::Concrete::Dimension > > &dimensions )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Action >( loader, this ) ), p_Concrete_Context( loader ),
              interface_action( interface_action ), dimensions( dimensions )
        {
        }
        bool Action::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Action >, data::Ptr< data::Concrete::Event >,
                                 data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >, data::Ptr< data::Concrete::Link > >{
                       data::Ptr< data::Concrete::Action >( loader, const_cast< Action * >( this ) ) };
        }
        void Action::set_inheritance_pointer() { p_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Action >( p_Concrete_Context, this ); }
        void Action::load( mega::io::Loader &loader )
        {
            loader.load( p_Concrete_Context );
            loader.load( interface_action );
            loader.load( dimensions );
        }
        void Action::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Concrete_Context );
            storer.store( interface_action );
            storer.store( dimensions );
        }
        void Action::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Action" },
                                             { "filetype", "Concrete" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "interface_action", interface_action } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimensions", dimensions } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Event : public mega::io::Object
        Event::Event( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Event >( loader, this ) ), p_Concrete_Context( loader ),
              interface_event( loader )
        {
        }
        Event::Event( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Event > &interface_event,
                      const std::vector< data::Ptr< data::Concrete::Dimension > > &dimensions )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Event >( loader, this ) ), p_Concrete_Context( loader ),
              interface_event( interface_event ), dimensions( dimensions )
        {
        }
        bool Event::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Action >, data::Ptr< data::Concrete::Event >,
                                 data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >, data::Ptr< data::Concrete::Link > >{
                       data::Ptr< data::Concrete::Event >( loader, const_cast< Event * >( this ) ) };
        }
        void Event::set_inheritance_pointer() { p_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Event >( p_Concrete_Context, this ); }
        void Event::load( mega::io::Loader &loader )
        {
            loader.load( p_Concrete_Context );
            loader.load( interface_event );
            loader.load( dimensions );
        }
        void Event::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Concrete_Context );
            storer.store( interface_event );
            storer.store( dimensions );
        }
        void Event::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Event" },
                                             { "filetype", "Concrete" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "interface_event", interface_event } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimensions", dimensions } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Function : public mega::io::Object
        Function::Function( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Function >( loader, this ) ), p_Concrete_Context( loader ),
              interface_function( loader )
        {
        }
        Function::Function( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Function > &interface_function )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Function >( loader, this ) ), p_Concrete_Context( loader ),
              interface_function( interface_function )
        {
        }
        bool Function::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Action >, data::Ptr< data::Concrete::Event >,
                                 data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >, data::Ptr< data::Concrete::Link > >{
                       data::Ptr< data::Concrete::Function >( loader, const_cast< Function * >( this ) ) };
        }
        void Function::set_inheritance_pointer() { p_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Function >( p_Concrete_Context, this ); }
        void Function::load( mega::io::Loader &loader )
        {
            loader.load( p_Concrete_Context );
            loader.load( interface_function );
        }
        void Function::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Concrete_Context );
            storer.store( interface_function );
        }
        void Function::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Function" },
                                             { "filetype", "Concrete" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "interface_function", interface_function } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Object : public mega::io::Object
        Object::Object( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Object >( loader, this ) ), p_Concrete_Context( loader ),
              interface_object( loader )
        {
        }
        Object::Object( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Object > &interface_object,
                        const std::vector< data::Ptr< data::Concrete::Dimension > > &dimensions )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Object >( loader, this ) ), p_Concrete_Context( loader ),
              interface_object( interface_object ), dimensions( dimensions )
        {
        }
        bool Object::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Action >, data::Ptr< data::Concrete::Event >,
                                 data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >, data::Ptr< data::Concrete::Link > >{
                       data::Ptr< data::Concrete::Object >( loader, const_cast< Object * >( this ) ) };
        }
        void Object::set_inheritance_pointer() { p_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Object >( p_Concrete_Context, this ); }
        void Object::load( mega::io::Loader &loader )
        {
            loader.load( p_Concrete_Context );
            loader.load( interface_object );
            loader.load( dimensions );
        }
        void Object::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Concrete_Context );
            storer.store( interface_object );
            storer.store( dimensions );
        }
        void Object::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Object" },
                                             { "filetype", "Concrete" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "interface_object", interface_object } } );
                part[ "properties" ].push_back( property );
            }
            {
                nlohmann::json property = nlohmann::json::object( { { "dimensions", dimensions } } );
                part[ "properties" ].push_back( property );
            }
        }

        // struct Link : public mega::io::Object
        Link::Link( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Link >( loader, this ) ), p_Concrete_Context( loader ),
              interface_link( loader )
        {
        }
        Link::Link( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo, const data::Ptr< data::Tree::Link > &interface_link )
            : mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Link >( loader, this ) ), p_Concrete_Context( loader ),
              interface_link( interface_link )
        {
        }
        bool Link::test_inheritance_pointer( ObjectPartLoader &loader ) const
        {
            return m_inheritance ==
                   std::variant< data::Ptr< data::Concrete::Context >, data::Ptr< data::Concrete::Action >, data::Ptr< data::Concrete::Event >,
                                 data::Ptr< data::Concrete::Function >, data::Ptr< data::Concrete::Object >, data::Ptr< data::Concrete::Link > >{
                       data::Ptr< data::Concrete::Link >( loader, const_cast< Link * >( this ) ) };
        }
        void Link::set_inheritance_pointer() { p_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Link >( p_Concrete_Context, this ); }
        void Link::load( mega::io::Loader &loader )
        {
            loader.load( p_Concrete_Context );
            loader.load( interface_link );
        }
        void Link::store( mega::io::Storer &storer ) const
        {
            storer.store( p_Concrete_Context );
            storer.store( interface_link );
        }
        void Link::to_json( nlohmann::json &part ) const
        {
            part = nlohmann::json::object( { { "partname", "Link" },
                                             { "filetype", "Concrete" },
                                             { "typeID", Object_Part_Type_ID },
                                             { "fileID", getFileID() },
                                             { "index", getIndex() },
                                             { "properties", nlohmann::json::array() } } );
            {
                nlohmann::json property = nlohmann::json::object( { { "interface_link", interface_link } } );
                part[ "properties" ].push_back( property );
            }
        }

    } // namespace Concrete

    mega::io::Object *Factory::create( ObjectPartLoader &loader, const mega::io::ObjectInfo &objectInfo )
    {
        switch( objectInfo.getType() )
        {
        case 0:
            return new Components::Component( loader, objectInfo );
        case 1:
            return new AST::Identifier( loader, objectInfo );
        case 2:
            return new AST::ScopedIdentifier( loader, objectInfo );
        case 3:
            return new AST::ArgumentList( loader, objectInfo );
        case 4:
            return new AST::ReturnType( loader, objectInfo );
        case 5:
            return new AST::Inheritance( loader, objectInfo );
        case 6:
            return new AST::Size( loader, objectInfo );
        case 7:
            return new AST::Dimension( loader, objectInfo );
        case 8:
            return new AST::Include( loader, objectInfo );
        case 9:
            return new AST::SystemInclude( loader, objectInfo );
        case 10:
            return new AST::MegaInclude( loader, objectInfo );
        case 11:
            return new AST::MegaIncludeInline( loader, objectInfo );
        case 12:
            return new AST::MegaIncludeNested( loader, objectInfo );
        case 13:
            return new AST::CPPInclude( loader, objectInfo );
        case 14:
            return new AST::Dependency( loader, objectInfo );
        case 15:
            return new AST::ContextDef( loader, objectInfo );
        case 17:
            return new AST::NamespaceDef( loader, objectInfo );
        case 18:
            return new AST::AbstractDef( loader, objectInfo );
        case 19:
            return new AST::ActionDef( loader, objectInfo );
        case 20:
            return new AST::EventDef( loader, objectInfo );
        case 21:
            return new AST::FunctionDef( loader, objectInfo );
        case 22:
            return new AST::ObjectDef( loader, objectInfo );
        case 23:
            return new AST::LinkDef( loader, objectInfo );
        case 24:
            return new AST::TableDef( loader, objectInfo );
        case 25:
            return new AST::SourceRoot( loader, objectInfo );
        case 26:
            return new AST::IncludeRoot( loader, objectInfo );
        case 27:
            return new AST::ObjectSourceRoot( loader, objectInfo );
        case 16:
            return new Body::ContextDef( loader, objectInfo );
        case 28:
            return new Tree::DimensionTrait( loader, objectInfo );
        case 30:
            return new Tree::InheritanceTrait( loader, objectInfo );
        case 32:
            return new Tree::ReturnTypeTrait( loader, objectInfo );
        case 33:
            return new Tree::ArgumentListTrait( loader, objectInfo );
        case 34:
            return new Tree::SizeTrait( loader, objectInfo );
        case 35:
            return new Tree::ContextGroup( loader, objectInfo );
        case 36:
            return new Tree::Root( loader, objectInfo );
        case 37:
            return new Tree::Context( loader, objectInfo );
        case 39:
            return new Tree::Namespace( loader, objectInfo );
        case 40:
            return new Tree::Abstract( loader, objectInfo );
        case 41:
            return new Tree::Action( loader, objectInfo );
        case 42:
            return new Tree::Event( loader, objectInfo );
        case 43:
            return new Tree::Function( loader, objectInfo );
        case 44:
            return new Tree::Object( loader, objectInfo );
        case 45:
            return new Tree::Link( loader, objectInfo );
        case 46:
            return new Tree::Table( loader, objectInfo );
        case 54:
            return new DPGraph::Glob( loader, objectInfo );
        case 55:
            return new DPGraph::ObjectDependencies( loader, objectInfo );
        case 56:
            return new DPGraph::Analysis( loader, objectInfo );
        case 57:
            return new SymbolTable::Symbol( loader, objectInfo );
        case 58:
            return new SymbolTable::SymbolSet( loader, objectInfo );
        case 59:
            return new SymbolTable::SymbolTable( loader, objectInfo );
        case 29:
            return new PerSourceSymbols::DimensionTrait( loader, objectInfo );
        case 38:
            return new PerSourceSymbols::Context( loader, objectInfo );
        case 31:
            return new Clang::InheritanceTrait( loader, objectInfo );
        case 47:
            return new Concrete::Dimension( loader, objectInfo );
        case 48:
            return new Concrete::Context( loader, objectInfo );
        case 49:
            return new Concrete::Action( loader, objectInfo );
        case 50:
            return new Concrete::Event( loader, objectInfo );
        case 51:
            return new Concrete::Function( loader, objectInfo );
        case 52:
            return new Concrete::Object( loader, objectInfo );
        case 53:
            return new Concrete::Link( loader, objectInfo );
        default:
            THROW_RTE( "Unrecognised object type ID" );
        }
    }

} // namespace data
