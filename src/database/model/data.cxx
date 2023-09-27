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

// this made no diff on windows
// #ifdef _WIN32
// #pragma runtime_checks( "", off )
// #pragma optimize( "2", on )
// #define VERIFY_RTE_MSG
// #endif


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
    Components_Component::Components_Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::ComponentType& type, const std::string& name, const mega::io::ComponentFilePath& file_path, const mega::io::ComponentFilePath& python_file_path, const mega::io::ComponentFilePath& init_file_path, const boost::filesystem::path& src_dir, const boost::filesystem::path& build_dir, const std::vector< std::string >& cpp_flags, const std::vector< std::string >& cpp_defines, const std::vector< boost::filesystem::path >& include_directories, const std::vector< mega::io::megaFilePath >& dependencies, const std::vector< mega::io::megaFilePath >& mega_source_files, const std::vector< mega::io::cppFilePath >& cpp_source_files, const std::vector< mega::io::schFilePath >& sch_source_files)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Components::Components_Component >( loader, this ) )          , type( type )
          , name( name )
          , file_path( file_path )
          , python_file_path( python_file_path )
          , init_file_path( init_file_path )
          , src_dir( src_dir )
          , build_dir( build_dir )
          , cpp_flags( cpp_flags )
          , cpp_defines( cpp_defines )
          , include_directories( include_directories )
          , dependencies( dependencies )
          , mega_source_files( mega_source_files )
          , cpp_source_files( cpp_source_files )
          , sch_source_files( sch_source_files )
    {
    }
    bool Components_Component::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Components::Components_Component >( loader, const_cast< Components_Component* >( this ) ) };
    }
    void Components_Component::set_inheritance_pointer()
    {
    }
    void Components_Component::load( mega::io::Loader& loader )
    {
        loader.load( type );
        loader.load( name );
        loader.load( file_path );
        loader.load( python_file_path );
        loader.load( init_file_path );
        loader.load( src_dir );
        loader.load( build_dir );
        loader.load( cpp_flags );
        loader.load( cpp_defines );
        loader.load( include_directories );
        loader.load( dependencies );
        loader.load( mega_source_files );
        loader.load( cpp_source_files );
        loader.load( sch_source_files );
    }
    void Components_Component::store( mega::io::Storer& storer ) const
    {
        storer.store( type );
        storer.store( name );
        storer.store( file_path );
        storer.store( python_file_path );
        storer.store( init_file_path );
        storer.store( src_dir );
        storer.store( build_dir );
        storer.store( cpp_flags );
        storer.store( cpp_defines );
        storer.store( include_directories );
        storer.store( dependencies );
        storer.store( mega_source_files );
        storer.store( cpp_source_files );
        storer.store( sch_source_files );
    }
    void Components_Component::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "name", name } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "file_path", file_path } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "python_file_path", python_file_path } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "init_file_path", init_file_path } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "src_dir", src_dir } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "build_dir", build_dir } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "cpp_flags", cpp_flags } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "cpp_defines", cpp_defines } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "include_directories", include_directories } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dependencies", dependencies } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "mega_source_files", mega_source_files } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "cpp_source_files", cpp_source_files } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "sch_source_files", sch_source_files } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct MegaMangle_Mangle : public mega::io::Object
    MegaMangle_Mangle::MegaMangle_Mangle( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Components::MegaMangle_Mangle >( loader, this ) )    {
    }
    MegaMangle_Mangle::MegaMangle_Mangle( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& canon, const std::string& mangle, const std::string& impl, const std::string& blit)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Components::MegaMangle_Mangle >( loader, this ) )          , canon( canon )
          , mangle( mangle )
          , impl( impl )
          , blit( blit )
    {
    }
    bool MegaMangle_Mangle::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Components::MegaMangle_Mangle >( loader, const_cast< MegaMangle_Mangle* >( this ) ) };
    }
    void MegaMangle_Mangle::set_inheritance_pointer()
    {
    }
    void MegaMangle_Mangle::load( mega::io::Loader& loader )
    {
        loader.load( canon );
        loader.load( mangle );
        loader.load( impl );
        loader.load( blit );
    }
    void MegaMangle_Mangle::store( mega::io::Storer& storer ) const
    {
        storer.store( canon );
        storer.store( mangle );
        storer.store( impl );
        storer.store( blit );
    }
    void MegaMangle_Mangle::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "MegaMangle_Mangle" },
                { "filetype" , "Components" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "canon", canon } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "mangle", mangle } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "impl", impl } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "blit", blit } } );
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_Identifier >( loader, const_cast< Parser_Identifier* >( this ) ) };
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
    void Parser_Identifier::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_ScopedIdentifier : public mega::io::Object
    Parser_ScopedIdentifier::Parser_ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ScopedIdentifier >( loader, this ) )    {
    }
    Parser_ScopedIdentifier::Parser_ScopedIdentifier( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_Identifier > >& ids, const std::string& source_file, const mega::U64& line_number)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ScopedIdentifier >( loader, this ) )          , ids( ids )
          , source_file( source_file )
          , line_number( line_number )
    {
    }
    bool Parser_ScopedIdentifier::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_ScopedIdentifier >( loader, const_cast< Parser_ScopedIdentifier* >( this ) ) };
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
    void Parser_ScopedIdentifier::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_file", source_file } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "line_number", line_number } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_TypeList : public mega::io::Object
    Parser_TypeList::Parser_TypeList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_TypeList >( loader, this ) )    {
    }
    Parser_TypeList::Parser_TypeList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< mega::Type >& args)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_TypeList >( loader, this ) )          , args( args )
    {
    }
    bool Parser_TypeList::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_TypeList >( loader, const_cast< Parser_TypeList* >( this ) ) };
    }
    void Parser_TypeList::set_inheritance_pointer()
    {
    }
    void Parser_TypeList::load( mega::io::Loader& loader )
    {
        loader.load( args );
    }
    void Parser_TypeList::store( mega::io::Storer& storer ) const
    {
        storer.store( args );
    }
    void Parser_TypeList::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Parser_TypeList" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "args", args } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_ArgumentList : public mega::io::Object
    Parser_ArgumentList::Parser_ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ArgumentList >( loader, this ) )    {
    }
    Parser_ArgumentList::Parser_ArgumentList( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< mega::TypeName >& args)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ArgumentList >( loader, this ) )          , args( args )
    {
    }
    bool Parser_ArgumentList::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_ArgumentList >( loader, const_cast< Parser_ArgumentList* >( this ) ) };
    }
    void Parser_ArgumentList::set_inheritance_pointer()
    {
    }
    void Parser_ArgumentList::load( mega::io::Loader& loader )
    {
        loader.load( args );
    }
    void Parser_ArgumentList::store( mega::io::Storer& storer ) const
    {
        storer.store( args );
    }
    void Parser_ArgumentList::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "args", args } } );
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_ReturnType >( loader, const_cast< Parser_ReturnType* >( this ) ) };
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
    void Parser_ReturnType::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Transition : public mega::io::Object
    Parser_Transition::Parser_Transition( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Transition >( loader, this ) )    {
    }
    Parser_Transition::Parser_Transition( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& is_successor, const bool& is_predecessor, const std::string& str)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Transition >( loader, this ) )          , is_successor( is_successor )
          , is_predecessor( is_predecessor )
          , str( str )
    {
    }
    bool Parser_Transition::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_Transition >( loader, const_cast< Parser_Transition* >( this ) ) };
    }
    void Parser_Transition::set_inheritance_pointer()
    {
    }
    void Parser_Transition::load( mega::io::Loader& loader )
    {
        loader.load( is_successor );
        loader.load( is_predecessor );
        loader.load( str );
    }
    void Parser_Transition::store( mega::io::Storer& storer ) const
    {
        storer.store( is_successor );
        storer.store( is_predecessor );
        storer.store( str );
    }
    void Parser_Transition::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Parser_Transition" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "is_successor", is_successor } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "is_predecessor", is_predecessor } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "str", str } } );
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_Inheritance >( loader, const_cast< Parser_Inheritance* >( this ) ) };
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
    void Parser_Inheritance::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_Size >( loader, const_cast< Parser_Size* >( this ) ) };
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
    void Parser_Size::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Initialiser : public mega::io::Object
    Parser_Initialiser::Parser_Initialiser( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Initialiser >( loader, this ) )    {
    }
    Parser_Initialiser::Parser_Initialiser( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& initialiser, const mega::U64& source_loc_start, const mega::U64& source_loc_end)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Initialiser >( loader, this ) )          , initialiser( initialiser )
          , source_loc_start( source_loc_start )
          , source_loc_end( source_loc_end )
    {
    }
    bool Parser_Initialiser::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_Initialiser >( loader, const_cast< Parser_Initialiser* >( this ) ) };
    }
    void Parser_Initialiser::set_inheritance_pointer()
    {
    }
    void Parser_Initialiser::load( mega::io::Loader& loader )
    {
        loader.load( initialiser );
        loader.load( source_loc_start );
        loader.load( source_loc_end );
    }
    void Parser_Initialiser::store( mega::io::Storer& storer ) const
    {
        storer.store( initialiser );
        storer.store( source_loc_start );
        storer.store( source_loc_end );
    }
    void Parser_Initialiser::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Parser_Initialiser" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "initialiser", initialiser } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_loc_start", source_loc_start } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_loc_end", source_loc_end } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Link : public mega::io::Object
    Parser_Link::Parser_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Link >( loader, this ) )          , id( loader )
          , type( loader )
    {
    }
    Parser_Link::Parser_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Identifier >& id, const data::Ptr< data::AST::Parser_TypeList >& type, const bool& owning, const std::optional< mega::CardinalityRange >& cardinality)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Link >( loader, this ) )          , id( id )
          , type( type )
          , owning( owning )
          , cardinality( cardinality )
    {
    }
    bool Parser_Link::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_Link >( loader, const_cast< Parser_Link* >( this ) ) };
    }
    void Parser_Link::set_inheritance_pointer()
    {
    }
    void Parser_Link::load( mega::io::Loader& loader )
    {
        loader.load( id );
        loader.load( type );
        loader.load( owning );
        loader.load( cardinality );
    }
    void Parser_Link::store( mega::io::Storer& storer ) const
    {
        storer.store( id );
        storer.store( type );
        storer.store( owning );
        storer.store( cardinality );
    }
    void Parser_Link::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "id", id } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "type", type } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "owning", owning } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "cardinality", cardinality } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Dimension : public mega::io::Object
    Parser_Dimension::Parser_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Dimension >( loader, this ) )          , id( loader )
    {
    }
    Parser_Dimension::Parser_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const bool& isConst, const data::Ptr< data::AST::Parser_Identifier >& id, const std::string& type, const std::optional< data::Ptr< data::AST::Parser_Initialiser > >& initialiser)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Dimension >( loader, this ) )          , isConst( isConst )
          , id( id )
          , type( type )
          , initialiser( initialiser )
    {
    }
    bool Parser_Dimension::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_Dimension >( loader, const_cast< Parser_Dimension* >( this ) ) };
    }
    void Parser_Dimension::set_inheritance_pointer()
    {
    }
    void Parser_Dimension::load( mega::io::Loader& loader )
    {
        loader.load( isConst );
        loader.load( id );
        loader.load( type );
        loader.load( initialiser );
    }
    void Parser_Dimension::store( mega::io::Storer& storer ) const
    {
        storer.store( isConst );
        storer.store( id );
        storer.store( type );
        storer.store( initialiser );
    }
    void Parser_Dimension::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "type", type } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "initialiser", initialiser } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Requirement : public mega::io::Object
    Parser_Requirement::Parser_Requirement( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Requirement >( loader, this ) )          , id( loader )
          , argumentList( loader )
    {
    }
    Parser_Requirement::Parser_Requirement( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Identifier >& id, const data::Ptr< data::AST::Parser_TypeList >& argumentList)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Requirement >( loader, this ) )          , id( id )
          , argumentList( argumentList )
    {
    }
    bool Parser_Requirement::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_Requirement >( loader, const_cast< Parser_Requirement* >( this ) ) };
    }
    void Parser_Requirement::set_inheritance_pointer()
    {
    }
    void Parser_Requirement::load( mega::io::Loader& loader )
    {
        loader.load( id );
        loader.load( argumentList );
    }
    void Parser_Requirement::store( mega::io::Storer& storer ) const
    {
        storer.store( id );
        storer.store( argumentList );
    }
    void Parser_Requirement::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Parser_Requirement" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "argumentList", argumentList } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Part : public mega::io::Object
    Parser_Part::Parser_Part( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Part >( loader, this ) )    {
    }
    Parser_Part::Parser_Part( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< std::string >& identifiers)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Part >( loader, this ) )          , identifiers( identifiers )
    {
    }
    bool Parser_Part::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_Part >( loader, const_cast< Parser_Part* >( this ) ) };
    }
    void Parser_Part::set_inheritance_pointer()
    {
    }
    void Parser_Part::load( mega::io::Loader& loader )
    {
        loader.load( identifiers );
    }
    void Parser_Part::store( mega::io::Storer& storer ) const
    {
        storer.store( identifiers );
    }
    void Parser_Part::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Parser_Part" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "identifiers", identifiers } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_Include : public mega::io::Object
    Parser_Include::Parser_Include( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Include >( loader, this ) )    {
    }
    bool Parser_Include::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_Include >( loader, const_cast< Parser_Include* >( this ) ) };
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
    void Parser_Include::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_SystemInclude >( loader, const_cast< Parser_SystemInclude* >( this ) ) };
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
    void Parser_SystemInclude::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_MegaInclude >( loader, const_cast< Parser_MegaInclude* >( this ) ) };
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
    void Parser_MegaInclude::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "root", root.value() } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_MegaIncludeInline : public mega::io::Object
    Parser_MegaIncludeInline::Parser_MegaIncludeInline( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_MegaIncludeInline >( loader, this ) )          , p_AST_Parser_MegaInclude( loader )
    {
    }
    bool Parser_MegaIncludeInline::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_MegaIncludeInline >( loader, const_cast< Parser_MegaIncludeInline* >( this ) ) };
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
    void Parser_MegaIncludeInline::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_MegaIncludeNested >( loader, const_cast< Parser_MegaIncludeNested* >( this ) ) };
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
    void Parser_MegaIncludeNested::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_CPPInclude >( loader, const_cast< Parser_CPPInclude* >( this ) ) };
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
    void Parser_CPPInclude::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_Dependency >( loader, const_cast< Parser_Dependency* >( this ) ) };
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
    void Parser_Dependency::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_ContextDef : public mega::io::Object
    Parser_ContextDef::Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ContextDef >( loader, this ) )          , p_Body_Parser_ContextDef( loader )
          , id( loader )
    {
    }
    Parser_ContextDef::Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_ScopedIdentifier >& id, const std::vector< data::Ptr< data::AST::Parser_ContextDef > >& children, const std::vector< data::Ptr< data::AST::Parser_Dimension > >& dimensions, const std::vector< data::Ptr< data::AST::Parser_Link > >& links, const std::vector< data::Ptr< data::AST::Parser_Include > >& includes, const std::vector< data::Ptr< data::AST::Parser_Dependency > >& dependencies, const std::vector< data::Ptr< data::AST::Parser_Requirement > >& requirements, const std::vector< data::Ptr< data::AST::Parser_Part > >& parts)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ContextDef >( loader, this ) )          , p_Body_Parser_ContextDef( loader )
          , id( id )
          , children( children )
          , dimensions( dimensions )
          , links( links )
          , includes( includes )
          , dependencies( dependencies )
          , requirements( requirements )
          , parts( parts )
    {
    }
    bool Parser_ContextDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_ContextDef >( loader, const_cast< Parser_ContextDef* >( this ) ) };
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
        loader.load( links );
        loader.load( includes );
        loader.load( dependencies );
        loader.load( requirements );
        loader.load( parts );
    }
    void Parser_ContextDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Body_Parser_ContextDef );
        storer.store( id );
        storer.store( children );
        storer.store( dimensions );
        storer.store( links );
        storer.store( includes );
        storer.store( dependencies );
        storer.store( requirements );
        storer.store( parts );
    }
    void Parser_ContextDef::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "children", children } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "links", links } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "includes", includes } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dependencies", dependencies } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "requirements", requirements } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "parts", parts } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_NamespaceDef : public mega::io::Object
    Parser_NamespaceDef::Parser_NamespaceDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_NamespaceDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
    {
    }
    bool Parser_NamespaceDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_NamespaceDef >( loader, const_cast< Parser_NamespaceDef* >( this ) ) };
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
    void Parser_NamespaceDef::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
          , size( loader )
          , inheritance( loader )
    {
    }
    Parser_AbstractDef::Parser_AbstractDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Size >& size, const data::Ptr< data::AST::Parser_Inheritance >& inheritance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_AbstractDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , size( size )
          , inheritance( inheritance )
    {
    }
    bool Parser_AbstractDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_AbstractDef >( loader, const_cast< Parser_AbstractDef* >( this ) ) };
    }
    void Parser_AbstractDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_AbstractDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_AbstractDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
        loader.load( size );
        loader.load( inheritance );
    }
    void Parser_AbstractDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
        storer.store( size );
        storer.store( inheritance );
    }
    void Parser_AbstractDef::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "size", size } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_StateDef : public mega::io::Object
    Parser_StateDef::Parser_StateDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_StateDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , size( loader )
          , inheritance( loader )
          , transition( loader )
    {
    }
    Parser_StateDef::Parser_StateDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Size >& size, const data::Ptr< data::AST::Parser_Inheritance >& inheritance, const data::Ptr< data::AST::Parser_Transition >& transition)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_StateDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , size( size )
          , inheritance( inheritance )
          , transition( transition )
    {
    }
    bool Parser_StateDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_StateDef >( loader, const_cast< Parser_StateDef* >( this ) ) };
    }
    void Parser_StateDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_StateDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_StateDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
        loader.load( size );
        loader.load( inheritance );
        loader.load( transition );
    }
    void Parser_StateDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
        storer.store( size );
        storer.store( inheritance );
        storer.store( transition );
    }
    void Parser_StateDef::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Parser_StateDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "size", size } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "transition", transition } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_ActionDef : public mega::io::Object
    Parser_ActionDef::Parser_ActionDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ActionDef >( loader, this ) )          , p_AST_Parser_StateDef( loader )
    {
    }
    bool Parser_ActionDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_ActionDef >( loader, const_cast< Parser_ActionDef* >( this ) ) };
    }
    void Parser_ActionDef::set_inheritance_pointer()
    {
        p_AST_Parser_StateDef->m_inheritance = data::Ptr< data::AST::Parser_ActionDef >( p_AST_Parser_StateDef, this );
    }
    void Parser_ActionDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_StateDef );
    }
    void Parser_ActionDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_StateDef );
    }
    void Parser_ActionDef::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Parser_ActionDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Parser_ComponentDef : public mega::io::Object
    Parser_ComponentDef::Parser_ComponentDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ComponentDef >( loader, this ) )          , p_AST_Parser_StateDef( loader )
    {
    }
    bool Parser_ComponentDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_ComponentDef >( loader, const_cast< Parser_ComponentDef* >( this ) ) };
    }
    void Parser_ComponentDef::set_inheritance_pointer()
    {
        p_AST_Parser_StateDef->m_inheritance = data::Ptr< data::AST::Parser_ComponentDef >( p_AST_Parser_StateDef, this );
    }
    void Parser_ComponentDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_StateDef );
    }
    void Parser_ComponentDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_StateDef );
    }
    void Parser_ComponentDef::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Parser_ComponentDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_EventDef >( loader, const_cast< Parser_EventDef* >( this ) ) };
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
    void Parser_EventDef::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_InteruptDef : public mega::io::Object
    Parser_InteruptDef::Parser_InteruptDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_InteruptDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , argumentList( loader )
          , transition( loader )
    {
    }
    Parser_InteruptDef::Parser_InteruptDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_ArgumentList >& argumentList, const data::Ptr< data::AST::Parser_Transition >& transition)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_InteruptDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , argumentList( argumentList )
          , transition( transition )
    {
    }
    bool Parser_InteruptDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_InteruptDef >( loader, const_cast< Parser_InteruptDef* >( this ) ) };
    }
    void Parser_InteruptDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_InteruptDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_InteruptDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
        loader.load( argumentList );
        loader.load( transition );
    }
    void Parser_InteruptDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
        storer.store( argumentList );
        storer.store( transition );
    }
    void Parser_InteruptDef::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Parser_InteruptDef" },
                { "filetype" , "AST" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "argumentList", argumentList } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "transition", transition } } );
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_FunctionDef >( loader, const_cast< Parser_FunctionDef* >( this ) ) };
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
    void Parser_FunctionDef::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "returnType", returnType } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_ObjectDef : public mega::io::Object
    Parser_ObjectDef::Parser_ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ObjectDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , size( loader )
          , inheritance( loader )
    {
    }
    Parser_ObjectDef::Parser_ObjectDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Size >& size, const data::Ptr< data::AST::Parser_Inheritance >& inheritance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ObjectDef >( loader, this ) )          , p_AST_Parser_ContextDef( loader )
          , size( size )
          , inheritance( inheritance )
    {
    }
    bool Parser_ObjectDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_ObjectDef >( loader, const_cast< Parser_ObjectDef* >( this ) ) };
    }
    void Parser_ObjectDef::set_inheritance_pointer()
    {
        p_AST_Parser_ContextDef->m_inheritance = data::Ptr< data::AST::Parser_ObjectDef >( p_AST_Parser_ContextDef, this );
    }
    void Parser_ObjectDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ContextDef );
        loader.load( size );
        loader.load( inheritance );
    }
    void Parser_ObjectDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ContextDef );
        storer.store( size );
        storer.store( inheritance );
    }
    void Parser_ObjectDef::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "size", size } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_SourceRoot >( loader, const_cast< Parser_SourceRoot* >( this ) ) };
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
    void Parser_SourceRoot::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "component", component } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "ast", ast } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Parser_IncludeRoot : public mega::io::Object
    Parser_IncludeRoot::Parser_IncludeRoot( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_IncludeRoot >( loader, this ) )          , p_AST_Parser_SourceRoot( loader )
    {
    }
    bool Parser_IncludeRoot::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_IncludeRoot >( loader, const_cast< Parser_IncludeRoot* >( this ) ) };
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
    void Parser_IncludeRoot::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_ObjectSourceRoot >( loader, const_cast< Parser_ObjectSourceRoot* >( this ) ) };
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
    void Parser_ObjectSourceRoot::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
    void Parser_ContextDef::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace Tree
{
    // struct Interface_DimensionTrait : public mega::io::Object
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_DimensionTrait >( loader, this ) )          , p_AST_Parser_Dimension( loader )
          , p_PerSourceSymbols_Interface_DimensionTrait( loader )
          , p_PerSourceDerivations_Interface_DimensionTrait( loader )
          , p_Clang_Interface_DimensionTrait( loader )
          , parent( loader )
    {
    }
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_IContext >& parent)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_DimensionTrait >( loader, this ) )          , p_AST_Parser_Dimension( loader )
          , p_PerSourceSymbols_Interface_DimensionTrait( loader )
          , p_PerSourceDerivations_Interface_DimensionTrait( loader )
          , p_Clang_Interface_DimensionTrait( loader )
          , parent( parent )
    {
    }
    bool Interface_DimensionTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_DimensionTrait >( loader, const_cast< Interface_DimensionTrait* >( this ) ) };
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
    void Interface_DimensionTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_LinkTrait : public mega::io::Object
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_LinkTrait >( loader, this ) )          , p_AST_Parser_Link( loader )
          , p_PerSourceSymbols_Interface_LinkTrait( loader )
          , p_PerSourceDerivations_Interface_LinkTrait( loader )
          , parent( loader )
    {
    }
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_IContext >& parent)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_LinkTrait >( loader, this ) )          , p_AST_Parser_Link( loader )
          , p_PerSourceSymbols_Interface_LinkTrait( loader )
          , p_PerSourceDerivations_Interface_LinkTrait( loader )
          , parent( parent )
    {
    }
    bool Interface_LinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_LinkTrait >( loader, const_cast< Interface_LinkTrait* >( this ) ) };
    }
    void Interface_LinkTrait::set_inheritance_pointer()
    {
        p_AST_Parser_Link->m_inheritance = data::Ptr< data::Tree::Interface_LinkTrait >( p_AST_Parser_Link, this );
    }
    void Interface_LinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_Link );
        loader.load( parent );
    }
    void Interface_LinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_Link );
        storer.store( parent );
    }
    void Interface_LinkTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_LinkTrait" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent", parent } } );
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_InheritanceTrait >( loader, const_cast< Interface_InheritanceTrait* >( this ) ) };
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
    void Interface_InheritanceTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_InheritanceTrait" },
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
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_ReturnTypeTrait >( loader, const_cast< Interface_ReturnTypeTrait* >( this ) ) };
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
    void Interface_ReturnTypeTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_ArgumentListTrait >( loader, const_cast< Interface_ArgumentListTrait* >( this ) ) };
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
    void Interface_ArgumentListTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_ArgumentListTrait" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Interface_PartTrait : public mega::io::Object
    Interface_PartTrait::Interface_PartTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_PartTrait >( loader, this ) )          , p_AST_Parser_Part( loader )
          , parent( loader )
    {
    }
    Interface_PartTrait::Interface_PartTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_IContext >& parent)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_PartTrait >( loader, this ) )          , p_AST_Parser_Part( loader )
          , parent( parent )
    {
    }
    bool Interface_PartTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_PartTrait >( loader, const_cast< Interface_PartTrait* >( this ) ) };
    }
    void Interface_PartTrait::set_inheritance_pointer()
    {
        p_AST_Parser_Part->m_inheritance = data::Ptr< data::Tree::Interface_PartTrait >( p_AST_Parser_Part, this );
    }
    void Interface_PartTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_Part );
        loader.load( parent );
    }
    void Interface_PartTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_Part );
        storer.store( parent );
    }
    void Interface_PartTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_PartTrait" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent", parent } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_RequirementTrait : public mega::io::Object
    Interface_RequirementTrait::Interface_RequirementTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_RequirementTrait >( loader, this ) )          , p_AST_Parser_Requirement( loader )
          , p_Clang_Interface_RequirementTrait( loader )
          , parent( loader )
    {
    }
    Interface_RequirementTrait::Interface_RequirementTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_IContext >& parent)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_RequirementTrait >( loader, this ) )          , p_AST_Parser_Requirement( loader )
          , p_Clang_Interface_RequirementTrait( loader )
          , parent( parent )
    {
    }
    bool Interface_RequirementTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_RequirementTrait >( loader, const_cast< Interface_RequirementTrait* >( this ) ) };
    }
    void Interface_RequirementTrait::set_inheritance_pointer()
    {
        p_AST_Parser_Requirement->m_inheritance = data::Ptr< data::Tree::Interface_RequirementTrait >( p_AST_Parser_Requirement, this );
    }
    void Interface_RequirementTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_Requirement );
        loader.load( parent );
    }
    void Interface_RequirementTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_Requirement );
        storer.store( parent );
    }
    void Interface_RequirementTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_RequirementTrait" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent", parent } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_TransitionTypeTrait : public mega::io::Object
    Interface_TransitionTypeTrait::Interface_TransitionTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_TransitionTypeTrait >( loader, this ) )          , p_AST_Parser_Transition( loader )
          , p_Clang_Interface_TransitionTypeTrait( loader )
    {
    }
    bool Interface_TransitionTypeTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_TransitionTypeTrait >( loader, const_cast< Interface_TransitionTypeTrait* >( this ) ) };
    }
    void Interface_TransitionTypeTrait::set_inheritance_pointer()
    {
        p_AST_Parser_Transition->m_inheritance = data::Ptr< data::Tree::Interface_TransitionTypeTrait >( p_AST_Parser_Transition, this );
    }
    void Interface_TransitionTypeTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_Transition );
    }
    void Interface_TransitionTypeTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_Transition );
    }
    void Interface_TransitionTypeTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_TransitionTypeTrait" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Interface_EventTypeTrait : public mega::io::Object
    Interface_EventTypeTrait::Interface_EventTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_EventTypeTrait >( loader, this ) )          , p_AST_Parser_ArgumentList( loader )
          , p_Clang_Interface_EventTypeTrait( loader )
    {
    }
    bool Interface_EventTypeTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_EventTypeTrait >( loader, const_cast< Interface_EventTypeTrait* >( this ) ) };
    }
    void Interface_EventTypeTrait::set_inheritance_pointer()
    {
        p_AST_Parser_ArgumentList->m_inheritance = data::Ptr< data::Tree::Interface_EventTypeTrait >( p_AST_Parser_ArgumentList, this );
    }
    void Interface_EventTypeTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_ArgumentList );
    }
    void Interface_EventTypeTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_ArgumentList );
    }
    void Interface_EventTypeTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_EventTypeTrait" },
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
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_SizeTrait >( loader, const_cast< Interface_SizeTrait* >( this ) ) };
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
    void Interface_SizeTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_ContextGroup >( loader, const_cast< Interface_ContextGroup* >( this ) ) };
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
    void Interface_ContextGroup::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_Root >( loader, const_cast< Interface_Root* >( this ) ) };
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
    void Interface_Root::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_IContext : public mega::io::Object
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_IContext >( loader, this ) )          , p_Tree_Interface_ContextGroup( loader )
          , p_PerSourceSymbols_Interface_IContext( loader )
          , p_PerSourceDerivations_Interface_IContext( loader )
          , parent( loader )
          , component( loader )
    {
    }
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& identifier, const data::Ptr< data::Tree::Interface_ContextGroup >& parent, const data::Ptr< data::Components::Components_Component >& component)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_IContext >( loader, this ) )          , p_Tree_Interface_ContextGroup( loader )
          , p_PerSourceSymbols_Interface_IContext( loader )
          , p_PerSourceDerivations_Interface_IContext( loader )
          , identifier( identifier )
          , parent( parent )
          , component( component )
    {
    }
    bool Interface_IContext::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_IContext >( loader, const_cast< Interface_IContext* >( this ) ) };
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
        loader.load( component );
    }
    void Interface_IContext::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_ContextGroup );
        storer.store( identifier );
        storer.store( parent );
        storer.store( component );
    }
    void Interface_IContext::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent", parent } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "component", component } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_InvocationContext : public mega::io::Object
    Interface_InvocationContext::Interface_InvocationContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_InvocationContext >( loader, this ) )          , p_Tree_Interface_IContext( loader )
          , p_Locations_Interface_InvocationContext( loader )
    {
    }
    bool Interface_InvocationContext::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_InvocationContext >( loader, const_cast< Interface_InvocationContext* >( this ) ) };
    }
    void Interface_InvocationContext::set_inheritance_pointer()
    {
        p_Tree_Interface_IContext->m_inheritance = data::Ptr< data::Tree::Interface_InvocationContext >( p_Tree_Interface_IContext, this );
    }
    void Interface_InvocationContext::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_IContext );
    }
    void Interface_InvocationContext::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
    }
    void Interface_InvocationContext::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_InvocationContext" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
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
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_Namespace >( loader, const_cast< Interface_Namespace* >( this ) ) };
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
    void Interface_Namespace::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "namespace_defs", namespace_defs } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_traits", dimension_traits.value() } } );
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_Abstract >( loader, const_cast< Interface_Abstract* >( this ) ) };
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
        loader.load( link_traits );
        loader.load( inheritance_trait );
        loader.load( size_trait );
        loader.load( requirement_traits );
    }
    void Interface_Abstract::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( abstract_defs );
        VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Interface_Abstract.dimension_traits has NOT been set" );
        storer.store( dimension_traits );
        VERIFY_RTE_MSG( link_traits.has_value(), "Tree::Interface_Abstract.link_traits has NOT been set" );
        storer.store( link_traits );
        VERIFY_RTE_MSG( inheritance_trait.has_value(), "Tree::Interface_Abstract.inheritance_trait has NOT been set" );
        storer.store( inheritance_trait );
        VERIFY_RTE_MSG( size_trait.has_value(), "Tree::Interface_Abstract.size_trait has NOT been set" );
        storer.store( size_trait );
        VERIFY_RTE_MSG( requirement_traits.has_value(), "Tree::Interface_Abstract.requirement_traits has NOT been set" );
        storer.store( requirement_traits );
    }
    void Interface_Abstract::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_traits", dimension_traits.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "link_traits", link_traits.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_trait", inheritance_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "size_trait", size_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "requirement_traits", requirement_traits.value() } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_State : public mega::io::Object
    Interface_State::Interface_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_State >( loader, this ) )          , p_Tree_Interface_InvocationContext( loader )
    {
    }
    Interface_State::Interface_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_StateDef > >& state_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_State >( loader, this ) )          , p_Tree_Interface_InvocationContext( loader )
          , state_defs( state_defs )
    {
    }
    bool Interface_State::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_State >( loader, const_cast< Interface_State* >( this ) ) };
    }
    void Interface_State::set_inheritance_pointer()
    {
        p_Tree_Interface_InvocationContext->m_inheritance = data::Ptr< data::Tree::Interface_State >( p_Tree_Interface_InvocationContext, this );
    }
    void Interface_State::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_InvocationContext );
        loader.load( state_defs );
        loader.load( dimension_traits );
        loader.load( link_traits );
        loader.load( inheritance_trait );
        loader.load( size_trait );
        loader.load( transition_trait );
        loader.load( part_traits );
        loader.load( requirement_traits );
    }
    void Interface_State::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_InvocationContext );
        storer.store( state_defs );
        VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Interface_State.dimension_traits has NOT been set" );
        storer.store( dimension_traits );
        VERIFY_RTE_MSG( link_traits.has_value(), "Tree::Interface_State.link_traits has NOT been set" );
        storer.store( link_traits );
        VERIFY_RTE_MSG( inheritance_trait.has_value(), "Tree::Interface_State.inheritance_trait has NOT been set" );
        storer.store( inheritance_trait );
        VERIFY_RTE_MSG( size_trait.has_value(), "Tree::Interface_State.size_trait has NOT been set" );
        storer.store( size_trait );
        VERIFY_RTE_MSG( transition_trait.has_value(), "Tree::Interface_State.transition_trait has NOT been set" );
        storer.store( transition_trait );
        VERIFY_RTE_MSG( part_traits.has_value(), "Tree::Interface_State.part_traits has NOT been set" );
        storer.store( part_traits );
        VERIFY_RTE_MSG( requirement_traits.has_value(), "Tree::Interface_State.requirement_traits has NOT been set" );
        storer.store( requirement_traits );
    }
    void Interface_State::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_State" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "state_defs", state_defs } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_traits", dimension_traits.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "link_traits", link_traits.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_trait", inheritance_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "size_trait", size_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "transition_trait", transition_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "part_traits", part_traits.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "requirement_traits", requirement_traits.value() } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Action : public mega::io::Object
    Interface_Action::Interface_Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Action >( loader, this ) )          , p_Tree_Interface_State( loader )
    {
    }
    bool Interface_Action::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_Action >( loader, const_cast< Interface_Action* >( this ) ) };
    }
    void Interface_Action::set_inheritance_pointer()
    {
        p_Tree_Interface_State->m_inheritance = data::Ptr< data::Tree::Interface_Action >( p_Tree_Interface_State, this );
    }
    void Interface_Action::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_State );
    }
    void Interface_Action::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_State );
    }
    void Interface_Action::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_Action" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Interface_Component : public mega::io::Object
    Interface_Component::Interface_Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Component >( loader, this ) )          , p_Tree_Interface_State( loader )
    {
    }
    bool Interface_Component::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_Component >( loader, const_cast< Interface_Component* >( this ) ) };
    }
    void Interface_Component::set_inheritance_pointer()
    {
        p_Tree_Interface_State->m_inheritance = data::Ptr< data::Tree::Interface_Component >( p_Tree_Interface_State, this );
    }
    void Interface_Component::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_State );
    }
    void Interface_Component::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_State );
    }
    void Interface_Component::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_Component" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
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
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_Event >( loader, const_cast< Interface_Event* >( this ) ) };
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
    void Interface_Event::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_traits", dimension_traits.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_trait", inheritance_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "size_trait", size_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Interupt : public mega::io::Object
    Interface_Interupt::Interface_Interupt( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Interupt >( loader, this ) )          , p_Tree_Interface_InvocationContext( loader )
          , events_trait( loader )
    {
    }
    Interface_Interupt::Interface_Interupt( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_InteruptDef > >& interupt_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Interupt >( loader, this ) )          , p_Tree_Interface_InvocationContext( loader )
          , interupt_defs( interupt_defs )
    {
    }
    bool Interface_Interupt::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_Interupt >( loader, const_cast< Interface_Interupt* >( this ) ) };
    }
    void Interface_Interupt::set_inheritance_pointer()
    {
        p_Tree_Interface_InvocationContext->m_inheritance = data::Ptr< data::Tree::Interface_Interupt >( p_Tree_Interface_InvocationContext, this );
    }
    void Interface_Interupt::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_InvocationContext );
        loader.load( interupt_defs );
        loader.load( events_trait );
        loader.load( transition_trait );
    }
    void Interface_Interupt::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_InvocationContext );
        storer.store( interupt_defs );
        VERIFY_RTE_MSG( events_trait.has_value(), "Tree::Interface_Interupt.events_trait has NOT been set" );
        storer.store( events_trait );
        VERIFY_RTE_MSG( transition_trait.has_value(), "Tree::Interface_Interupt.transition_trait has NOT been set" );
        storer.store( transition_trait );
    }
    void Interface_Interupt::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_Interupt" },
                { "filetype" , "Tree" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interupt_defs", interupt_defs } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "events_trait", events_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "transition_trait", transition_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_Function : public mega::io::Object
    Interface_Function::Interface_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Function >( loader, this ) )          , p_Tree_Interface_InvocationContext( loader )
          , return_type_trait( loader )
          , arguments_trait( loader )
    {
    }
    Interface_Function::Interface_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_FunctionDef > >& function_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Function >( loader, this ) )          , p_Tree_Interface_InvocationContext( loader )
          , function_defs( function_defs )
    {
    }
    bool Interface_Function::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_Function >( loader, const_cast< Interface_Function* >( this ) ) };
    }
    void Interface_Function::set_inheritance_pointer()
    {
        p_Tree_Interface_InvocationContext->m_inheritance = data::Ptr< data::Tree::Interface_Function >( p_Tree_Interface_InvocationContext, this );
    }
    void Interface_Function::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_InvocationContext );
        loader.load( function_defs );
        loader.load( return_type_trait );
        loader.load( arguments_trait );
    }
    void Interface_Function::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_InvocationContext );
        storer.store( function_defs );
        VERIFY_RTE_MSG( return_type_trait.has_value(), "Tree::Interface_Function.return_type_trait has NOT been set" );
        storer.store( return_type_trait );
        VERIFY_RTE_MSG( arguments_trait.has_value(), "Tree::Interface_Function.arguments_trait has NOT been set" );
        storer.store( arguments_trait );
    }
    void Interface_Function::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "return_type_trait", return_type_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "arguments_trait", arguments_trait.value() } } );
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_Object >( loader, const_cast< Interface_Object* >( this ) ) };
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
        loader.load( link_traits );
        loader.load( inheritance_trait );
        loader.load( size_trait );
    }
    void Interface_Object::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( object_defs );
        VERIFY_RTE_MSG( dimension_traits.has_value(), "Tree::Interface_Object.dimension_traits has NOT been set" );
        storer.store( dimension_traits );
        VERIFY_RTE_MSG( link_traits.has_value(), "Tree::Interface_Object.link_traits has NOT been set" );
        storer.store( link_traits );
        VERIFY_RTE_MSG( inheritance_trait.has_value(), "Tree::Interface_Object.inheritance_trait has NOT been set" );
        storer.store( inheritance_trait );
        VERIFY_RTE_MSG( size_trait.has_value(), "Tree::Interface_Object.size_trait has NOT been set" );
        storer.store( size_trait );
    }
    void Interface_Object::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension_traits", dimension_traits.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "link_traits", link_traits.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_trait", inheritance_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "size_trait", size_trait.value() } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace MetaAnalysis
{
    // struct Meta_SequenceAction : public mega::io::Object
    Meta_SequenceAction::Meta_SequenceAction( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MetaAnalysis::Meta_SequenceAction >( loader, this ) )          , p_Tree_Interface_Action( loader )
    {
    }
    bool Meta_SequenceAction::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MetaAnalysis::Meta_SequenceAction >( loader, const_cast< Meta_SequenceAction* >( this ) ) };
    }
    void Meta_SequenceAction::set_inheritance_pointer()
    {
        p_Tree_Interface_Action->m_inheritance = data::Ptr< data::MetaAnalysis::Meta_SequenceAction >( p_Tree_Interface_Action, this );
    }
    void Meta_SequenceAction::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_Action );
    }
    void Meta_SequenceAction::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_Action );
    }
    void Meta_SequenceAction::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Meta_SequenceAction" },
                { "filetype" , "MetaAnalysis" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Meta_StackAction : public mega::io::Object
    Meta_StackAction::Meta_StackAction( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MetaAnalysis::Meta_StackAction >( loader, this ) )          , p_Tree_Interface_Action( loader )
    {
    }
    bool Meta_StackAction::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MetaAnalysis::Meta_StackAction >( loader, const_cast< Meta_StackAction* >( this ) ) };
    }
    void Meta_StackAction::set_inheritance_pointer()
    {
        p_Tree_Interface_Action->m_inheritance = data::Ptr< data::MetaAnalysis::Meta_StackAction >( p_Tree_Interface_Action, this );
    }
    void Meta_StackAction::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_Action );
    }
    void Meta_StackAction::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_Action );
    }
    void Meta_StackAction::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Meta_StackAction" },
                { "filetype" , "MetaAnalysis" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Meta_PlanAction : public mega::io::Object
    Meta_PlanAction::Meta_PlanAction( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MetaAnalysis::Meta_PlanAction >( loader, this ) )          , p_Tree_Interface_Action( loader )
    {
    }
    bool Meta_PlanAction::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MetaAnalysis::Meta_PlanAction >( loader, const_cast< Meta_PlanAction* >( this ) ) };
    }
    void Meta_PlanAction::set_inheritance_pointer()
    {
        p_Tree_Interface_Action->m_inheritance = data::Ptr< data::MetaAnalysis::Meta_PlanAction >( p_Tree_Interface_Action, this );
    }
    void Meta_PlanAction::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_Action );
    }
    void Meta_PlanAction::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_Action );
    }
    void Meta_PlanAction::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Meta_PlanAction" },
                { "filetype" , "MetaAnalysis" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
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
        return m_inheritance == data::Variant{ data::Ptr< data::DPGraph::Dependencies_Glob >( loader, const_cast< Dependencies_Glob* >( this ) ) };
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
    void Dependencies_Glob::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "glob", glob } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Dependencies_SourceFileDependencies : public mega::io::Object
    Dependencies_SourceFileDependencies::Dependencies_SourceFileDependencies( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_SourceFileDependencies >( loader, this ) )    {
    }
    Dependencies_SourceFileDependencies::Dependencies_SourceFileDependencies( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::io::megaFilePath& source_file, const mega::U64& hash_code, const std::vector< data::Ptr< data::DPGraph::Dependencies_Glob > >& globs, const std::vector< boost::filesystem::path >& resolution)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_SourceFileDependencies >( loader, this ) )          , source_file( source_file )
          , hash_code( hash_code )
          , globs( globs )
          , resolution( resolution )
    {
    }
    bool Dependencies_SourceFileDependencies::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::DPGraph::Dependencies_SourceFileDependencies >( loader, const_cast< Dependencies_SourceFileDependencies* >( this ) ) };
    }
    void Dependencies_SourceFileDependencies::set_inheritance_pointer()
    {
    }
    void Dependencies_SourceFileDependencies::load( mega::io::Loader& loader )
    {
        loader.load( source_file );
        loader.load( hash_code );
        loader.load( globs );
        loader.load( resolution );
    }
    void Dependencies_SourceFileDependencies::store( mega::io::Storer& storer ) const
    {
        storer.store( source_file );
        storer.store( hash_code );
        storer.store( globs );
        storer.store( resolution );
    }
    void Dependencies_SourceFileDependencies::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Dependencies_SourceFileDependencies" },
                { "filetype" , "DPGraph" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_file", source_file } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "hash_code", hash_code } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "globs", globs } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "resolution", resolution } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Dependencies_TransitiveDependencies : public mega::io::Object
    Dependencies_TransitiveDependencies::Dependencies_TransitiveDependencies( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_TransitiveDependencies >( loader, this ) )    {
    }
    Dependencies_TransitiveDependencies::Dependencies_TransitiveDependencies( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< mega::io::megaFilePath >& mega_source_files)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_TransitiveDependencies >( loader, this ) )          , mega_source_files( mega_source_files )
    {
    }
    bool Dependencies_TransitiveDependencies::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::DPGraph::Dependencies_TransitiveDependencies >( loader, const_cast< Dependencies_TransitiveDependencies* >( this ) ) };
    }
    void Dependencies_TransitiveDependencies::set_inheritance_pointer()
    {
    }
    void Dependencies_TransitiveDependencies::load( mega::io::Loader& loader )
    {
        loader.load( mega_source_files );
    }
    void Dependencies_TransitiveDependencies::store( mega::io::Storer& storer ) const
    {
        storer.store( mega_source_files );
    }
    void Dependencies_TransitiveDependencies::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Dependencies_TransitiveDependencies" },
                { "filetype" , "DPGraph" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "mega_source_files", mega_source_files } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Dependencies_Analysis : public mega::io::Object
    Dependencies_Analysis::Dependencies_Analysis( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_Analysis >( loader, this ) )    {
    }
    Dependencies_Analysis::Dependencies_Analysis( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::DPGraph::Dependencies_SourceFileDependencies > >& objects, const std::map< mega::io::megaFilePath, data::Ptr< data::DPGraph::Dependencies_TransitiveDependencies > >& mega_dependencies, const std::map< mega::io::cppFilePath, data::Ptr< data::DPGraph::Dependencies_TransitiveDependencies > >& cpp_dependencies, const std::vector< mega::io::megaFilePath >& topological_mega_files)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::DPGraph::Dependencies_Analysis >( loader, this ) )          , objects( objects )
          , mega_dependencies( mega_dependencies )
          , cpp_dependencies( cpp_dependencies )
          , topological_mega_files( topological_mega_files )
    {
    }
    bool Dependencies_Analysis::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::DPGraph::Dependencies_Analysis >( loader, const_cast< Dependencies_Analysis* >( this ) ) };
    }
    void Dependencies_Analysis::set_inheritance_pointer()
    {
    }
    void Dependencies_Analysis::load( mega::io::Loader& loader )
    {
        loader.load( objects );
        loader.load( mega_dependencies );
        loader.load( cpp_dependencies );
        loader.load( topological_mega_files );
    }
    void Dependencies_Analysis::store( mega::io::Storer& storer ) const
    {
        storer.store( objects );
        storer.store( mega_dependencies );
        storer.store( cpp_dependencies );
        storer.store( topological_mega_files );
    }
    void Dependencies_Analysis::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "mega_dependencies", mega_dependencies } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "cpp_dependencies", cpp_dependencies } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "topological_mega_files", topological_mega_files } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace SymbolTable
{
    // struct Symbols_SymbolTypeID : public mega::io::Object
    Symbols_SymbolTypeID::Symbols_SymbolTypeID( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_SymbolTypeID >( loader, this ) )    {
    }
    Symbols_SymbolTypeID::Symbols_SymbolTypeID( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& symbol, const mega::TypeID& id, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& contexts, const std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > >& dimensions, const std::vector< data::Ptr< data::Tree::Interface_LinkTrait > >& links)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_SymbolTypeID >( loader, this ) )          , symbol( symbol )
          , id( id )
          , contexts( contexts )
          , dimensions( dimensions )
          , links( links )
    {
    }
    bool Symbols_SymbolTypeID::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::SymbolTable::Symbols_SymbolTypeID >( loader, const_cast< Symbols_SymbolTypeID* >( this ) ) };
    }
    void Symbols_SymbolTypeID::set_inheritance_pointer()
    {
    }
    void Symbols_SymbolTypeID::load( mega::io::Loader& loader )
    {
        loader.load( symbol );
        loader.load( id );
        loader.load( contexts );
        loader.load( dimensions );
        loader.load( links );
    }
    void Symbols_SymbolTypeID::store( mega::io::Storer& storer ) const
    {
        storer.store( symbol );
        storer.store( id );
        storer.store( contexts );
        storer.store( dimensions );
        storer.store( links );
    }
    void Symbols_SymbolTypeID::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Symbols_SymbolTypeID" },
                { "filetype" , "SymbolTable" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbol", symbol } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "contexts", contexts } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "links", links } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Symbols_InterfaceTypeID : public mega::io::Object
    Symbols_InterfaceTypeID::Symbols_InterfaceTypeID( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_InterfaceTypeID >( loader, this ) )    {
    }
    Symbols_InterfaceTypeID::Symbols_InterfaceTypeID( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::TypeIDSequence& symbol_ids, const mega::TypeID& id, const std::optional< data::Ptr< data::Tree::Interface_IContext > >& context, const std::optional< data::Ptr< data::Tree::Interface_DimensionTrait > >& dimension, const std::optional< data::Ptr< data::Tree::Interface_LinkTrait > >& link)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_InterfaceTypeID >( loader, this ) )          , symbol_ids( symbol_ids )
          , id( id )
          , context( context )
          , dimension( dimension )
          , link( link )
    {
    }
    bool Symbols_InterfaceTypeID::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::SymbolTable::Symbols_InterfaceTypeID >( loader, const_cast< Symbols_InterfaceTypeID* >( this ) ) };
    }
    void Symbols_InterfaceTypeID::set_inheritance_pointer()
    {
    }
    void Symbols_InterfaceTypeID::load( mega::io::Loader& loader )
    {
        loader.load( symbol_ids );
        loader.load( id );
        loader.load( context );
        loader.load( dimension );
        loader.load( link );
    }
    void Symbols_InterfaceTypeID::store( mega::io::Storer& storer ) const
    {
        storer.store( symbol_ids );
        storer.store( id );
        storer.store( context );
        storer.store( dimension );
        storer.store( link );
    }
    void Symbols_InterfaceTypeID::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Symbols_InterfaceTypeID" },
                { "filetype" , "SymbolTable" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbol_ids", symbol_ids } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "context", context } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension", dimension } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "link", link } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Symbols_SymbolTable : public mega::io::Object
    Symbols_SymbolTable::Symbols_SymbolTable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_SymbolTable >( loader, this ) )          , p_ConcreteTable_Symbols_SymbolTable( loader )
    {
    }
    Symbols_SymbolTable::Symbols_SymbolTable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::map< std::string, data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& symbol_names, const std::map< mega::TypeID, data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& symbol_type_ids, const std::map< mega::TypeIDSequence, data::Ptr< data::SymbolTable::Symbols_InterfaceTypeID > >& interface_type_id_sequences, const std::map< mega::TypeID, data::Ptr< data::SymbolTable::Symbols_InterfaceTypeID > >& interface_type_ids)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::SymbolTable::Symbols_SymbolTable >( loader, this ) )          , p_ConcreteTable_Symbols_SymbolTable( loader )
          , symbol_names( symbol_names )
          , symbol_type_ids( symbol_type_ids )
          , interface_type_id_sequences( interface_type_id_sequences )
          , interface_type_ids( interface_type_ids )
    {
    }
    bool Symbols_SymbolTable::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::SymbolTable::Symbols_SymbolTable >( loader, const_cast< Symbols_SymbolTable* >( this ) ) };
    }
    void Symbols_SymbolTable::set_inheritance_pointer()
    {
    }
    void Symbols_SymbolTable::load( mega::io::Loader& loader )
    {
        loader.load( symbol_names );
        loader.load( symbol_type_ids );
        loader.load( interface_type_id_sequences );
        loader.load( interface_type_ids );
    }
    void Symbols_SymbolTable::store( mega::io::Storer& storer ) const
    {
        storer.store( symbol_names );
        storer.store( symbol_type_ids );
        storer.store( interface_type_id_sequences );
        storer.store( interface_type_ids );
    }
    void Symbols_SymbolTable::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "symbol_names", symbol_names } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbol_type_ids", symbol_type_ids } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_type_id_sequences", interface_type_id_sequences } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_type_ids", interface_type_ids } } );
            _part__[ "properties" ].push_back( property );
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
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait, const mega::TypeID& symbol_id, const mega::TypeID& interface_id)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_DimensionTrait( p_Tree_Interface_DimensionTrait )
          , symbol_id( symbol_id )
          , interface_id( interface_id )
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
        loader.load( symbol_id );
        loader.load( interface_id );
    }
    void Interface_DimensionTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_DimensionTrait );
        storer.store( symbol_id );
        storer.store( interface_id );
    }
    void Interface_DimensionTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "symbol_id", symbol_id } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_id", interface_id } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_LinkTrait : public mega::io::Object
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_LinkTrait( loader )
    {
    }
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_LinkTrait > p_Tree_Interface_LinkTrait, const mega::TypeID& symbol_id, const mega::TypeID& interface_id)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_LinkTrait( p_Tree_Interface_LinkTrait )
          , symbol_id( symbol_id )
          , interface_id( interface_id )
    {
    }
    bool Interface_LinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_LinkTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_LinkTrait->p_PerSourceSymbols_Interface_LinkTrait = data::Ptr< data::PerSourceSymbols::Interface_LinkTrait >( p_Tree_Interface_LinkTrait, this );
    }
    void Interface_LinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_LinkTrait );
        loader.load( symbol_id );
        loader.load( interface_id );
    }
    void Interface_LinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_LinkTrait );
        storer.store( symbol_id );
        storer.store( interface_id );
    }
    void Interface_LinkTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_LinkTrait" },
                { "filetype" , "PerSourceSymbols" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbol_id", symbol_id } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_id", interface_id } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_IContext : public mega::io::Object
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_IContext( loader )
    {
    }
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext, const mega::TypeID& symbol_id, const mega::TypeID& interface_id)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_IContext( p_Tree_Interface_IContext )
          , symbol_id( symbol_id )
          , interface_id( interface_id )
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
        loader.load( symbol_id );
        loader.load( interface_id );
    }
    void Interface_IContext::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_IContext );
        storer.store( symbol_id );
        storer.store( interface_id );
    }
    void Interface_IContext::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "symbol_id", symbol_id } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_id", interface_id } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace Clang
{
    // struct Interface_TypePath : public mega::io::Object
    Interface_TypePath::Interface_TypePath( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Clang::Interface_TypePath >( loader, this ) )    {
    }
    Interface_TypePath::Interface_TypePath( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& types)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Clang::Interface_TypePath >( loader, this ) )          , types( types )
    {
    }
    bool Interface_TypePath::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Clang::Interface_TypePath >( loader, const_cast< Interface_TypePath* >( this ) ) };
    }
    void Interface_TypePath::set_inheritance_pointer()
    {
    }
    void Interface_TypePath::load( mega::io::Loader& loader )
    {
        loader.load( types );
    }
    void Interface_TypePath::store( mega::io::Storer& storer ) const
    {
        storer.store( types );
    }
    void Interface_TypePath::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_TypePath" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "types", types } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_TypePathVariant : public mega::io::Object
    Interface_TypePathVariant::Interface_TypePathVariant( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Clang::Interface_TypePathVariant >( loader, this ) )    {
    }
    Interface_TypePathVariant::Interface_TypePathVariant( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Clang::Interface_TypePath > >& sequence)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Clang::Interface_TypePathVariant >( loader, this ) )          , sequence( sequence )
    {
    }
    bool Interface_TypePathVariant::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Clang::Interface_TypePathVariant >( loader, const_cast< Interface_TypePathVariant* >( this ) ) };
    }
    void Interface_TypePathVariant::set_inheritance_pointer()
    {
    }
    void Interface_TypePathVariant::load( mega::io::Loader& loader )
    {
        loader.load( sequence );
    }
    void Interface_TypePathVariant::store( mega::io::Storer& storer ) const
    {
        storer.store( sequence );
    }
    void Interface_TypePathVariant::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_TypePathVariant" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "sequence", sequence } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_DimensionTrait : public mega::io::Object
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_DimensionTrait( loader )
          , mangle( loader )
    {
    }
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait, const std::string& canonical_type, const data::Ptr< data::Components::MegaMangle_Mangle >& mangle, const std::string& erased_type, const mega::U64& size, const mega::U64& alignment, const bool& simple, const std::vector< data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& symbols)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_DimensionTrait( p_Tree_Interface_DimensionTrait )
          , canonical_type( canonical_type )
          , mangle( mangle )
          , erased_type( erased_type )
          , size( size )
          , alignment( alignment )
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
        loader.load( mangle );
        loader.load( erased_type );
        loader.load( size );
        loader.load( alignment );
        loader.load( simple );
        loader.load( symbols );
    }
    void Interface_DimensionTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_DimensionTrait );
        storer.store( canonical_type );
        storer.store( mangle );
        storer.store( erased_type );
        storer.store( size );
        storer.store( alignment );
        storer.store( simple );
        storer.store( symbols );
    }
    void Interface_DimensionTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "mangle", mangle } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "erased_type", erased_type } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "size", size } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "alignment", alignment } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "simple", simple } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "symbols", symbols } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_TypedLinkTrait : public mega::io::Object
    Interface_TypedLinkTrait::Interface_TypedLinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Clang::Interface_TypedLinkTrait >( loader, this ) )          , p_Tree_Interface_LinkTrait( loader )
    {
    }
    Interface_TypedLinkTrait::Interface_TypedLinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& tuple)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Clang::Interface_TypedLinkTrait >( loader, this ) )          , p_Tree_Interface_LinkTrait( loader )
          , tuple( tuple )
    {
    }
    bool Interface_TypedLinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Clang::Interface_TypedLinkTrait >( loader, const_cast< Interface_TypedLinkTrait* >( this ) ) };
    }
    void Interface_TypedLinkTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_LinkTrait->m_inheritance = data::Ptr< data::Clang::Interface_TypedLinkTrait >( p_Tree_Interface_LinkTrait, this );
    }
    void Interface_TypedLinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_LinkTrait );
        loader.load( tuple );
    }
    void Interface_TypedLinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_LinkTrait );
        storer.store( tuple );
    }
    void Interface_TypedLinkTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_TypedLinkTrait" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "tuple", tuple } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_ObjectLinkTrait : public mega::io::Object
    Interface_ObjectLinkTrait::Interface_ObjectLinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Clang::Interface_ObjectLinkTrait >( loader, this ) )          , p_Clang_Interface_TypedLinkTrait( loader )
          , p_PerSourceModel_Interface_ObjectLinkTrait( loader )
    {
    }
    bool Interface_ObjectLinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Clang::Interface_ObjectLinkTrait >( loader, const_cast< Interface_ObjectLinkTrait* >( this ) ) };
    }
    void Interface_ObjectLinkTrait::set_inheritance_pointer()
    {
        p_Clang_Interface_TypedLinkTrait->m_inheritance = data::Ptr< data::Clang::Interface_ObjectLinkTrait >( p_Clang_Interface_TypedLinkTrait, this );
    }
    void Interface_ObjectLinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Clang_Interface_TypedLinkTrait );
    }
    void Interface_ObjectLinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Clang_Interface_TypedLinkTrait );
    }
    void Interface_ObjectLinkTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_ObjectLinkTrait" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Interface_ComponentLinkTrait : public mega::io::Object
    Interface_ComponentLinkTrait::Interface_ComponentLinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Clang::Interface_ComponentLinkTrait >( loader, this ) )          , p_Clang_Interface_TypedLinkTrait( loader )
    {
    }
    bool Interface_ComponentLinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Clang::Interface_ComponentLinkTrait >( loader, const_cast< Interface_ComponentLinkTrait* >( this ) ) };
    }
    void Interface_ComponentLinkTrait::set_inheritance_pointer()
    {
        p_Clang_Interface_TypedLinkTrait->m_inheritance = data::Ptr< data::Clang::Interface_ComponentLinkTrait >( p_Clang_Interface_TypedLinkTrait, this );
    }
    void Interface_ComponentLinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Clang_Interface_TypedLinkTrait );
    }
    void Interface_ComponentLinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Clang_Interface_TypedLinkTrait );
    }
    void Interface_ComponentLinkTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_ComponentLinkTrait" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
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
    void Interface_InheritanceTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
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
    void Interface_ReturnTypeTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
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
    void Interface_ArgumentListTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_RequirementTrait : public mega::io::Object
    Interface_RequirementTrait::Interface_RequirementTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_RequirementTrait( loader )
    {
    }
    Interface_RequirementTrait::Interface_RequirementTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_RequirementTrait > p_Tree_Interface_RequirementTrait, const std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& tuple)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_RequirementTrait( p_Tree_Interface_RequirementTrait )
          , tuple( tuple )
    {
    }
    bool Interface_RequirementTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_RequirementTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_RequirementTrait->p_Clang_Interface_RequirementTrait = data::Ptr< data::Clang::Interface_RequirementTrait >( p_Tree_Interface_RequirementTrait, this );
    }
    void Interface_RequirementTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_RequirementTrait );
        loader.load( tuple );
    }
    void Interface_RequirementTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_RequirementTrait );
        storer.store( tuple );
    }
    void Interface_RequirementTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_RequirementTrait" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "tuple", tuple } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_TransitionTypeTrait : public mega::io::Object
    Interface_TransitionTypeTrait::Interface_TransitionTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_TransitionTypeTrait( loader )
    {
    }
    Interface_TransitionTypeTrait::Interface_TransitionTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_TransitionTypeTrait > p_Tree_Interface_TransitionTypeTrait, const std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& tuple)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_TransitionTypeTrait( p_Tree_Interface_TransitionTypeTrait )
          , tuple( tuple )
    {
    }
    bool Interface_TransitionTypeTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_TransitionTypeTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_TransitionTypeTrait->p_Clang_Interface_TransitionTypeTrait = data::Ptr< data::Clang::Interface_TransitionTypeTrait >( p_Tree_Interface_TransitionTypeTrait, this );
    }
    void Interface_TransitionTypeTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_TransitionTypeTrait );
        loader.load( tuple );
    }
    void Interface_TransitionTypeTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_TransitionTypeTrait );
        storer.store( tuple );
    }
    void Interface_TransitionTypeTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_TransitionTypeTrait" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "tuple", tuple } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_EventTypeTrait : public mega::io::Object
    Interface_EventTypeTrait::Interface_EventTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_EventTypeTrait( loader )
    {
    }
    Interface_EventTypeTrait::Interface_EventTypeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_EventTypeTrait > p_Tree_Interface_EventTypeTrait, const std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& tuple)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_EventTypeTrait( p_Tree_Interface_EventTypeTrait )
          , tuple( tuple )
    {
    }
    bool Interface_EventTypeTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_EventTypeTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_EventTypeTrait->p_Clang_Interface_EventTypeTrait = data::Ptr< data::Clang::Interface_EventTypeTrait >( p_Tree_Interface_EventTypeTrait, this );
    }
    void Interface_EventTypeTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_EventTypeTrait );
        loader.load( tuple );
    }
    void Interface_EventTypeTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_EventTypeTrait );
        storer.store( tuple );
    }
    void Interface_EventTypeTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_EventTypeTrait" },
                { "filetype" , "Clang" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "tuple", tuple } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_SizeTrait : public mega::io::Object
    Interface_SizeTrait::Interface_SizeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_SizeTrait( loader )
    {
    }
    Interface_SizeTrait::Interface_SizeTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_SizeTrait > p_Tree_Interface_SizeTrait, const mega::U64& size)
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
    void Interface_SizeTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace Concrete
{
    // struct Concrete_Graph_Vertex : public mega::io::Object
    Concrete_Graph_Vertex::Concrete_Graph_Vertex( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Graph_Vertex >( loader, this ) )          , p_PerSourceModel_Concrete_Graph_Vertex( loader )
    {
    }
    bool Concrete_Graph_Vertex::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Graph_Vertex >( loader, const_cast< Concrete_Graph_Vertex* >( this ) ) };
    }
    void Concrete_Graph_Vertex::set_inheritance_pointer()
    {
    }
    void Concrete_Graph_Vertex::load( mega::io::Loader& loader )
    {
    }
    void Concrete_Graph_Vertex::store( mega::io::Storer& storer ) const
    {
    }
    void Concrete_Graph_Vertex::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Graph_Vertex" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Concrete_Dimensions_User : public mega::io::Object
    Concrete_Dimensions_User::Concrete_Dimensions_User( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_User >( loader, this ) )          , p_Concrete_Concrete_Graph_Vertex( loader )
          , p_PerSourceConcreteTable_Concrete_Dimensions_User( loader )
          , p_MemoryLayout_Concrete_Dimensions_User( loader )
          , parent_context( loader )
          , interface_dimension( loader )
    {
    }
    Concrete_Dimensions_User::Concrete_Dimensions_User( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Context >& parent_context, const data::Ptr< data::Tree::Interface_DimensionTrait >& interface_dimension)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_User >( loader, this ) )          , p_Concrete_Concrete_Graph_Vertex( loader )
          , p_PerSourceConcreteTable_Concrete_Dimensions_User( loader )
          , p_MemoryLayout_Concrete_Dimensions_User( loader )
          , parent_context( parent_context )
          , interface_dimension( interface_dimension )
    {
    }
    bool Concrete_Dimensions_User::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Dimensions_User >( loader, const_cast< Concrete_Dimensions_User* >( this ) ) };
    }
    void Concrete_Dimensions_User::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Graph_Vertex->m_inheritance = data::Ptr< data::Concrete::Concrete_Dimensions_User >( p_Concrete_Concrete_Graph_Vertex, this );
    }
    void Concrete_Dimensions_User::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Graph_Vertex );
        loader.load( parent_context );
        loader.load( interface_dimension );
    }
    void Concrete_Dimensions_User::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Graph_Vertex );
        storer.store( parent_context );
        storer.store( interface_dimension );
    }
    void Concrete_Dimensions_User::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimensions_User" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent_context", parent_context } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_dimension", interface_dimension } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Dimensions_Link : public mega::io::Object
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_Link >( loader, this ) )          , p_Concrete_Concrete_Graph_Vertex( loader )
          , p_PerSourceConcreteTable_Concrete_Dimensions_Link( loader )
          , p_MemoryLayout_Concrete_Dimensions_Link( loader )
          , parent_context( loader )
    {
    }
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Context >& parent_context)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_Link >( loader, this ) )          , p_Concrete_Concrete_Graph_Vertex( loader )
          , p_PerSourceConcreteTable_Concrete_Dimensions_Link( loader )
          , p_MemoryLayout_Concrete_Dimensions_Link( loader )
          , parent_context( parent_context )
    {
    }
    bool Concrete_Dimensions_Link::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Dimensions_Link >( loader, const_cast< Concrete_Dimensions_Link* >( this ) ) };
    }
    void Concrete_Dimensions_Link::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Graph_Vertex->m_inheritance = data::Ptr< data::Concrete::Concrete_Dimensions_Link >( p_Concrete_Concrete_Graph_Vertex, this );
    }
    void Concrete_Dimensions_Link::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Graph_Vertex );
        loader.load( parent_context );
    }
    void Concrete_Dimensions_Link::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Graph_Vertex );
        storer.store( parent_context );
    }
    void Concrete_Dimensions_Link::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimensions_Link" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent_context", parent_context } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Dimensions_UserLink : public mega::io::Object
    Concrete_Dimensions_UserLink::Concrete_Dimensions_UserLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_UserLink >( loader, this ) )          , p_Concrete_Concrete_Dimensions_Link( loader )
          , interface_link( loader )
    {
    }
    Concrete_Dimensions_UserLink::Concrete_Dimensions_UserLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Clang::Interface_TypedLinkTrait >& interface_link)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_UserLink >( loader, this ) )          , p_Concrete_Concrete_Dimensions_Link( loader )
          , interface_link( interface_link )
    {
    }
    bool Concrete_Dimensions_UserLink::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Dimensions_UserLink >( loader, const_cast< Concrete_Dimensions_UserLink* >( this ) ) };
    }
    void Concrete_Dimensions_UserLink::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_Link->m_inheritance = data::Ptr< data::Concrete::Concrete_Dimensions_UserLink >( p_Concrete_Concrete_Dimensions_Link, this );
    }
    void Concrete_Dimensions_UserLink::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_Link );
        loader.load( interface_link );
    }
    void Concrete_Dimensions_UserLink::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Link );
        storer.store( interface_link );
    }
    void Concrete_Dimensions_UserLink::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimensions_UserLink" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_link", interface_link } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Dimensions_OwnershipLink : public mega::io::Object
    Concrete_Dimensions_OwnershipLink::Concrete_Dimensions_OwnershipLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink >( loader, this ) )          , p_Concrete_Concrete_Dimensions_Link( loader )
    {
    }
    bool Concrete_Dimensions_OwnershipLink::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink >( loader, const_cast< Concrete_Dimensions_OwnershipLink* >( this ) ) };
    }
    void Concrete_Dimensions_OwnershipLink::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_Link->m_inheritance = data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink >( p_Concrete_Concrete_Dimensions_Link, this );
    }
    void Concrete_Dimensions_OwnershipLink::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_Link );
    }
    void Concrete_Dimensions_OwnershipLink::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Link );
    }
    void Concrete_Dimensions_OwnershipLink::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimensions_OwnershipLink" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Concrete_ContextGroup : public mega::io::Object
    Concrete_ContextGroup::Concrete_ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_ContextGroup >( loader, this ) )          , p_Concrete_Concrete_Graph_Vertex( loader )
    {
    }
    Concrete_ContextGroup::Concrete_ContextGroup( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Concrete::Concrete_Context > >& children)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_ContextGroup >( loader, this ) )          , p_Concrete_Concrete_Graph_Vertex( loader )
          , children( children )
    {
    }
    bool Concrete_ContextGroup::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_ContextGroup >( loader, const_cast< Concrete_ContextGroup* >( this ) ) };
    }
    void Concrete_ContextGroup::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Graph_Vertex->m_inheritance = data::Ptr< data::Concrete::Concrete_ContextGroup >( p_Concrete_Concrete_Graph_Vertex, this );
    }
    void Concrete_ContextGroup::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Graph_Vertex );
        loader.load( children );
    }
    void Concrete_ContextGroup::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Graph_Vertex );
        storer.store( children );
    }
    void Concrete_ContextGroup::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Context : public mega::io::Object
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Context >( loader, this ) )          , p_Concrete_Concrete_ContextGroup( loader )
          , p_PerSourceConcreteTable_Concrete_Context( loader )
          , p_MemoryLayout_Concrete_Context( loader )
          , component( loader )
          , parent( loader )
          , interface( loader )
    {
    }
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Components::Components_Component >& component, const data::Ptr< data::Concrete::Concrete_ContextGroup >& parent, const data::Ptr< data::Tree::Interface_IContext >& interface, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& inheritance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Context >( loader, this ) )          , p_Concrete_Concrete_ContextGroup( loader )
          , p_PerSourceConcreteTable_Concrete_Context( loader )
          , p_MemoryLayout_Concrete_Context( loader )
          , component( component )
          , parent( parent )
          , interface( interface )
          , inheritance( inheritance )
    {
    }
    bool Concrete_Context::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Context >( loader, const_cast< Concrete_Context* >( this ) ) };
    }
    void Concrete_Context::set_inheritance_pointer()
    {
        p_Concrete_Concrete_ContextGroup->m_inheritance = data::Ptr< data::Concrete::Concrete_Context >( p_Concrete_Concrete_ContextGroup, this );
    }
    void Concrete_Context::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_ContextGroup );
        loader.load( component );
        loader.load( concrete_object );
        loader.load( parent );
        loader.load( interface );
        loader.load( inheritance );
    }
    void Concrete_Context::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_ContextGroup );
        storer.store( component );
        VERIFY_RTE_MSG( concrete_object.has_value(), "Concrete::Concrete_Context.concrete_object has NOT been set" );
        storer.store( concrete_object );
        storer.store( parent );
        storer.store( interface );
        storer.store( inheritance );
    }
    void Concrete_Context::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "component", component } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete_object", concrete_object.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent", parent } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface", interface } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance", inheritance } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Interupt : public mega::io::Object
    Concrete_Interupt::Concrete_Interupt( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Interupt >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_interupt( loader )
    {
    }
    Concrete_Interupt::Concrete_Interupt( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Interupt >& interface_interupt)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Interupt >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , interface_interupt( interface_interupt )
    {
    }
    bool Concrete_Interupt::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Interupt >( loader, const_cast< Concrete_Interupt* >( this ) ) };
    }
    void Concrete_Interupt::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Concrete_Interupt >( p_Concrete_Concrete_Context, this );
    }
    void Concrete_Interupt::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Context );
        loader.load( interface_interupt );
    }
    void Concrete_Interupt::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( interface_interupt );
    }
    void Concrete_Interupt::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Interupt" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_interupt", interface_interupt } } );
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Function >( loader, const_cast< Concrete_Function* >( this ) ) };
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
    void Concrete_Function::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_UserDimensionContext : public mega::io::Object
    Concrete_UserDimensionContext::Concrete_UserDimensionContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_UserDimensionContext >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
    {
    }
    Concrete_UserDimensionContext::Concrete_UserDimensionContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& dimensions, const std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& links)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_UserDimensionContext >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , dimensions( dimensions )
          , links( links )
    {
    }
    bool Concrete_UserDimensionContext::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_UserDimensionContext >( loader, const_cast< Concrete_UserDimensionContext* >( this ) ) };
    }
    void Concrete_UserDimensionContext::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Context->m_inheritance = data::Ptr< data::Concrete::Concrete_UserDimensionContext >( p_Concrete_Concrete_Context, this );
    }
    void Concrete_UserDimensionContext::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Context );
        loader.load( dimensions );
        loader.load( links );
    }
    void Concrete_UserDimensionContext::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( dimensions );
        storer.store( links );
    }
    void Concrete_UserDimensionContext::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_UserDimensionContext" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimensions", dimensions } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "links", links } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Namespace : public mega::io::Object
    Concrete_Namespace::Concrete_Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Namespace >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , interface_namespace( loader )
    {
    }
    Concrete_Namespace::Concrete_Namespace( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Namespace >& interface_namespace)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Namespace >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , interface_namespace( interface_namespace )
    {
    }
    bool Concrete_Namespace::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Namespace >( loader, const_cast< Concrete_Namespace* >( this ) ) };
    }
    void Concrete_Namespace::set_inheritance_pointer()
    {
        p_Concrete_Concrete_UserDimensionContext->m_inheritance = data::Ptr< data::Concrete::Concrete_Namespace >( p_Concrete_Concrete_UserDimensionContext, this );
    }
    void Concrete_Namespace::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_UserDimensionContext );
        loader.load( interface_namespace );
    }
    void Concrete_Namespace::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_UserDimensionContext );
        storer.store( interface_namespace );
    }
    void Concrete_Namespace::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_State : public mega::io::Object
    Concrete_State::Concrete_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_State >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , p_MemoryLayout_Concrete_State( loader )
          , interface_state( loader )
    {
    }
    Concrete_State::Concrete_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_State >& interface_state)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_State >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , p_MemoryLayout_Concrete_State( loader )
          , interface_state( interface_state )
    {
    }
    bool Concrete_State::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_State >( loader, const_cast< Concrete_State* >( this ) ) };
    }
    void Concrete_State::set_inheritance_pointer()
    {
        p_Concrete_Concrete_UserDimensionContext->m_inheritance = data::Ptr< data::Concrete::Concrete_State >( p_Concrete_Concrete_UserDimensionContext, this );
    }
    void Concrete_State::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_UserDimensionContext );
        loader.load( interface_state );
    }
    void Concrete_State::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_UserDimensionContext );
        storer.store( interface_state );
    }
    void Concrete_State::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_State" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_state", interface_state } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Action : public mega::io::Object
    Concrete_Action::Concrete_Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Action >( loader, this ) )          , p_Concrete_Concrete_State( loader )
          , interface_action( loader )
    {
    }
    Concrete_Action::Concrete_Action( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Action >& interface_action)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Action >( loader, this ) )          , p_Concrete_Concrete_State( loader )
          , interface_action( interface_action )
    {
    }
    bool Concrete_Action::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Action >( loader, const_cast< Concrete_Action* >( this ) ) };
    }
    void Concrete_Action::set_inheritance_pointer()
    {
        p_Concrete_Concrete_State->m_inheritance = data::Ptr< data::Concrete::Concrete_Action >( p_Concrete_Concrete_State, this );
    }
    void Concrete_Action::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_State );
        loader.load( interface_action );
    }
    void Concrete_Action::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_State );
        storer.store( interface_action );
    }
    void Concrete_Action::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Component : public mega::io::Object
    Concrete_Component::Concrete_Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Component >( loader, this ) )          , p_Concrete_Concrete_State( loader )
          , interface_component( loader )
    {
    }
    Concrete_Component::Concrete_Component( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Component >& interface_component)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Component >( loader, this ) )          , p_Concrete_Concrete_State( loader )
          , interface_component( interface_component )
    {
    }
    bool Concrete_Component::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Component >( loader, const_cast< Concrete_Component* >( this ) ) };
    }
    void Concrete_Component::set_inheritance_pointer()
    {
        p_Concrete_Concrete_State->m_inheritance = data::Ptr< data::Concrete::Concrete_Component >( p_Concrete_Concrete_State, this );
    }
    void Concrete_Component::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_State );
        loader.load( interface_component );
    }
    void Concrete_Component::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_State );
        storer.store( interface_component );
    }
    void Concrete_Component::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Component" },
                { "filetype" , "Concrete" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface_component", interface_component } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Event : public mega::io::Object
    Concrete_Event::Concrete_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Event >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , p_MemoryLayout_Concrete_Event( loader )
          , interface_event( loader )
    {
    }
    Concrete_Event::Concrete_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Event >& interface_event)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Event >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , p_MemoryLayout_Concrete_Event( loader )
          , interface_event( interface_event )
    {
    }
    bool Concrete_Event::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Event >( loader, const_cast< Concrete_Event* >( this ) ) };
    }
    void Concrete_Event::set_inheritance_pointer()
    {
        p_Concrete_Concrete_UserDimensionContext->m_inheritance = data::Ptr< data::Concrete::Concrete_Event >( p_Concrete_Concrete_UserDimensionContext, this );
    }
    void Concrete_Event::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_UserDimensionContext );
        loader.load( interface_event );
    }
    void Concrete_Event::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_UserDimensionContext );
        storer.store( interface_event );
    }
    void Concrete_Event::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Object : public mega::io::Object
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Object >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , p_MemoryLayout_Concrete_Object( loader )
          , p_PerSourceModel_Concrete_Object( loader )
          , interface_object( loader )
          , ownership_link( loader )
    {
    }
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Object >& interface_object)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Object >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , p_MemoryLayout_Concrete_Object( loader )
          , p_PerSourceModel_Concrete_Object( loader )
          , interface_object( interface_object )
    {
    }
    bool Concrete_Object::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Object >( loader, const_cast< Concrete_Object* >( this ) ) };
    }
    void Concrete_Object::set_inheritance_pointer()
    {
        p_Concrete_Concrete_UserDimensionContext->m_inheritance = data::Ptr< data::Concrete::Concrete_Object >( p_Concrete_Concrete_UserDimensionContext, this );
    }
    void Concrete_Object::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_UserDimensionContext );
        loader.load( interface_object );
        loader.load( ownership_link );
    }
    void Concrete_Object::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_UserDimensionContext );
        storer.store( interface_object );
        VERIFY_RTE_MSG( ownership_link.has_value(), "Concrete::Concrete_Object.ownership_link has NOT been set" );
        storer.store( ownership_link );
    }
    void Concrete_Object::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "ownership_link", ownership_link.value() } } );
            _part__[ "properties" ].push_back( property );
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
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Root >( loader, const_cast< Concrete_Root* >( this ) ) };
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
    void Concrete_Root::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace Derivations
{
    // struct Inheritance_ObjectMapping : public mega::io::Object
    Inheritance_ObjectMapping::Inheritance_ObjectMapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Derivations::Inheritance_ObjectMapping >( loader, this ) )    {
    }
    Inheritance_ObjectMapping::Inheritance_ObjectMapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::io::megaFilePath& source_file, const mega::U64& hash_code, const std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Concrete::Concrete_Context > >& inheritance_contexts, const std::multimap< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_User > >& inheritance_dimensions, const std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& inheritance_links)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Derivations::Inheritance_ObjectMapping >( loader, this ) )          , source_file( source_file )
          , hash_code( hash_code )
          , inheritance_contexts( inheritance_contexts )
          , inheritance_dimensions( inheritance_dimensions )
          , inheritance_links( inheritance_links )
    {
    }
    bool Inheritance_ObjectMapping::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Derivations::Inheritance_ObjectMapping >( loader, const_cast< Inheritance_ObjectMapping* >( this ) ) };
    }
    void Inheritance_ObjectMapping::set_inheritance_pointer()
    {
    }
    void Inheritance_ObjectMapping::load( mega::io::Loader& loader )
    {
        loader.load( source_file );
        loader.load( hash_code );
        loader.load( inheritance_contexts );
        loader.load( inheritance_dimensions );
        loader.load( inheritance_links );
    }
    void Inheritance_ObjectMapping::store( mega::io::Storer& storer ) const
    {
        storer.store( source_file );
        storer.store( hash_code );
        storer.store( inheritance_contexts );
        storer.store( inheritance_dimensions );
        storer.store( inheritance_links );
    }
    void Inheritance_ObjectMapping::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Inheritance_ObjectMapping" },
                { "filetype" , "Derivations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_file", source_file } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "hash_code", hash_code } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_contexts", inheritance_contexts } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_dimensions", inheritance_dimensions } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_links", inheritance_links } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Inheritance_Mapping : public mega::io::Object
    Inheritance_Mapping::Inheritance_Mapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Derivations::Inheritance_Mapping >( loader, this ) )    {
    }
    Inheritance_Mapping::Inheritance_Mapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Derivations::Inheritance_ObjectMapping > >& mappings, const std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Concrete::Concrete_Context > >& inheritance_contexts, const std::multimap< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_User > >& inheritance_dimensions, const std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& inheritance_links)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Derivations::Inheritance_Mapping >( loader, this ) )          , mappings( mappings )
          , inheritance_contexts( inheritance_contexts )
          , inheritance_dimensions( inheritance_dimensions )
          , inheritance_links( inheritance_links )
    {
    }
    bool Inheritance_Mapping::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Derivations::Inheritance_Mapping >( loader, const_cast< Inheritance_Mapping* >( this ) ) };
    }
    void Inheritance_Mapping::set_inheritance_pointer()
    {
    }
    void Inheritance_Mapping::load( mega::io::Loader& loader )
    {
        loader.load( mappings );
        loader.load( inheritance_contexts );
        loader.load( inheritance_dimensions );
        loader.load( inheritance_links );
    }
    void Inheritance_Mapping::store( mega::io::Storer& storer ) const
    {
        storer.store( mappings );
        storer.store( inheritance_contexts );
        storer.store( inheritance_dimensions );
        storer.store( inheritance_links );
    }
    void Inheritance_Mapping::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Inheritance_Mapping" },
                { "filetype" , "Derivations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "mappings", mappings } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_contexts", inheritance_contexts } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_dimensions", inheritance_dimensions } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "inheritance_links", inheritance_links } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace PerSourceDerivations
{
    // struct Interface_DimensionTrait : public mega::io::Object
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_DimensionTrait( loader )
    {
    }
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_DimensionTrait > p_Tree_Interface_DimensionTrait, const std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& concrete)
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
        p_Tree_Interface_DimensionTrait->p_PerSourceDerivations_Interface_DimensionTrait = data::Ptr< data::PerSourceDerivations::Interface_DimensionTrait >( p_Tree_Interface_DimensionTrait, this );
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
    void Interface_DimensionTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_DimensionTrait" },
                { "filetype" , "PerSourceDerivations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete", concrete } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_LinkTrait : public mega::io::Object
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_LinkTrait( loader )
    {
    }
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_LinkTrait > p_Tree_Interface_LinkTrait, const std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& concrete)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_LinkTrait( p_Tree_Interface_LinkTrait )
          , concrete( concrete )
    {
    }
    bool Interface_LinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_LinkTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_LinkTrait->p_PerSourceDerivations_Interface_LinkTrait = data::Ptr< data::PerSourceDerivations::Interface_LinkTrait >( p_Tree_Interface_LinkTrait, this );
    }
    void Interface_LinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_LinkTrait );
        loader.load( concrete );
    }
    void Interface_LinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_LinkTrait );
        storer.store( concrete );
    }
    void Interface_LinkTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_LinkTrait" },
                { "filetype" , "PerSourceDerivations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete", concrete } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_IContext : public mega::io::Object
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_IContext( loader )
    {
    }
    Interface_IContext::Interface_IContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_IContext > p_Tree_Interface_IContext, const std::vector< data::Ptr< data::Concrete::Concrete_Context > >& concrete)
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
        p_Tree_Interface_IContext->p_PerSourceDerivations_Interface_IContext = data::Ptr< data::PerSourceDerivations::Interface_IContext >( p_Tree_Interface_IContext, this );
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
    void Interface_IContext::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_IContext" },
                { "filetype" , "PerSourceDerivations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete", concrete } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace Model
{
    // struct Concrete_Graph_Edge : public mega::io::Object
    Concrete_Graph_Edge::Concrete_Graph_Edge( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::Concrete_Graph_Edge >( loader, this ) )          , source( loader )
          , target( loader )
    {
    }
    Concrete_Graph_Edge::Concrete_Graph_Edge( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::EdgeType& type, const data::Ptr< data::Concrete::Concrete_Graph_Vertex >& source, const data::Ptr< data::Concrete::Concrete_Graph_Vertex >& target)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::Concrete_Graph_Edge >( loader, this ) )          , type( type )
          , source( source )
          , target( target )
    {
    }
    bool Concrete_Graph_Edge::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Model::Concrete_Graph_Edge >( loader, const_cast< Concrete_Graph_Edge* >( this ) ) };
    }
    void Concrete_Graph_Edge::set_inheritance_pointer()
    {
    }
    void Concrete_Graph_Edge::load( mega::io::Loader& loader )
    {
        loader.load( type );
        loader.load( source );
        loader.load( target );
    }
    void Concrete_Graph_Edge::store( mega::io::Storer& storer ) const
    {
        storer.store( type );
        storer.store( source );
        storer.store( target );
    }
    void Concrete_Graph_Edge::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Graph_Edge" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "type", type } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "source", source } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "target", target } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct HyperGraph_Relation : public mega::io::Object
    HyperGraph_Relation::HyperGraph_Relation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_Relation >( loader, this ) )    {
    }
    HyperGraph_Relation::HyperGraph_Relation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::RelationID& id)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_Relation >( loader, this ) )          , id( id )
    {
    }
    bool HyperGraph_Relation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Model::HyperGraph_Relation >( loader, const_cast< HyperGraph_Relation* >( this ) ) };
    }
    void HyperGraph_Relation::set_inheritance_pointer()
    {
    }
    void HyperGraph_Relation::load( mega::io::Loader& loader )
    {
        loader.load( id );
    }
    void HyperGraph_Relation::store( mega::io::Storer& storer ) const
    {
        storer.store( id );
    }
    void HyperGraph_Relation::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "HyperGraph_Relation" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct HyperGraph_ObjectRelation : public mega::io::Object
    HyperGraph_ObjectRelation::HyperGraph_ObjectRelation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_ObjectRelation >( loader, this ) )          , p_Model_HyperGraph_Relation( loader )
    {
    }
    bool HyperGraph_ObjectRelation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Model::HyperGraph_ObjectRelation >( loader, const_cast< HyperGraph_ObjectRelation* >( this ) ) };
    }
    void HyperGraph_ObjectRelation::set_inheritance_pointer()
    {
        p_Model_HyperGraph_Relation->m_inheritance = data::Ptr< data::Model::HyperGraph_ObjectRelation >( p_Model_HyperGraph_Relation, this );
    }
    void HyperGraph_ObjectRelation::load( mega::io::Loader& loader )
    {
        loader.load( p_Model_HyperGraph_Relation );
    }
    void HyperGraph_ObjectRelation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Model_HyperGraph_Relation );
    }
    void HyperGraph_ObjectRelation::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "HyperGraph_ObjectRelation" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct HyperGraph_OwningObjectRelation : public mega::io::Object
    HyperGraph_OwningObjectRelation::HyperGraph_OwningObjectRelation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_OwningObjectRelation >( loader, this ) )          , p_Model_HyperGraph_ObjectRelation( loader )
          , owned( loader )
    {
    }
    HyperGraph_OwningObjectRelation::HyperGraph_OwningObjectRelation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Clang::Interface_ObjectLinkTrait > >& owners, const data::Ptr< data::Tree::Interface_IContext >& owned)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_OwningObjectRelation >( loader, this ) )          , p_Model_HyperGraph_ObjectRelation( loader )
          , owners( owners )
          , owned( owned )
    {
    }
    bool HyperGraph_OwningObjectRelation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Model::HyperGraph_OwningObjectRelation >( loader, const_cast< HyperGraph_OwningObjectRelation* >( this ) ) };
    }
    void HyperGraph_OwningObjectRelation::set_inheritance_pointer()
    {
        p_Model_HyperGraph_ObjectRelation->m_inheritance = data::Ptr< data::Model::HyperGraph_OwningObjectRelation >( p_Model_HyperGraph_ObjectRelation, this );
    }
    void HyperGraph_OwningObjectRelation::load( mega::io::Loader& loader )
    {
        loader.load( p_Model_HyperGraph_ObjectRelation );
        loader.load( owners );
        loader.load( owned );
    }
    void HyperGraph_OwningObjectRelation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Model_HyperGraph_ObjectRelation );
        storer.store( owners );
        storer.store( owned );
    }
    void HyperGraph_OwningObjectRelation::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "HyperGraph_OwningObjectRelation" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "owners", owners } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "owned", owned } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct HyperGraph_NonOwningObjectRelation : public mega::io::Object
    HyperGraph_NonOwningObjectRelation::HyperGraph_NonOwningObjectRelation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_NonOwningObjectRelation >( loader, this ) )          , p_Model_HyperGraph_ObjectRelation( loader )
          , source( loader )
          , target( loader )
    {
    }
    HyperGraph_NonOwningObjectRelation::HyperGraph_NonOwningObjectRelation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Clang::Interface_ObjectLinkTrait >& source, const data::Ptr< data::Clang::Interface_ObjectLinkTrait >& target)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_NonOwningObjectRelation >( loader, this ) )          , p_Model_HyperGraph_ObjectRelation( loader )
          , source( source )
          , target( target )
    {
    }
    bool HyperGraph_NonOwningObjectRelation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Model::HyperGraph_NonOwningObjectRelation >( loader, const_cast< HyperGraph_NonOwningObjectRelation* >( this ) ) };
    }
    void HyperGraph_NonOwningObjectRelation::set_inheritance_pointer()
    {
        p_Model_HyperGraph_ObjectRelation->m_inheritance = data::Ptr< data::Model::HyperGraph_NonOwningObjectRelation >( p_Model_HyperGraph_ObjectRelation, this );
    }
    void HyperGraph_NonOwningObjectRelation::load( mega::io::Loader& loader )
    {
        loader.load( p_Model_HyperGraph_ObjectRelation );
        loader.load( source );
        loader.load( target );
    }
    void HyperGraph_NonOwningObjectRelation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Model_HyperGraph_ObjectRelation );
        storer.store( source );
        storer.store( target );
    }
    void HyperGraph_NonOwningObjectRelation::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "HyperGraph_NonOwningObjectRelation" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "source", source } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "target", target } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct HyperGraph_Graph : public mega::io::Object
    HyperGraph_Graph::HyperGraph_Graph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_Graph >( loader, this ) )    {
    }
    HyperGraph_Graph::HyperGraph_Graph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::map< data::Ptr< data::Clang::Interface_ObjectLinkTrait >, data::Ptr< data::Model::HyperGraph_Relation > >& relations)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_Graph >( loader, this ) )          , relations( relations )
    {
    }
    bool HyperGraph_Graph::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Model::HyperGraph_Graph >( loader, const_cast< HyperGraph_Graph* >( this ) ) };
    }
    void HyperGraph_Graph::set_inheritance_pointer()
    {
    }
    void HyperGraph_Graph::load( mega::io::Loader& loader )
    {
        loader.load( relations );
    }
    void HyperGraph_Graph::store( mega::io::Storer& storer ) const
    {
        storer.store( relations );
    }
    void HyperGraph_Graph::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "relations", relations } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace PerSourceModel
{
    // struct Interface_ObjectLinkTrait : public mega::io::Object
    Interface_ObjectLinkTrait::Interface_ObjectLinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Clang_Interface_ObjectLinkTrait( loader )
          , relation( loader )
    {
    }
    Interface_ObjectLinkTrait::Interface_ObjectLinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Clang::Interface_ObjectLinkTrait > p_Clang_Interface_ObjectLinkTrait, const data::Ptr< data::Model::HyperGraph_Relation >& relation)
        :   mega::io::Object( objectInfo )          , p_Clang_Interface_ObjectLinkTrait( p_Clang_Interface_ObjectLinkTrait )
          , relation( relation )
    {
    }
    bool Interface_ObjectLinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_ObjectLinkTrait::set_inheritance_pointer()
    {
        p_Clang_Interface_ObjectLinkTrait->p_PerSourceModel_Interface_ObjectLinkTrait = data::Ptr< data::PerSourceModel::Interface_ObjectLinkTrait >( p_Clang_Interface_ObjectLinkTrait, this );
    }
    void Interface_ObjectLinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Clang_Interface_ObjectLinkTrait );
        loader.load( relation );
    }
    void Interface_ObjectLinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Clang_Interface_ObjectLinkTrait );
        storer.store( relation );
    }
    void Interface_ObjectLinkTrait::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_ObjectLinkTrait" },
                { "filetype" , "PerSourceModel" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "relation", relation } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Graph_Vertex : public mega::io::Object
    Concrete_Graph_Vertex::Concrete_Graph_Vertex( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Graph_Vertex( loader )
    {
    }
    Concrete_Graph_Vertex::Concrete_Graph_Vertex( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Graph_Vertex > p_Concrete_Concrete_Graph_Vertex, const std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& out_edges, const std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& in_edges)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Graph_Vertex( p_Concrete_Concrete_Graph_Vertex )
          , out_edges( out_edges )
          , in_edges( in_edges )
    {
    }
    bool Concrete_Graph_Vertex::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Graph_Vertex::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Graph_Vertex->p_PerSourceModel_Concrete_Graph_Vertex = data::Ptr< data::PerSourceModel::Concrete_Graph_Vertex >( p_Concrete_Concrete_Graph_Vertex, this );
    }
    void Concrete_Graph_Vertex::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Graph_Vertex );
        loader.load( out_edges );
        loader.load( in_edges );
    }
    void Concrete_Graph_Vertex::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Graph_Vertex );
        storer.store( out_edges );
        storer.store( in_edges );
    }
    void Concrete_Graph_Vertex::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Graph_Vertex" },
                { "filetype" , "PerSourceModel" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "out_edges", out_edges } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "in_edges", in_edges } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Object : public mega::io::Object
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Object( loader )
    {
    }
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Object > p_Concrete_Concrete_Object, const std::vector< data::Ptr< data::Concrete::Concrete_UserDimensionContext > >& link_contexts)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Object( p_Concrete_Concrete_Object )
          , link_contexts( link_contexts )
    {
    }
    bool Concrete_Object::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Object::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Object->p_PerSourceModel_Concrete_Object = data::Ptr< data::PerSourceModel::Concrete_Object >( p_Concrete_Concrete_Object, this );
    }
    void Concrete_Object::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Object );
        loader.load( link_contexts );
    }
    void Concrete_Object::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Object );
        storer.store( link_contexts );
    }
    void Concrete_Object::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Object" },
                { "filetype" , "PerSourceModel" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "link_contexts", link_contexts } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace Model
{
    // struct Alias_Edge : public mega::io::Object
    Alias_Edge::Alias_Edge( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::Alias_Edge >( loader, this ) )          , next( loader )
    {
    }
    Alias_Edge::Alias_Edge( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Model::Alias_Step >& next, const std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& edges)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::Alias_Edge >( loader, this ) )          , next( next )
          , edges( edges )
    {
    }
    bool Alias_Edge::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Model::Alias_Edge >( loader, const_cast< Alias_Edge* >( this ) ) };
    }
    void Alias_Edge::set_inheritance_pointer()
    {
    }
    void Alias_Edge::load( mega::io::Loader& loader )
    {
        loader.load( next );
        loader.load( edges );
    }
    void Alias_Edge::store( mega::io::Storer& storer ) const
    {
        storer.store( next );
        storer.store( edges );
    }
    void Alias_Edge::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Alias_Edge" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "next", next } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "edges", edges } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Alias_Step : public mega::io::Object
    Alias_Step::Alias_Step( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::Alias_Step >( loader, this ) )          , vertex( loader )
    {
    }
    Alias_Step::Alias_Step( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Graph_Vertex >& vertex, const std::vector< data::Ptr< data::Model::Alias_Edge > >& edges)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::Alias_Step >( loader, this ) )          , vertex( vertex )
          , edges( edges )
    {
    }
    bool Alias_Step::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Model::Alias_Step >( loader, const_cast< Alias_Step* >( this ) ) };
    }
    void Alias_Step::set_inheritance_pointer()
    {
    }
    void Alias_Step::load( mega::io::Loader& loader )
    {
        loader.load( vertex );
        loader.load( edges );
    }
    void Alias_Step::store( mega::io::Storer& storer ) const
    {
        storer.store( vertex );
        storer.store( edges );
    }
    void Alias_Step::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Alias_Step" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "vertex", vertex } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "edges", edges } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Alias_And : public mega::io::Object
    Alias_And::Alias_And( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::Alias_And >( loader, this ) )          , p_Model_Alias_Step( loader )
    {
    }
    bool Alias_And::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Model::Alias_And >( loader, const_cast< Alias_And* >( this ) ) };
    }
    void Alias_And::set_inheritance_pointer()
    {
        p_Model_Alias_Step->m_inheritance = data::Ptr< data::Model::Alias_And >( p_Model_Alias_Step, this );
    }
    void Alias_And::load( mega::io::Loader& loader )
    {
        loader.load( p_Model_Alias_Step );
    }
    void Alias_And::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Model_Alias_Step );
    }
    void Alias_And::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Alias_And" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Alias_Or : public mega::io::Object
    Alias_Or::Alias_Or( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::Alias_Or >( loader, this ) )          , p_Model_Alias_Step( loader )
    {
    }
    bool Alias_Or::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Model::Alias_Or >( loader, const_cast< Alias_Or* >( this ) ) };
    }
    void Alias_Or::set_inheritance_pointer()
    {
        p_Model_Alias_Step->m_inheritance = data::Ptr< data::Model::Alias_Or >( p_Model_Alias_Step, this );
    }
    void Alias_Or::load( mega::io::Loader& loader )
    {
        loader.load( p_Model_Alias_Step );
    }
    void Alias_Or::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Model_Alias_Step );
    }
    void Alias_Or::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Alias_Or" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Alias_AliasDerivation : public mega::io::Object
    Alias_AliasDerivation::Alias_AliasDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::Alias_AliasDerivation >( loader, this ) )          , context( loader )
    {
    }
    Alias_AliasDerivation::Alias_AliasDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Graph_Vertex >& context, const std::vector< data::Ptr< data::Model::Alias_Edge > >& edges)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::Alias_AliasDerivation >( loader, this ) )          , context( context )
          , edges( edges )
    {
    }
    bool Alias_AliasDerivation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Model::Alias_AliasDerivation >( loader, const_cast< Alias_AliasDerivation* >( this ) ) };
    }
    void Alias_AliasDerivation::set_inheritance_pointer()
    {
    }
    void Alias_AliasDerivation::load( mega::io::Loader& loader )
    {
        loader.load( context );
        loader.load( edges );
    }
    void Alias_AliasDerivation::store( mega::io::Storer& storer ) const
    {
        storer.store( context );
        storer.store( edges );
    }
    void Alias_AliasDerivation::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Alias_AliasDerivation" },
                { "filetype" , "Model" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "context", context } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "edges", edges } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace PerSourceModel
{
}
namespace MemoryLayout
{
    // struct Concrete_Dimensions_User : public mega::io::Object
    Concrete_Dimensions_User::Concrete_Dimensions_User( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_User( loader )
          , part( loader )
    {
    }
    Concrete_Dimensions_User::Concrete_Dimensions_User( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Dimensions_User > p_Concrete_Concrete_Dimensions_User, const mega::U64& offset, const data::Ptr< data::MemoryLayout::MemoryLayout_Part >& part)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_User( p_Concrete_Concrete_Dimensions_User )
          , offset( offset )
          , part( part )
    {
    }
    bool Concrete_Dimensions_User::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Dimensions_User::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_User->p_MemoryLayout_Concrete_Dimensions_User = data::Ptr< data::MemoryLayout::Concrete_Dimensions_User >( p_Concrete_Concrete_Dimensions_User, this );
    }
    void Concrete_Dimensions_User::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_User );
        loader.load( offset );
        loader.load( part );
    }
    void Concrete_Dimensions_User::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_User );
        storer.store( offset );
        storer.store( part );
    }
    void Concrete_Dimensions_User::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimensions_User" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "offset", offset } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "part", part } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Dimensions_Link : public mega::io::Object
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Link( loader )
          , part( loader )
    {
    }
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Dimensions_Link > p_Concrete_Concrete_Dimensions_Link, const mega::U64& offset, const data::Ptr< data::MemoryLayout::MemoryLayout_Part >& part, const bool& singular)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Link( p_Concrete_Concrete_Dimensions_Link )
          , offset( offset )
          , part( part )
          , singular( singular )
    {
    }
    bool Concrete_Dimensions_Link::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Dimensions_Link::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_Link->p_MemoryLayout_Concrete_Dimensions_Link = data::Ptr< data::MemoryLayout::Concrete_Dimensions_Link >( p_Concrete_Concrete_Dimensions_Link, this );
    }
    void Concrete_Dimensions_Link::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_Link );
        loader.load( offset );
        loader.load( part );
        loader.load( singular );
    }
    void Concrete_Dimensions_Link::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Link );
        storer.store( offset );
        storer.store( part );
        storer.store( singular );
    }
    void Concrete_Dimensions_Link::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimensions_Link" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "offset", offset } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "part", part } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "singular", singular } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Dimensions_Allocation : public mega::io::Object
    Concrete_Dimensions_Allocation::Concrete_Dimensions_Allocation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocation >( loader, this ) )          , p_PerSourceConcreteTable_Concrete_Dimensions_Allocation( loader )
          , parent_context( loader )
          , part( loader )
    {
    }
    Concrete_Dimensions_Allocation::Concrete_Dimensions_Allocation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Context >& parent_context)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocation >( loader, this ) )          , p_PerSourceConcreteTable_Concrete_Dimensions_Allocation( loader )
          , parent_context( parent_context )
    {
    }
    bool Concrete_Dimensions_Allocation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocation >( loader, const_cast< Concrete_Dimensions_Allocation* >( this ) ) };
    }
    void Concrete_Dimensions_Allocation::set_inheritance_pointer()
    {
    }
    void Concrete_Dimensions_Allocation::load( mega::io::Loader& loader )
    {
        loader.load( parent_context );
        loader.load( part );
        loader.load( offset );
    }
    void Concrete_Dimensions_Allocation::store( mega::io::Storer& storer ) const
    {
        storer.store( parent_context );
        VERIFY_RTE_MSG( part.has_value(), "MemoryLayout::Concrete_Dimensions_Allocation.part has NOT been set" );
        storer.store( part );
        VERIFY_RTE_MSG( offset.has_value(), "MemoryLayout::Concrete_Dimensions_Allocation.offset has NOT been set" );
        storer.store( offset );
    }
    void Concrete_Dimensions_Allocation::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimensions_Allocation" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent_context", parent_context } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "part", part.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "offset", offset.value() } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Dimensions_Allocator : public mega::io::Object
    Concrete_Dimensions_Allocator::Concrete_Dimensions_Allocator( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocator >( loader, this ) )          , p_MemoryLayout_Concrete_Dimensions_Allocation( loader )
          , allocator( loader )
    {
    }
    Concrete_Dimensions_Allocator::Concrete_Dimensions_Allocator( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::MemoryLayout::Allocators_Allocator >& allocator)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocator >( loader, this ) )          , p_MemoryLayout_Concrete_Dimensions_Allocation( loader )
          , allocator( allocator )
    {
    }
    bool Concrete_Dimensions_Allocator::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocator >( loader, const_cast< Concrete_Dimensions_Allocator* >( this ) ) };
    }
    void Concrete_Dimensions_Allocator::set_inheritance_pointer()
    {
        p_MemoryLayout_Concrete_Dimensions_Allocation->m_inheritance = data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocator >( p_MemoryLayout_Concrete_Dimensions_Allocation, this );
    }
    void Concrete_Dimensions_Allocator::load( mega::io::Loader& loader )
    {
        loader.load( p_MemoryLayout_Concrete_Dimensions_Allocation );
        loader.load( allocator );
    }
    void Concrete_Dimensions_Allocator::store( mega::io::Storer& storer ) const
    {
        storer.store( p_MemoryLayout_Concrete_Dimensions_Allocation );
        storer.store( allocator );
    }
    void Concrete_Dimensions_Allocator::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimensions_Allocator" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "allocator", allocator } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Context : public mega::io::Object
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Context( loader )
          , allocator( loader )
    {
    }
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Context > p_Concrete_Concrete_Context, const data::Ptr< data::MemoryLayout::Allocators_Allocator >& allocator, const std::vector< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocator > >& allocation_dimensions)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Context( p_Concrete_Concrete_Context )
          , allocator( allocator )
          , allocation_dimensions( allocation_dimensions )
    {
    }
    bool Concrete_Context::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Context::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Context->p_MemoryLayout_Concrete_Context = data::Ptr< data::MemoryLayout::Concrete_Context >( p_Concrete_Concrete_Context, this );
    }
    void Concrete_Context::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Context );
        loader.load( allocator );
        loader.load( allocation_dimensions );
    }
    void Concrete_Context::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( allocator );
        storer.store( allocation_dimensions );
    }
    void Concrete_Context::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Context" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "allocator", allocator } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "allocation_dimensions", allocation_dimensions } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_State : public mega::io::Object
    Concrete_State::Concrete_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_State( loader )
    {
    }
    Concrete_State::Concrete_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_State > p_Concrete_Concrete_State, const mega::U64& local_size, const mega::U64& total_size)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_State( p_Concrete_Concrete_State )
          , local_size( local_size )
          , total_size( total_size )
    {
    }
    bool Concrete_State::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_State::set_inheritance_pointer()
    {
        p_Concrete_Concrete_State->p_MemoryLayout_Concrete_State = data::Ptr< data::MemoryLayout::Concrete_State >( p_Concrete_Concrete_State, this );
    }
    void Concrete_State::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_State );
        loader.load( local_size );
        loader.load( total_size );
    }
    void Concrete_State::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_State );
        storer.store( local_size );
        storer.store( total_size );
    }
    void Concrete_State::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_State" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "local_size", local_size } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "total_size", total_size } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Event : public mega::io::Object
    Concrete_Event::Concrete_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Event( loader )
    {
    }
    Concrete_Event::Concrete_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Event > p_Concrete_Concrete_Event, const mega::U64& local_size, const mega::U64& total_size)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Event( p_Concrete_Concrete_Event )
          , local_size( local_size )
          , total_size( total_size )
    {
    }
    bool Concrete_Event::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Event::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Event->p_MemoryLayout_Concrete_Event = data::Ptr< data::MemoryLayout::Concrete_Event >( p_Concrete_Concrete_Event, this );
    }
    void Concrete_Event::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Event );
        loader.load( local_size );
        loader.load( total_size );
    }
    void Concrete_Event::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Event );
        storer.store( local_size );
        storer.store( total_size );
    }
    void Concrete_Event::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Event" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "local_size", local_size } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "total_size", total_size } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Object : public mega::io::Object
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Object( loader )
    {
    }
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Object > p_Concrete_Concrete_Object, const std::vector< data::Ptr< data::MemoryLayout::MemoryLayout_Buffer > >& buffers)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Object( p_Concrete_Concrete_Object )
          , buffers( buffers )
    {
    }
    bool Concrete_Object::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Object::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Object->p_MemoryLayout_Concrete_Object = data::Ptr< data::MemoryLayout::Concrete_Object >( p_Concrete_Concrete_Object, this );
    }
    void Concrete_Object::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Object );
        loader.load( buffers );
    }
    void Concrete_Object::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Object );
        storer.store( buffers );
    }
    void Concrete_Object::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Object" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "buffers", buffers } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Allocators_Allocator : public mega::io::Object
    Allocators_Allocator::Allocators_Allocator( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Allocators_Allocator >( loader, this ) )          , allocated_context( loader )
    {
    }
    Allocators_Allocator::Allocators_Allocator( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::optional< data::Ptr< data::Concrete::Concrete_Context > >& parent_context, const data::Ptr< data::Concrete::Concrete_Context >& allocated_context)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Allocators_Allocator >( loader, this ) )          , parent_context( parent_context )
          , allocated_context( allocated_context )
    {
    }
    bool Allocators_Allocator::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::Allocators_Allocator >( loader, const_cast< Allocators_Allocator* >( this ) ) };
    }
    void Allocators_Allocator::set_inheritance_pointer()
    {
    }
    void Allocators_Allocator::load( mega::io::Loader& loader )
    {
        loader.load( parent_context );
        loader.load( allocated_context );
        loader.load( dimension );
    }
    void Allocators_Allocator::store( mega::io::Storer& storer ) const
    {
        storer.store( parent_context );
        storer.store( allocated_context );
        VERIFY_RTE_MSG( dimension.has_value(), "MemoryLayout::Allocators_Allocator.dimension has NOT been set" );
        storer.store( dimension );
    }
    void Allocators_Allocator::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Allocators_Allocator" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parent_context", parent_context } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "allocated_context", allocated_context } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dimension", dimension.value() } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Allocators_Nothing : public mega::io::Object
    Allocators_Nothing::Allocators_Nothing( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Allocators_Nothing >( loader, this ) )          , p_MemoryLayout_Allocators_Allocator( loader )
    {
    }
    bool Allocators_Nothing::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::Allocators_Nothing >( loader, const_cast< Allocators_Nothing* >( this ) ) };
    }
    void Allocators_Nothing::set_inheritance_pointer()
    {
        p_MemoryLayout_Allocators_Allocator->m_inheritance = data::Ptr< data::MemoryLayout::Allocators_Nothing >( p_MemoryLayout_Allocators_Allocator, this );
    }
    void Allocators_Nothing::load( mega::io::Loader& loader )
    {
        loader.load( p_MemoryLayout_Allocators_Allocator );
    }
    void Allocators_Nothing::store( mega::io::Storer& storer ) const
    {
        storer.store( p_MemoryLayout_Allocators_Allocator );
    }
    void Allocators_Nothing::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Allocators_Nothing" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Allocators_Singleton : public mega::io::Object
    Allocators_Singleton::Allocators_Singleton( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Allocators_Singleton >( loader, this ) )          , p_MemoryLayout_Allocators_Allocator( loader )
    {
    }
    bool Allocators_Singleton::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::Allocators_Singleton >( loader, const_cast< Allocators_Singleton* >( this ) ) };
    }
    void Allocators_Singleton::set_inheritance_pointer()
    {
        p_MemoryLayout_Allocators_Allocator->m_inheritance = data::Ptr< data::MemoryLayout::Allocators_Singleton >( p_MemoryLayout_Allocators_Allocator, this );
    }
    void Allocators_Singleton::load( mega::io::Loader& loader )
    {
        loader.load( p_MemoryLayout_Allocators_Allocator );
    }
    void Allocators_Singleton::store( mega::io::Storer& storer ) const
    {
        storer.store( p_MemoryLayout_Allocators_Allocator );
    }
    void Allocators_Singleton::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Allocators_Singleton" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Allocators_Range : public mega::io::Object
    Allocators_Range::Allocators_Range( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Allocators_Range >( loader, this ) )          , p_MemoryLayout_Allocators_Allocator( loader )
    {
    }
    bool Allocators_Range::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::Allocators_Range >( loader, const_cast< Allocators_Range* >( this ) ) };
    }
    void Allocators_Range::set_inheritance_pointer()
    {
        p_MemoryLayout_Allocators_Allocator->m_inheritance = data::Ptr< data::MemoryLayout::Allocators_Range >( p_MemoryLayout_Allocators_Allocator, this );
    }
    void Allocators_Range::load( mega::io::Loader& loader )
    {
        loader.load( p_MemoryLayout_Allocators_Allocator );
    }
    void Allocators_Range::store( mega::io::Storer& storer ) const
    {
        storer.store( p_MemoryLayout_Allocators_Allocator );
    }
    void Allocators_Range::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Allocators_Range" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Allocators_Range32 : public mega::io::Object
    Allocators_Range32::Allocators_Range32( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Allocators_Range32 >( loader, this ) )          , p_MemoryLayout_Allocators_Range( loader )
    {
    }
    bool Allocators_Range32::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::Allocators_Range32 >( loader, const_cast< Allocators_Range32* >( this ) ) };
    }
    void Allocators_Range32::set_inheritance_pointer()
    {
        p_MemoryLayout_Allocators_Range->m_inheritance = data::Ptr< data::MemoryLayout::Allocators_Range32 >( p_MemoryLayout_Allocators_Range, this );
    }
    void Allocators_Range32::load( mega::io::Loader& loader )
    {
        loader.load( p_MemoryLayout_Allocators_Range );
    }
    void Allocators_Range32::store( mega::io::Storer& storer ) const
    {
        storer.store( p_MemoryLayout_Allocators_Range );
    }
    void Allocators_Range32::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Allocators_Range32" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Allocators_Range64 : public mega::io::Object
    Allocators_Range64::Allocators_Range64( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Allocators_Range64 >( loader, this ) )          , p_MemoryLayout_Allocators_Range( loader )
    {
    }
    bool Allocators_Range64::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::Allocators_Range64 >( loader, const_cast< Allocators_Range64* >( this ) ) };
    }
    void Allocators_Range64::set_inheritance_pointer()
    {
        p_MemoryLayout_Allocators_Range->m_inheritance = data::Ptr< data::MemoryLayout::Allocators_Range64 >( p_MemoryLayout_Allocators_Range, this );
    }
    void Allocators_Range64::load( mega::io::Loader& loader )
    {
        loader.load( p_MemoryLayout_Allocators_Range );
    }
    void Allocators_Range64::store( mega::io::Storer& storer ) const
    {
        storer.store( p_MemoryLayout_Allocators_Range );
    }
    void Allocators_Range64::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Allocators_Range64" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Allocators_RangeAny : public mega::io::Object
    Allocators_RangeAny::Allocators_RangeAny( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::Allocators_RangeAny >( loader, this ) )          , p_MemoryLayout_Allocators_Range( loader )
    {
    }
    bool Allocators_RangeAny::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::Allocators_RangeAny >( loader, const_cast< Allocators_RangeAny* >( this ) ) };
    }
    void Allocators_RangeAny::set_inheritance_pointer()
    {
        p_MemoryLayout_Allocators_Range->m_inheritance = data::Ptr< data::MemoryLayout::Allocators_RangeAny >( p_MemoryLayout_Allocators_Range, this );
    }
    void Allocators_RangeAny::load( mega::io::Loader& loader )
    {
        loader.load( p_MemoryLayout_Allocators_Range );
    }
    void Allocators_RangeAny::store( mega::io::Storer& storer ) const
    {
        storer.store( p_MemoryLayout_Allocators_Range );
    }
    void Allocators_RangeAny::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Allocators_RangeAny" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct MemoryLayout_Part : public mega::io::Object
    MemoryLayout_Part::MemoryLayout_Part( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::MemoryLayout_Part >( loader, this ) )          , context( loader )
          , buffer( loader )
    {
    }
    MemoryLayout_Part::MemoryLayout_Part( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::U64& total_domain_size, const data::Ptr< data::Concrete::Concrete_Context >& context, const std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& user_dimensions, const std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& link_dimensions, const std::vector< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocation > >& allocation_dimensions)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::MemoryLayout_Part >( loader, this ) )          , total_domain_size( total_domain_size )
          , context( context )
          , user_dimensions( user_dimensions )
          , link_dimensions( link_dimensions )
          , allocation_dimensions( allocation_dimensions )
    {
    }
    bool MemoryLayout_Part::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::MemoryLayout_Part >( loader, const_cast< MemoryLayout_Part* >( this ) ) };
    }
    void MemoryLayout_Part::set_inheritance_pointer()
    {
    }
    void MemoryLayout_Part::load( mega::io::Loader& loader )
    {
        loader.load( total_domain_size );
        loader.load( context );
        loader.load( user_dimensions );
        loader.load( link_dimensions );
        loader.load( allocation_dimensions );
        loader.load( size );
        loader.load( alignment );
        loader.load( offset );
        loader.load( buffer );
    }
    void MemoryLayout_Part::store( mega::io::Storer& storer ) const
    {
        storer.store( total_domain_size );
        storer.store( context );
        storer.store( user_dimensions );
        storer.store( link_dimensions );
        storer.store( allocation_dimensions );
        VERIFY_RTE_MSG( size.has_value(), "MemoryLayout::MemoryLayout_Part.size has NOT been set" );
        storer.store( size );
        VERIFY_RTE_MSG( alignment.has_value(), "MemoryLayout::MemoryLayout_Part.alignment has NOT been set" );
        storer.store( alignment );
        VERIFY_RTE_MSG( offset.has_value(), "MemoryLayout::MemoryLayout_Part.offset has NOT been set" );
        storer.store( offset );
        VERIFY_RTE_MSG( buffer.has_value(), "MemoryLayout::MemoryLayout_Part.buffer has NOT been set" );
        storer.store( buffer );
    }
    void MemoryLayout_Part::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "MemoryLayout_Part" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "total_domain_size", total_domain_size } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "context", context } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "user_dimensions", user_dimensions } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "link_dimensions", link_dimensions } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "allocation_dimensions", allocation_dimensions } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "size", size.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "alignment", alignment.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "offset", offset.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "buffer", buffer.value() } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct MemoryLayout_Buffer : public mega::io::Object
    MemoryLayout_Buffer::MemoryLayout_Buffer( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::MemoryLayout_Buffer >( loader, this ) )    {
    }
    MemoryLayout_Buffer::MemoryLayout_Buffer( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::MemoryLayout::MemoryLayout_Part > >& parts, const mega::U64& size, const mega::U64& alignment)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::MemoryLayout_Buffer >( loader, this ) )          , parts( parts )
          , size( size )
          , alignment( alignment )
    {
    }
    bool MemoryLayout_Buffer::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::MemoryLayout_Buffer >( loader, const_cast< MemoryLayout_Buffer* >( this ) ) };
    }
    void MemoryLayout_Buffer::set_inheritance_pointer()
    {
    }
    void MemoryLayout_Buffer::load( mega::io::Loader& loader )
    {
        loader.load( parts );
        loader.load( size );
        loader.load( alignment );
    }
    void MemoryLayout_Buffer::store( mega::io::Storer& storer ) const
    {
        storer.store( parts );
        storer.store( size );
        storer.store( alignment );
    }
    void MemoryLayout_Buffer::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "MemoryLayout_Buffer" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "parts", parts } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "size", size } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "alignment", alignment } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct MemoryLayout_NonSimpleBuffer : public mega::io::Object
    MemoryLayout_NonSimpleBuffer::MemoryLayout_NonSimpleBuffer( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::MemoryLayout_NonSimpleBuffer >( loader, this ) )          , p_MemoryLayout_MemoryLayout_Buffer( loader )
    {
    }
    bool MemoryLayout_NonSimpleBuffer::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::MemoryLayout_NonSimpleBuffer >( loader, const_cast< MemoryLayout_NonSimpleBuffer* >( this ) ) };
    }
    void MemoryLayout_NonSimpleBuffer::set_inheritance_pointer()
    {
        p_MemoryLayout_MemoryLayout_Buffer->m_inheritance = data::Ptr< data::MemoryLayout::MemoryLayout_NonSimpleBuffer >( p_MemoryLayout_MemoryLayout_Buffer, this );
    }
    void MemoryLayout_NonSimpleBuffer::load( mega::io::Loader& loader )
    {
        loader.load( p_MemoryLayout_MemoryLayout_Buffer );
    }
    void MemoryLayout_NonSimpleBuffer::store( mega::io::Storer& storer ) const
    {
        storer.store( p_MemoryLayout_MemoryLayout_Buffer );
    }
    void MemoryLayout_NonSimpleBuffer::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "MemoryLayout_NonSimpleBuffer" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct MemoryLayout_SimpleBuffer : public mega::io::Object
    MemoryLayout_SimpleBuffer::MemoryLayout_SimpleBuffer( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::MemoryLayout_SimpleBuffer >( loader, this ) )          , p_MemoryLayout_MemoryLayout_Buffer( loader )
    {
    }
    bool MemoryLayout_SimpleBuffer::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::MemoryLayout_SimpleBuffer >( loader, const_cast< MemoryLayout_SimpleBuffer* >( this ) ) };
    }
    void MemoryLayout_SimpleBuffer::set_inheritance_pointer()
    {
        p_MemoryLayout_MemoryLayout_Buffer->m_inheritance = data::Ptr< data::MemoryLayout::MemoryLayout_SimpleBuffer >( p_MemoryLayout_MemoryLayout_Buffer, this );
    }
    void MemoryLayout_SimpleBuffer::load( mega::io::Loader& loader )
    {
        loader.load( p_MemoryLayout_MemoryLayout_Buffer );
    }
    void MemoryLayout_SimpleBuffer::store( mega::io::Storer& storer ) const
    {
        storer.store( p_MemoryLayout_MemoryLayout_Buffer );
    }
    void MemoryLayout_SimpleBuffer::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "MemoryLayout_SimpleBuffer" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct MemoryLayout_GPUBuffer : public mega::io::Object
    MemoryLayout_GPUBuffer::MemoryLayout_GPUBuffer( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::MemoryLayout_GPUBuffer >( loader, this ) )          , p_MemoryLayout_MemoryLayout_Buffer( loader )
    {
    }
    bool MemoryLayout_GPUBuffer::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::MemoryLayout::MemoryLayout_GPUBuffer >( loader, const_cast< MemoryLayout_GPUBuffer* >( this ) ) };
    }
    void MemoryLayout_GPUBuffer::set_inheritance_pointer()
    {
        p_MemoryLayout_MemoryLayout_Buffer->m_inheritance = data::Ptr< data::MemoryLayout::MemoryLayout_GPUBuffer >( p_MemoryLayout_MemoryLayout_Buffer, this );
    }
    void MemoryLayout_GPUBuffer::load( mega::io::Loader& loader )
    {
        loader.load( p_MemoryLayout_MemoryLayout_Buffer );
    }
    void MemoryLayout_GPUBuffer::store( mega::io::Storer& storer ) const
    {
        storer.store( p_MemoryLayout_MemoryLayout_Buffer );
    }
    void MemoryLayout_GPUBuffer::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "MemoryLayout_GPUBuffer" },
                { "filetype" , "MemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
}
namespace GlobalMemoryLayout
{
    // struct MemoryLayout_MemoryMap : public mega::io::Object
    MemoryLayout_MemoryMap::MemoryLayout_MemoryMap( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( loader, this ) )          , interface( loader )
    {
    }
    MemoryLayout_MemoryMap::MemoryLayout_MemoryMap( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_IContext >& interface, const mega::U64& block_size, const mega::U64& block_alignment, const mega::U64& fixed_allocation, const std::vector< data::Ptr< data::Concrete::Concrete_Object > >& concrete)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( loader, this ) )          , interface( interface )
          , block_size( block_size )
          , block_alignment( block_alignment )
          , fixed_allocation( fixed_allocation )
          , concrete( concrete )
    {
    }
    bool MemoryLayout_MemoryMap::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( loader, const_cast< MemoryLayout_MemoryMap* >( this ) ) };
    }
    void MemoryLayout_MemoryMap::set_inheritance_pointer()
    {
    }
    void MemoryLayout_MemoryMap::load( mega::io::Loader& loader )
    {
        loader.load( interface );
        loader.load( block_size );
        loader.load( block_alignment );
        loader.load( fixed_allocation );
        loader.load( concrete );
    }
    void MemoryLayout_MemoryMap::store( mega::io::Storer& storer ) const
    {
        storer.store( interface );
        storer.store( block_size );
        storer.store( block_alignment );
        storer.store( fixed_allocation );
        storer.store( concrete );
    }
    void MemoryLayout_MemoryMap::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "MemoryLayout_MemoryMap" },
                { "filetype" , "GlobalMemoryLayout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "interface", interface } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "block_size", block_size } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "block_alignment", block_alignment } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "fixed_allocation", fixed_allocation } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete", concrete } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace GlobalMemoryRollout
{
    // struct Concrete_MemoryMappedObject : public mega::io::Object
    Concrete_MemoryMappedObject::Concrete_MemoryMappedObject( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::GlobalMemoryRollout::Concrete_MemoryMappedObject >( loader, this ) )          , p_Concrete_Concrete_Object( loader )
          , memory_map( loader )
    {
    }
    Concrete_MemoryMappedObject::Concrete_MemoryMappedObject( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >& memory_map)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::GlobalMemoryRollout::Concrete_MemoryMappedObject >( loader, this ) )          , p_Concrete_Concrete_Object( loader )
          , memory_map( memory_map )
    {
    }
    bool Concrete_MemoryMappedObject::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::GlobalMemoryRollout::Concrete_MemoryMappedObject >( loader, const_cast< Concrete_MemoryMappedObject* >( this ) ) };
    }
    void Concrete_MemoryMappedObject::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Object->m_inheritance = data::Ptr< data::GlobalMemoryRollout::Concrete_MemoryMappedObject >( p_Concrete_Concrete_Object, this );
    }
    void Concrete_MemoryMappedObject::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Object );
        loader.load( memory_map );
    }
    void Concrete_MemoryMappedObject::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Object );
        storer.store( memory_map );
    }
    void Concrete_MemoryMappedObject::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_MemoryMappedObject" },
                { "filetype" , "GlobalMemoryRollout" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "memory_map", memory_map } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace ConcreteTable
{
    // struct Symbols_ConcreteTypeID : public mega::io::Object
    Symbols_ConcreteTypeID::Symbols_ConcreteTypeID( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID >( loader, this ) )    {
    }
    Symbols_ConcreteTypeID::Symbols_ConcreteTypeID( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::TypeID& id, const std::optional< data::Ptr< data::Concrete::Concrete_Context > >& context, const std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& dim_user, const std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& dim_link, const std::optional< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocation > >& dim_allocation)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID >( loader, this ) )          , id( id )
          , context( context )
          , dim_user( dim_user )
          , dim_link( dim_link )
          , dim_allocation( dim_allocation )
    {
    }
    bool Symbols_ConcreteTypeID::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID >( loader, const_cast< Symbols_ConcreteTypeID* >( this ) ) };
    }
    void Symbols_ConcreteTypeID::set_inheritance_pointer()
    {
    }
    void Symbols_ConcreteTypeID::load( mega::io::Loader& loader )
    {
        loader.load( id );
        loader.load( context );
        loader.load( dim_user );
        loader.load( dim_link );
        loader.load( dim_allocation );
    }
    void Symbols_ConcreteTypeID::store( mega::io::Storer& storer ) const
    {
        storer.store( id );
        storer.store( context );
        storer.store( dim_user );
        storer.store( dim_link );
        storer.store( dim_allocation );
    }
    void Symbols_ConcreteTypeID::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Symbols_ConcreteTypeID" },
                { "filetype" , "ConcreteTable" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "id", id } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "context", context } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dim_user", dim_user } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dim_link", dim_link } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dim_allocation", dim_allocation } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Symbols_SymbolTable : public mega::io::Object
    Symbols_SymbolTable::Symbols_SymbolTable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_SymbolTable_Symbols_SymbolTable( loader )
    {
    }
    Symbols_SymbolTable::Symbols_SymbolTable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< SymbolTable::Symbols_SymbolTable > p_SymbolTable_Symbols_SymbolTable, const std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& concrete_type_id_sequences, const std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& concrete_type_id_seq_alloc, const std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& concrete_type_id_set_link, const std::map< mega::TypeID, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& concrete_type_ids)
        :   mega::io::Object( objectInfo )          , p_SymbolTable_Symbols_SymbolTable( p_SymbolTable_Symbols_SymbolTable )
          , concrete_type_id_sequences( concrete_type_id_sequences )
          , concrete_type_id_seq_alloc( concrete_type_id_seq_alloc )
          , concrete_type_id_set_link( concrete_type_id_set_link )
          , concrete_type_ids( concrete_type_ids )
    {
    }
    bool Symbols_SymbolTable::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Symbols_SymbolTable::set_inheritance_pointer()
    {
        p_SymbolTable_Symbols_SymbolTable->p_ConcreteTable_Symbols_SymbolTable = data::Ptr< data::ConcreteTable::Symbols_SymbolTable >( p_SymbolTable_Symbols_SymbolTable, this );
    }
    void Symbols_SymbolTable::load( mega::io::Loader& loader )
    {
        loader.load( p_SymbolTable_Symbols_SymbolTable );
        loader.load( concrete_type_id_sequences );
        loader.load( concrete_type_id_seq_alloc );
        loader.load( concrete_type_id_set_link );
        loader.load( concrete_type_ids );
    }
    void Symbols_SymbolTable::store( mega::io::Storer& storer ) const
    {
        storer.store( p_SymbolTable_Symbols_SymbolTable );
        storer.store( concrete_type_id_sequences );
        storer.store( concrete_type_id_seq_alloc );
        storer.store( concrete_type_id_set_link );
        storer.store( concrete_type_ids );
    }
    void Symbols_SymbolTable::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Symbols_SymbolTable" },
                { "filetype" , "ConcreteTable" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete_type_id_sequences", concrete_type_id_sequences } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete_type_id_seq_alloc", concrete_type_id_seq_alloc } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete_type_id_set_link", concrete_type_id_set_link } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete_type_ids", concrete_type_ids } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace PerSourceConcreteTable
{
    // struct Concrete_Dimensions_User : public mega::io::Object
    Concrete_Dimensions_User::Concrete_Dimensions_User( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_User( loader )
    {
    }
    Concrete_Dimensions_User::Concrete_Dimensions_User( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Dimensions_User > p_Concrete_Concrete_Dimensions_User, const mega::TypeID& concrete_id)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_User( p_Concrete_Concrete_Dimensions_User )
          , concrete_id( concrete_id )
    {
    }
    bool Concrete_Dimensions_User::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Dimensions_User::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_User->p_PerSourceConcreteTable_Concrete_Dimensions_User = data::Ptr< data::PerSourceConcreteTable::Concrete_Dimensions_User >( p_Concrete_Concrete_Dimensions_User, this );
    }
    void Concrete_Dimensions_User::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_User );
        loader.load( concrete_id );
    }
    void Concrete_Dimensions_User::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_User );
        storer.store( concrete_id );
    }
    void Concrete_Dimensions_User::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimensions_User" },
                { "filetype" , "PerSourceConcreteTable" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete_id", concrete_id } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Dimensions_Link : public mega::io::Object
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Link( loader )
    {
    }
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Dimensions_Link > p_Concrete_Concrete_Dimensions_Link, const mega::TypeID& concrete_id)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Link( p_Concrete_Concrete_Dimensions_Link )
          , concrete_id( concrete_id )
    {
    }
    bool Concrete_Dimensions_Link::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Dimensions_Link::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_Link->p_PerSourceConcreteTable_Concrete_Dimensions_Link = data::Ptr< data::PerSourceConcreteTable::Concrete_Dimensions_Link >( p_Concrete_Concrete_Dimensions_Link, this );
    }
    void Concrete_Dimensions_Link::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_Link );
        loader.load( concrete_id );
    }
    void Concrete_Dimensions_Link::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Link );
        storer.store( concrete_id );
    }
    void Concrete_Dimensions_Link::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimensions_Link" },
                { "filetype" , "PerSourceConcreteTable" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete_id", concrete_id } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Dimensions_Allocation : public mega::io::Object
    Concrete_Dimensions_Allocation::Concrete_Dimensions_Allocation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_MemoryLayout_Concrete_Dimensions_Allocation( loader )
    {
    }
    Concrete_Dimensions_Allocation::Concrete_Dimensions_Allocation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< MemoryLayout::Concrete_Dimensions_Allocation > p_MemoryLayout_Concrete_Dimensions_Allocation, const mega::TypeID& concrete_id)
        :   mega::io::Object( objectInfo )          , p_MemoryLayout_Concrete_Dimensions_Allocation( p_MemoryLayout_Concrete_Dimensions_Allocation )
          , concrete_id( concrete_id )
    {
    }
    bool Concrete_Dimensions_Allocation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Dimensions_Allocation::set_inheritance_pointer()
    {
        p_MemoryLayout_Concrete_Dimensions_Allocation->p_PerSourceConcreteTable_Concrete_Dimensions_Allocation = data::Ptr< data::PerSourceConcreteTable::Concrete_Dimensions_Allocation >( p_MemoryLayout_Concrete_Dimensions_Allocation, this );
    }
    void Concrete_Dimensions_Allocation::load( mega::io::Loader& loader )
    {
        loader.load( p_MemoryLayout_Concrete_Dimensions_Allocation );
        loader.load( concrete_id );
    }
    void Concrete_Dimensions_Allocation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_MemoryLayout_Concrete_Dimensions_Allocation );
        storer.store( concrete_id );
    }
    void Concrete_Dimensions_Allocation::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Dimensions_Allocation" },
                { "filetype" , "PerSourceConcreteTable" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete_id", concrete_id } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Concrete_Context : public mega::io::Object
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Context( loader )
    {
    }
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Context > p_Concrete_Concrete_Context, const mega::TypeID& concrete_id)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Context( p_Concrete_Concrete_Context )
          , concrete_id( concrete_id )
    {
    }
    bool Concrete_Context::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Context::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Context->p_PerSourceConcreteTable_Concrete_Context = data::Ptr< data::PerSourceConcreteTable::Concrete_Context >( p_Concrete_Concrete_Context, this );
    }
    void Concrete_Context::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Context );
        loader.load( concrete_id );
    }
    void Concrete_Context::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( concrete_id );
    }
    void Concrete_Context::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Concrete_Context" },
                { "filetype" , "PerSourceConcreteTable" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete_id", concrete_id } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace AutomataAnalysis
{
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
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Variables_Variable >( loader, const_cast< Invocations_Variables_Variable* >( this ) ) };
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
    void Invocations_Variables_Variable::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Variables_Stack : public mega::io::Object
    Invocations_Variables_Stack::Invocations_Variables_Stack( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Stack >( loader, this ) )          , p_Operations_Invocations_Variables_Variable( loader )
          , concrete( loader )
    {
    }
    Invocations_Variables_Stack::Invocations_Variables_Stack( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Graph_Vertex >& concrete)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Stack >( loader, this ) )          , p_Operations_Invocations_Variables_Variable( loader )
          , concrete( concrete )
    {
    }
    bool Invocations_Variables_Stack::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Variables_Stack >( loader, const_cast< Invocations_Variables_Stack* >( this ) ) };
    }
    void Invocations_Variables_Stack::set_inheritance_pointer()
    {
        p_Operations_Invocations_Variables_Variable->m_inheritance = data::Ptr< data::Operations::Invocations_Variables_Stack >( p_Operations_Invocations_Variables_Variable, this );
    }
    void Invocations_Variables_Stack::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Variables_Variable );
        loader.load( concrete );
    }
    void Invocations_Variables_Stack::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Variables_Variable );
        storer.store( concrete );
    }
    void Invocations_Variables_Stack::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Invocations_Variables_Stack" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "concrete", concrete } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Variables_Reference : public mega::io::Object
    Invocations_Variables_Reference::Invocations_Variables_Reference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Reference >( loader, this ) )          , p_Operations_Invocations_Variables_Variable( loader )
    {
    }
    Invocations_Variables_Reference::Invocations_Variables_Reference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Concrete::Concrete_Graph_Vertex > >& types)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Reference >( loader, this ) )          , p_Operations_Invocations_Variables_Variable( loader )
          , types( types )
    {
    }
    bool Invocations_Variables_Reference::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Variables_Reference >( loader, const_cast< Invocations_Variables_Reference* >( this ) ) };
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
    void Invocations_Variables_Reference::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Variables_Memory : public mega::io::Object
    Invocations_Variables_Memory::Invocations_Variables_Memory( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Memory >( loader, this ) )          , p_Operations_Invocations_Variables_Reference( loader )
    {
    }
    bool Invocations_Variables_Memory::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Variables_Memory >( loader, const_cast< Invocations_Variables_Memory* >( this ) ) };
    }
    void Invocations_Variables_Memory::set_inheritance_pointer()
    {
        p_Operations_Invocations_Variables_Reference->m_inheritance = data::Ptr< data::Operations::Invocations_Variables_Memory >( p_Operations_Invocations_Variables_Reference, this );
    }
    void Invocations_Variables_Memory::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Variables_Reference );
    }
    void Invocations_Variables_Memory::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Variables_Reference );
    }
    void Invocations_Variables_Memory::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Invocations_Variables_Memory" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Invocations_Variables_Parameter : public mega::io::Object
    Invocations_Variables_Parameter::Invocations_Variables_Parameter( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_Parameter >( loader, this ) )          , p_Operations_Invocations_Variables_Reference( loader )
    {
    }
    bool Invocations_Variables_Parameter::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Variables_Parameter >( loader, const_cast< Invocations_Variables_Parameter* >( this ) ) };
    }
    void Invocations_Variables_Parameter::set_inheritance_pointer()
    {
        p_Operations_Invocations_Variables_Reference->m_inheritance = data::Ptr< data::Operations::Invocations_Variables_Parameter >( p_Operations_Invocations_Variables_Reference, this );
    }
    void Invocations_Variables_Parameter::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Variables_Reference );
    }
    void Invocations_Variables_Parameter::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Variables_Reference );
    }
    void Invocations_Variables_Parameter::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Invocations_Variables_Parameter" },
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
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Instructions_Instruction >( loader, const_cast< Invocations_Instructions_Instruction* >( this ) ) };
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
    void Invocations_Instructions_Instruction::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Instructions_InstructionGroup >( loader, const_cast< Invocations_Instructions_InstructionGroup* >( this ) ) };
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
    void Invocations_Instructions_InstructionGroup::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_Root : public mega::io::Object
    Invocations_Instructions_Root::Invocations_Instructions_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Root >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , parameter( loader )
    {
    }
    Invocations_Instructions_Root::Invocations_Instructions_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Parameter >& parameter)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Root >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , parameter( parameter )
    {
    }
    bool Invocations_Instructions_Root::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Instructions_Root >( loader, const_cast< Invocations_Instructions_Root* >( this ) ) };
    }
    void Invocations_Instructions_Root::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_Root >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_Root::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( parameter );
    }
    void Invocations_Instructions_Root::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( parameter );
    }
    void Invocations_Instructions_Root::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "parameter", parameter } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_ParentDerivation : public mega::io::Object
    Invocations_Instructions_ParentDerivation::Invocations_Instructions_ParentDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from( loader )
          , to( loader )
    {
    }
    Invocations_Instructions_ParentDerivation::Invocations_Instructions_ParentDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Variable >& from, const data::Ptr< data::Operations::Invocations_Variables_Stack >& to)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from( from )
          , to( to )
    {
    }
    bool Invocations_Instructions_ParentDerivation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Instructions_ParentDerivation >( loader, const_cast< Invocations_Instructions_ParentDerivation* >( this ) ) };
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
    void Invocations_Instructions_ParentDerivation::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "to", to } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_ChildDerivation : public mega::io::Object
    Invocations_Instructions_ChildDerivation::Invocations_Instructions_ChildDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from( loader )
          , to( loader )
    {
    }
    Invocations_Instructions_ChildDerivation::Invocations_Instructions_ChildDerivation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Variable >& from, const data::Ptr< data::Operations::Invocations_Variables_Stack >& to)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from( from )
          , to( to )
    {
    }
    bool Invocations_Instructions_ChildDerivation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Instructions_ChildDerivation >( loader, const_cast< Invocations_Instructions_ChildDerivation* >( this ) ) };
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
    void Invocations_Instructions_ChildDerivation::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "to", to } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_PolyBranch : public mega::io::Object
    Invocations_Instructions_PolyBranch::Invocations_Instructions_PolyBranch( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_PolyBranch >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from_reference( loader )
    {
    }
    Invocations_Instructions_PolyBranch::Invocations_Instructions_PolyBranch( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Reference >& from_reference)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_PolyBranch >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , from_reference( from_reference )
    {
    }
    bool Invocations_Instructions_PolyBranch::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Instructions_PolyBranch >( loader, const_cast< Invocations_Instructions_PolyBranch* >( this ) ) };
    }
    void Invocations_Instructions_PolyBranch::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_PolyBranch >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_PolyBranch::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( from_reference );
    }
    void Invocations_Instructions_PolyBranch::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( from_reference );
    }
    void Invocations_Instructions_PolyBranch::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Invocations_Instructions_PolyBranch" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "from_reference", from_reference } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Instructions_PolyCase : public mega::io::Object
    Invocations_Instructions_PolyCase::Invocations_Instructions_PolyCase( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_PolyCase >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , reference( loader )
          , type( loader )
    {
    }
    Invocations_Instructions_PolyCase::Invocations_Instructions_PolyCase( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Reference >& reference, const data::Ptr< data::Concrete::Concrete_Context >& type)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_PolyCase >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , reference( reference )
          , type( type )
    {
    }
    bool Invocations_Instructions_PolyCase::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Instructions_PolyCase >( loader, const_cast< Invocations_Instructions_PolyCase* >( this ) ) };
    }
    void Invocations_Instructions_PolyCase::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_PolyCase >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_PolyCase::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( reference );
        loader.load( type );
    }
    void Invocations_Instructions_PolyCase::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( reference );
        storer.store( type );
    }
    void Invocations_Instructions_PolyCase::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "type", type } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Invocations_Operations_Operation : public mega::io::Object
    Invocations_Operations_Operation::Invocations_Operations_Operation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Operation >( loader, this ) )          , p_Operations_Invocations_Instructions_Instruction( loader )
          , variable( loader )
    {
    }
    Invocations_Operations_Operation::Invocations_Operations_Operation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Variable >& variable)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Operation >( loader, this ) )          , p_Operations_Invocations_Instructions_Instruction( loader )
          , variable( variable )
    {
    }
    bool Invocations_Operations_Operation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Operations_Operation >( loader, const_cast< Invocations_Operations_Operation* >( this ) ) };
    }
    void Invocations_Operations_Operation::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_Instruction->m_inheritance = data::Ptr< data::Operations::Invocations_Operations_Operation >( p_Operations_Invocations_Instructions_Instruction, this );
    }
    void Invocations_Operations_Operation::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_Instruction );
        loader.load( variable );
    }
    void Invocations_Operations_Operation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_Instruction );
        storer.store( variable );
    }
    void Invocations_Operations_Operation::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "variable", variable } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Derivation_Edge : public mega::io::Object
    Derivation_Edge::Derivation_Edge( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Derivation_Edge >( loader, this ) )          , next( loader )
    {
    }
    Derivation_Edge::Derivation_Edge( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Derivation_Step >& next, const bool& eliminated, const std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& edges)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Derivation_Edge >( loader, this ) )          , next( next )
          , eliminated( eliminated )
          , edges( edges )
    {
    }
    bool Derivation_Edge::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Derivation_Edge >( loader, const_cast< Derivation_Edge* >( this ) ) };
    }
    void Derivation_Edge::set_inheritance_pointer()
    {
    }
    void Derivation_Edge::load( mega::io::Loader& loader )
    {
        loader.load( next );
        loader.load( eliminated );
        loader.load( edges );
    }
    void Derivation_Edge::store( mega::io::Storer& storer ) const
    {
        storer.store( next );
        storer.store( eliminated );
        storer.store( edges );
    }
    void Derivation_Edge::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Derivation_Edge" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "next", next } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "eliminated", eliminated } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "edges", edges } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Derivation_Step : public mega::io::Object
    Derivation_Step::Derivation_Step( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Derivation_Step >( loader, this ) )          , vertex( loader )
    {
    }
    Derivation_Step::Derivation_Step( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Graph_Vertex >& vertex, const std::vector< data::Ptr< data::Operations::Derivation_Edge > >& edges)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Derivation_Step >( loader, this ) )          , vertex( vertex )
          , edges( edges )
    {
    }
    bool Derivation_Step::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Derivation_Step >( loader, const_cast< Derivation_Step* >( this ) ) };
    }
    void Derivation_Step::set_inheritance_pointer()
    {
    }
    void Derivation_Step::load( mega::io::Loader& loader )
    {
        loader.load( vertex );
        loader.load( edges );
    }
    void Derivation_Step::store( mega::io::Storer& storer ) const
    {
        storer.store( vertex );
        storer.store( edges );
    }
    void Derivation_Step::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Derivation_Step" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "vertex", vertex } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "edges", edges } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Derivation_And : public mega::io::Object
    Derivation_And::Derivation_And( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Derivation_And >( loader, this ) )          , p_Operations_Derivation_Step( loader )
    {
    }
    bool Derivation_And::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Derivation_And >( loader, const_cast< Derivation_And* >( this ) ) };
    }
    void Derivation_And::set_inheritance_pointer()
    {
        p_Operations_Derivation_Step->m_inheritance = data::Ptr< data::Operations::Derivation_And >( p_Operations_Derivation_Step, this );
    }
    void Derivation_And::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Derivation_Step );
    }
    void Derivation_And::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Derivation_Step );
    }
    void Derivation_And::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Derivation_And" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Derivation_Or : public mega::io::Object
    Derivation_Or::Derivation_Or( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Derivation_Or >( loader, this ) )          , p_Operations_Derivation_Step( loader )
    {
    }
    bool Derivation_Or::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Derivation_Or >( loader, const_cast< Derivation_Or* >( this ) ) };
    }
    void Derivation_Or::set_inheritance_pointer()
    {
        p_Operations_Derivation_Step->m_inheritance = data::Ptr< data::Operations::Derivation_Or >( p_Operations_Derivation_Step, this );
    }
    void Derivation_Or::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Derivation_Step );
    }
    void Derivation_Or::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Derivation_Step );
    }
    void Derivation_Or::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Derivation_Or" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Derivation_Root : public mega::io::Object
    Derivation_Root::Derivation_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Derivation_Root >( loader, this ) )    {
    }
    Derivation_Root::Derivation_Root( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Concrete::Concrete_Graph_Vertex > >& context, const std::vector< data::Ptr< data::Operations::Derivation_Edge > >& edges)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Derivation_Root >( loader, this ) )          , context( context )
          , edges( edges )
    {
    }
    bool Derivation_Root::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Derivation_Root >( loader, const_cast< Derivation_Root* >( this ) ) };
    }
    void Derivation_Root::set_inheritance_pointer()
    {
    }
    void Derivation_Root::load( mega::io::Loader& loader )
    {
        loader.load( context );
        loader.load( edges );
    }
    void Derivation_Root::store( mega::io::Storer& storer ) const
    {
        storer.store( context );
        storer.store( edges );
    }
    void Derivation_Root::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Derivation_Root" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "context", context } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "edges", edges } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_Invocation : public mega::io::Object
    Operations_Invocation::Operations_Invocation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Invocation >( loader, this ) )          , derivation( loader )
          , root_instruction( loader )
    {
    }
    Operations_Invocation::Operations_Invocation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::InvocationID& id, const data::Ptr< data::Operations::Derivation_Root >& derivation, const std::vector< data::Ptr< data::Operations::Invocations_Variables_Variable > >& variables, const std::vector< data::Ptr< data::Operations::Invocations_Operations_Operation > >& operations)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Invocation >( loader, this ) )          , id( id )
          , derivation( derivation )
          , variables( variables )
          , operations( operations )
    {
    }
    bool Operations_Invocation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Invocation >( loader, const_cast< Operations_Invocation* >( this ) ) };
    }
    void Operations_Invocation::set_inheritance_pointer()
    {
    }
    void Operations_Invocation::load( mega::io::Loader& loader )
    {
        loader.load( id );
        loader.load( derivation );
        loader.load( root_instruction );
        loader.load( variables );
        loader.load( operations );
    }
    void Operations_Invocation::store( mega::io::Storer& storer ) const
    {
        storer.store( id );
        storer.store( derivation );
        VERIFY_RTE_MSG( root_instruction.has_value(), "Operations::Operations_Invocation.root_instruction has NOT been set" );
        storer.store( root_instruction );
        storer.store( variables );
        storer.store( operations );
    }
    void Operations_Invocation::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
                { "id", id } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "derivation", derivation } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "root_instruction", root_instruction.value() } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "variables", variables } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "operations", operations } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Operations_Allocate : public mega::io::Object
    Operations_Allocate::Operations_Allocate( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Allocate >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_Allocate::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Allocate >( loader, const_cast< Operations_Allocate* >( this ) ) };
    }
    void Operations_Allocate::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_Allocate >( p_Operations_Operations_Invocation, this );
    }
    void Operations_Allocate::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_Allocate::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_Allocate::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_Allocate" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_Call : public mega::io::Object
    Operations_Call::Operations_Call( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Call >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_Call::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Call >( loader, const_cast< Operations_Call* >( this ) ) };
    }
    void Operations_Call::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_Call >( p_Operations_Operations_Invocation, this );
    }
    void Operations_Call::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_Call::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_Call::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_Call" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_Start : public mega::io::Object
    Operations_Start::Operations_Start( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Start >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_Start::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Start >( loader, const_cast< Operations_Start* >( this ) ) };
    }
    void Operations_Start::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_Start >( p_Operations_Operations_Invocation, this );
    }
    void Operations_Start::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_Start::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_Start::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_Start" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_Stop : public mega::io::Object
    Operations_Stop::Operations_Stop( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Stop >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_Stop::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Stop >( loader, const_cast< Operations_Stop* >( this ) ) };
    }
    void Operations_Stop::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_Stop >( p_Operations_Operations_Invocation, this );
    }
    void Operations_Stop::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_Stop::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_Stop::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_Stop" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_Move : public mega::io::Object
    Operations_Move::Operations_Move( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Move >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_Move::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Move >( loader, const_cast< Operations_Move* >( this ) ) };
    }
    void Operations_Move::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_Move >( p_Operations_Operations_Invocation, this );
    }
    void Operations_Move::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_Move::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_Move::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_Move" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_GetAction : public mega::io::Object
    Operations_GetAction::Operations_GetAction( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_GetAction >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_GetAction::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_GetAction >( loader, const_cast< Operations_GetAction* >( this ) ) };
    }
    void Operations_GetAction::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_GetAction >( p_Operations_Operations_Invocation, this );
    }
    void Operations_GetAction::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_GetAction::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_GetAction::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_GetAction" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_GetDimension : public mega::io::Object
    Operations_GetDimension::Operations_GetDimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_GetDimension >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_GetDimension::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_GetDimension >( loader, const_cast< Operations_GetDimension* >( this ) ) };
    }
    void Operations_GetDimension::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_GetDimension >( p_Operations_Operations_Invocation, this );
    }
    void Operations_GetDimension::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_GetDimension::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_GetDimension::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_GetDimension" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_Read : public mega::io::Object
    Operations_Read::Operations_Read( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Read >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_Read::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Read >( loader, const_cast< Operations_Read* >( this ) ) };
    }
    void Operations_Read::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_Read >( p_Operations_Operations_Invocation, this );
    }
    void Operations_Read::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_Read::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_Read::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_Read" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_Write : public mega::io::Object
    Operations_Write::Operations_Write( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Write >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_Write::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Write >( loader, const_cast< Operations_Write* >( this ) ) };
    }
    void Operations_Write::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_Write >( p_Operations_Operations_Invocation, this );
    }
    void Operations_Write::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_Write::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_Write::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_Write" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_ReadLink : public mega::io::Object
    Operations_ReadLink::Operations_ReadLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ReadLink >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_ReadLink::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_ReadLink >( loader, const_cast< Operations_ReadLink* >( this ) ) };
    }
    void Operations_ReadLink::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_ReadLink >( p_Operations_Operations_Invocation, this );
    }
    void Operations_ReadLink::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_ReadLink::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_ReadLink::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_ReadLink" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_WriteLink : public mega::io::Object
    Operations_WriteLink::Operations_WriteLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_WriteLink >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_WriteLink::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_WriteLink >( loader, const_cast< Operations_WriteLink* >( this ) ) };
    }
    void Operations_WriteLink::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_WriteLink >( p_Operations_Operations_Invocation, this );
    }
    void Operations_WriteLink::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_WriteLink::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_WriteLink::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_WriteLink" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_Range : public mega::io::Object
    Operations_Range::Operations_Range( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Range >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_Range::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Range >( loader, const_cast< Operations_Range* >( this ) ) };
    }
    void Operations_Range::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_Range >( p_Operations_Operations_Invocation, this );
    }
    void Operations_Range::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_Range::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
    void Operations_Range::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Operations_Range" },
                { "filetype" , "Operations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
    }
        
    // struct Operations_Invocations : public mega::io::Object
    Operations_Invocations::Operations_Invocations( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Invocations >( loader, this ) )    {
    }
    Operations_Invocations::Operations_Invocations( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::map< mega::InvocationID, data::Ptr< data::Operations::Operations_Invocation > >& invocations)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Invocations >( loader, this ) )          , invocations( invocations )
    {
    }
    bool Operations_Invocations::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Invocations >( loader, const_cast< Operations_Invocations* >( this ) ) };
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
    void Operations_Invocations::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
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
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace Locations
{
    // struct Interface_InvocationInstance : public mega::io::Object
    Interface_InvocationInstance::Interface_InvocationInstance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Locations::Interface_InvocationInstance >( loader, this ) )          , invocation( loader )
    {
    }
    Interface_InvocationInstance::Interface_InvocationInstance( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Operations_Invocation >& invocation, const mega::SourceLocation& source_location)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Locations::Interface_InvocationInstance >( loader, this ) )          , invocation( invocation )
          , source_location( source_location )
    {
    }
    bool Interface_InvocationInstance::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Locations::Interface_InvocationInstance >( loader, const_cast< Interface_InvocationInstance* >( this ) ) };
    }
    void Interface_InvocationInstance::set_inheritance_pointer()
    {
    }
    void Interface_InvocationInstance::load( mega::io::Loader& loader )
    {
        loader.load( invocation );
        loader.load( source_location );
    }
    void Interface_InvocationInstance::store( mega::io::Storer& storer ) const
    {
        storer.store( invocation );
        storer.store( source_location );
    }
    void Interface_InvocationInstance::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_InvocationInstance" },
                { "filetype" , "Locations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "invocation", invocation } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_location", source_location } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct Interface_InvocationContext : public mega::io::Object
    Interface_InvocationContext::Interface_InvocationContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_InvocationContext( loader )
    {
    }
    Interface_InvocationContext::Interface_InvocationContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_InvocationContext > p_Tree_Interface_InvocationContext, const std::vector< data::Ptr< data::Locations::Interface_InvocationInstance > >& invocation_instances, const mega::SourceLocation& source_location)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_InvocationContext( p_Tree_Interface_InvocationContext )
          , invocation_instances( invocation_instances )
          , source_location( source_location )
    {
    }
    bool Interface_InvocationContext::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_InvocationContext::set_inheritance_pointer()
    {
        p_Tree_Interface_InvocationContext->p_Locations_Interface_InvocationContext = data::Ptr< data::Locations::Interface_InvocationContext >( p_Tree_Interface_InvocationContext, this );
    }
    void Interface_InvocationContext::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_InvocationContext );
        loader.load( invocation_instances );
        loader.load( source_location );
    }
    void Interface_InvocationContext::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_InvocationContext );
        storer.store( invocation_instances );
        storer.store( source_location );
    }
    void Interface_InvocationContext::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "Interface_InvocationContext" },
                { "filetype" , "Locations" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "invocation_instances", invocation_instances } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "source_location", source_location } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}
namespace ValueSpace
{
}
namespace UnityAnalysis
{
    // struct UnityAnalysis_DataBinding : public mega::io::Object
    UnityAnalysis_DataBinding::UnityAnalysis_DataBinding( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_DataBinding >( loader, this ) )    {
    }
    UnityAnalysis_DataBinding::UnityAnalysis_DataBinding( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& typeName, const mega::TypeID& interfaceTypeID)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_DataBinding >( loader, this ) )          , typeName( typeName )
          , interfaceTypeID( interfaceTypeID )
    {
    }
    bool UnityAnalysis_DataBinding::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::UnityAnalysis::UnityAnalysis_DataBinding >( loader, const_cast< UnityAnalysis_DataBinding* >( this ) ) };
    }
    void UnityAnalysis_DataBinding::set_inheritance_pointer()
    {
    }
    void UnityAnalysis_DataBinding::load( mega::io::Loader& loader )
    {
        loader.load( typeName );
        loader.load( interfaceTypeID );
    }
    void UnityAnalysis_DataBinding::store( mega::io::Storer& storer ) const
    {
        storer.store( typeName );
        storer.store( interfaceTypeID );
    }
    void UnityAnalysis_DataBinding::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "UnityAnalysis_DataBinding" },
                { "filetype" , "UnityAnalysis" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "typeName", typeName } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interfaceTypeID", interfaceTypeID } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct UnityAnalysis_LinkBinding : public mega::io::Object
    UnityAnalysis_LinkBinding::UnityAnalysis_LinkBinding( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_LinkBinding >( loader, this ) )    {
    }
    UnityAnalysis_LinkBinding::UnityAnalysis_LinkBinding( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& typeName, const mega::TypeID& interfaceTypeID)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_LinkBinding >( loader, this ) )          , typeName( typeName )
          , interfaceTypeID( interfaceTypeID )
    {
    }
    bool UnityAnalysis_LinkBinding::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::UnityAnalysis::UnityAnalysis_LinkBinding >( loader, const_cast< UnityAnalysis_LinkBinding* >( this ) ) };
    }
    void UnityAnalysis_LinkBinding::set_inheritance_pointer()
    {
    }
    void UnityAnalysis_LinkBinding::load( mega::io::Loader& loader )
    {
        loader.load( typeName );
        loader.load( interfaceTypeID );
    }
    void UnityAnalysis_LinkBinding::store( mega::io::Storer& storer ) const
    {
        storer.store( typeName );
        storer.store( interfaceTypeID );
    }
    void UnityAnalysis_LinkBinding::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "UnityAnalysis_LinkBinding" },
                { "filetype" , "UnityAnalysis" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "typeName", typeName } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interfaceTypeID", interfaceTypeID } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct UnityAnalysis_ObjectBinding : public mega::io::Object
    UnityAnalysis_ObjectBinding::UnityAnalysis_ObjectBinding( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( loader, this ) )    {
    }
    UnityAnalysis_ObjectBinding::UnityAnalysis_ObjectBinding( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& typeName, const mega::TypeID& interfaceTypeID, const std::vector< data::Ptr< data::UnityAnalysis::UnityAnalysis_DataBinding > >& dataBindings, const std::vector< data::Ptr< data::UnityAnalysis::UnityAnalysis_LinkBinding > >& linkBindings)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( loader, this ) )          , typeName( typeName )
          , interfaceTypeID( interfaceTypeID )
          , dataBindings( dataBindings )
          , linkBindings( linkBindings )
    {
    }
    bool UnityAnalysis_ObjectBinding::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( loader, const_cast< UnityAnalysis_ObjectBinding* >( this ) ) };
    }
    void UnityAnalysis_ObjectBinding::set_inheritance_pointer()
    {
    }
    void UnityAnalysis_ObjectBinding::load( mega::io::Loader& loader )
    {
        loader.load( typeName );
        loader.load( interfaceTypeID );
        loader.load( dataBindings );
        loader.load( linkBindings );
    }
    void UnityAnalysis_ObjectBinding::store( mega::io::Storer& storer ) const
    {
        storer.store( typeName );
        storer.store( interfaceTypeID );
        storer.store( dataBindings );
        storer.store( linkBindings );
    }
    void UnityAnalysis_ObjectBinding::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "UnityAnalysis_ObjectBinding" },
                { "filetype" , "UnityAnalysis" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "typeName", typeName } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "interfaceTypeID", interfaceTypeID } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dataBindings", dataBindings } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "linkBindings", linkBindings } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct UnityAnalysis_Prefab : public mega::io::Object
    UnityAnalysis_Prefab::UnityAnalysis_Prefab( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_Prefab >( loader, this ) )          , p_UnityAnalysis_UnityAnalysis_ObjectBinding( loader )
    {
    }
    UnityAnalysis_Prefab::UnityAnalysis_Prefab( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& guid)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_Prefab >( loader, this ) )          , p_UnityAnalysis_UnityAnalysis_ObjectBinding( loader )
          , guid( guid )
    {
    }
    bool UnityAnalysis_Prefab::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::UnityAnalysis::UnityAnalysis_Prefab >( loader, const_cast< UnityAnalysis_Prefab* >( this ) ) };
    }
    void UnityAnalysis_Prefab::set_inheritance_pointer()
    {
        p_UnityAnalysis_UnityAnalysis_ObjectBinding->m_inheritance = data::Ptr< data::UnityAnalysis::UnityAnalysis_Prefab >( p_UnityAnalysis_UnityAnalysis_ObjectBinding, this );
    }
    void UnityAnalysis_Prefab::load( mega::io::Loader& loader )
    {
        loader.load( p_UnityAnalysis_UnityAnalysis_ObjectBinding );
        loader.load( guid );
    }
    void UnityAnalysis_Prefab::store( mega::io::Storer& storer ) const
    {
        storer.store( p_UnityAnalysis_UnityAnalysis_ObjectBinding );
        storer.store( guid );
    }
    void UnityAnalysis_Prefab::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "UnityAnalysis_Prefab" },
                { "filetype" , "UnityAnalysis" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "guid", guid } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct UnityAnalysis_Manual : public mega::io::Object
    UnityAnalysis_Manual::UnityAnalysis_Manual( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_Manual >( loader, this ) )          , p_UnityAnalysis_UnityAnalysis_ObjectBinding( loader )
    {
    }
    UnityAnalysis_Manual::UnityAnalysis_Manual( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& name)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_Manual >( loader, this ) )          , p_UnityAnalysis_UnityAnalysis_ObjectBinding( loader )
          , name( name )
    {
    }
    bool UnityAnalysis_Manual::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::UnityAnalysis::UnityAnalysis_Manual >( loader, const_cast< UnityAnalysis_Manual* >( this ) ) };
    }
    void UnityAnalysis_Manual::set_inheritance_pointer()
    {
        p_UnityAnalysis_UnityAnalysis_ObjectBinding->m_inheritance = data::Ptr< data::UnityAnalysis::UnityAnalysis_Manual >( p_UnityAnalysis_UnityAnalysis_ObjectBinding, this );
    }
    void UnityAnalysis_Manual::load( mega::io::Loader& loader )
    {
        loader.load( p_UnityAnalysis_UnityAnalysis_ObjectBinding );
        loader.load( name );
    }
    void UnityAnalysis_Manual::store( mega::io::Storer& storer ) const
    {
        storer.store( p_UnityAnalysis_UnityAnalysis_ObjectBinding );
        storer.store( name );
    }
    void UnityAnalysis_Manual::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "UnityAnalysis_Manual" },
                { "filetype" , "UnityAnalysis" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "name", name } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
    // struct UnityAnalysis_Binding : public mega::io::Object
    UnityAnalysis_Binding::UnityAnalysis_Binding( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_Binding >( loader, this ) )          , binding( loader )
          , object( loader )
    {
    }
    UnityAnalysis_Binding::UnityAnalysis_Binding( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::UnityAnalysis::UnityAnalysis_ObjectBinding >& binding, const data::Ptr< data::Concrete::Concrete_Object >& object, const std::map< data::Ptr< data::Concrete::Concrete_Dimensions_User >, data::Ptr< data::UnityAnalysis::UnityAnalysis_DataBinding > >& dataBindings, const std::map< data::Ptr< data::Concrete::Concrete_Dimensions_Link >, data::Ptr< data::UnityAnalysis::UnityAnalysis_LinkBinding > >& linkBindings)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::UnityAnalysis::UnityAnalysis_Binding >( loader, this ) )          , binding( binding )
          , object( object )
          , dataBindings( dataBindings )
          , linkBindings( linkBindings )
    {
    }
    bool UnityAnalysis_Binding::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::UnityAnalysis::UnityAnalysis_Binding >( loader, const_cast< UnityAnalysis_Binding* >( this ) ) };
    }
    void UnityAnalysis_Binding::set_inheritance_pointer()
    {
    }
    void UnityAnalysis_Binding::load( mega::io::Loader& loader )
    {
        loader.load( binding );
        loader.load( object );
        loader.load( dataBindings );
        loader.load( linkBindings );
    }
    void UnityAnalysis_Binding::store( mega::io::Storer& storer ) const
    {
        storer.store( binding );
        storer.store( object );
        storer.store( dataBindings );
        storer.store( linkBindings );
    }
    void UnityAnalysis_Binding::to_json( nlohmann::json& _part__ ) const
    {
        _part__ = nlohmann::json::object(
            { 
                { "partname", "UnityAnalysis_Binding" },
                { "filetype" , "UnityAnalysis" },
                { "typeID", Object_Part_Type_ID },
                { "fileID", getFileID() },
                { "index", getIndex() }, 
                { "properties", nlohmann::json::array() }
            });
        {
            nlohmann::json property = nlohmann::json::object({
                { "binding", binding } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "object", object } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "dataBindings", dataBindings } } );
            _part__[ "properties" ].push_back( property );
        }
        {
            nlohmann::json property = nlohmann::json::object({
                { "linkBindings", linkBindings } } );
            _part__[ "properties" ].push_back( property );
        }
    }
        
}

std::vector< data::Ptr< data::Model::Alias_Edge > >& Alias_AliasDerivation_push_back_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_AliasDerivation::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_AliasDerivation >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& Alias_Edge_push_back_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_Edge::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Edge >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Alias_Edge > >& Alias_Step_push_back_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_Step::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->edges;
        case data::Model::Alias_And::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->edges;
        case data::Model::Alias_Or::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& Components_Component_push_back_cpp_defines(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->cpp_defines;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& Components_Component_push_back_cpp_flags(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->cpp_flags;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::cppFilePath >& Components_Component_push_back_cpp_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->cpp_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& Components_Component_push_back_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< boost::filesystem::path >& Components_Component_push_back_include_directories(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->include_directories;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& Components_Component_push_back_mega_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->mega_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::schFilePath >& Components_Component_push_back_sch_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->sch_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Context > >& Concrete_ContextGroup_push_back_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocator > >& Concrete_Context_push_back_allocation_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& Concrete_Context_push_back_inheritance(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& Concrete_Graph_Vertex_push_back_in_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Graph_Vertex::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& Concrete_Graph_Vertex_push_back_out_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Graph_Vertex::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::MemoryLayout_Buffer > >& Concrete_Object_push_back_buffers(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Object >( m_data )->buffers;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_UserDimensionContext > >& Concrete_Object_push_back_link_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Object >( m_data )->link_contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& Concrete_UserDimensionContext_push_back_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& Concrete_UserDimensionContext_push_back_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::io::cppFilePath, data::Ptr< data::DPGraph::Dependencies_TransitiveDependencies > >& Dependencies_Analysis_insert_cpp_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->cpp_dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::io::megaFilePath, data::Ptr< data::DPGraph::Dependencies_TransitiveDependencies > >& Dependencies_Analysis_insert_mega_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->mega_dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::DPGraph::Dependencies_SourceFileDependencies > >& Dependencies_Analysis_push_back_objects(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->objects;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& Dependencies_Analysis_push_back_topological_mega_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->topological_mega_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::DPGraph::Dependencies_Glob > >& Dependencies_SourceFileDependencies_push_back_globs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_SourceFileDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_SourceFileDependencies >( m_data )->globs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< boost::filesystem::path >& Dependencies_SourceFileDependencies_push_back_resolution(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_SourceFileDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_SourceFileDependencies >( m_data )->resolution;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& Dependencies_TransitiveDependencies_push_back_mega_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_TransitiveDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_TransitiveDependencies >( m_data )->mega_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& Derivation_Edge_push_back_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Edge::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Edge >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Graph_Vertex > >& Derivation_Root_push_back_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Root::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Root >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Derivation_Edge > >& Derivation_Root_push_back_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Root::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Root >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Derivation_Edge > >& Derivation_Step_push_back_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Step::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->edges;
        case data::Operations::Derivation_And::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->edges;
        case data::Operations::Derivation_Or::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< data::Ptr< data::Clang::Interface_ObjectLinkTrait >, data::Ptr< data::Model::HyperGraph_Relation > >& HyperGraph_Graph_insert_relations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_Graph::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Graph >( m_data )->relations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_ObjectLinkTrait > >& HyperGraph_OwningObjectRelation_push_back_owners(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_OwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_OwningObjectRelation >( m_data )->owners;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Concrete::Concrete_Context > >& Inheritance_Mapping_insert_inheritance_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->inheritance_contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_User > >& Inheritance_Mapping_insert_inheritance_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->inheritance_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& Inheritance_Mapping_insert_inheritance_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->inheritance_links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Derivations::Inheritance_ObjectMapping > >& Inheritance_Mapping_push_back_mappings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->mappings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Concrete::Concrete_Context > >& Inheritance_ObjectMapping_insert_inheritance_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->inheritance_contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_User > >& Inheritance_ObjectMapping_insert_inheritance_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->inheritance_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& Inheritance_ObjectMapping_insert_inheritance_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->inheritance_links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_AbstractDef > >& Interface_Abstract_push_back_abstract_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->abstract_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_LinkTrait > > >& Interface_Abstract_push_back_link_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->link_traits;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->link_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_RequirementTrait > > >& Interface_Abstract_push_back_requirement_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->requirement_traits;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& Interface_ArgumentListTrait_push_back_canonical_types(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_ArgumentListTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_ArgumentListTrait >( m_data )->canonical_types;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& Interface_ContextGroup_push_back_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Root::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& Interface_DimensionTrait_push_back_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_DimensionTrait >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& Interface_DimensionTrait_push_back_symbols(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->symbols;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& Interface_EventTypeTrait_push_back_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_EventTypeTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_EventTypeTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_EventDef > >& Interface_Event_push_back_event_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Event >( m_data )->event_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_FunctionDef > >& Interface_Function_push_back_function_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Function >( m_data )->function_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Context > >& Interface_IContext_push_back_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& Interface_InheritanceTrait_push_back_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_InheritanceTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_InheritanceTrait >( m_data )->contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_InteruptDef > >& Interface_Interupt_push_back_interupt_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Interupt >( m_data )->interupt_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Locations::Interface_InvocationInstance > >& Interface_InvocationContext_push_back_invocation_instances(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& Interface_LinkTrait_push_back_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > > >& Interface_Namespace_push_back_dimension_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Namespace >( m_data )->dimension_traits;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->dimension_traits;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Event >( m_data )->dimension_traits;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->dimension_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_ContextDef > >& Interface_Namespace_push_back_namespace_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Namespace >( m_data )->namespace_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_ObjectDef > >& Interface_Object_push_back_object_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->object_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& Interface_RequirementTrait_push_back_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_RequirementTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_RequirementTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_PartTrait > > >& Interface_State_push_back_part_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_StateDef > >& Interface_State_push_back_state_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& Interface_TransitionTypeTrait_push_back_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_TransitionTypeTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TransitionTypeTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePath > >& Interface_TypePathVariant_push_back_sequence(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Clang::Interface_TypePathVariant::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypePathVariant >( m_data )->sequence;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& Interface_TypePath_push_back_types(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Clang::Interface_TypePath::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypePath >( m_data )->types;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& Interface_TypedLinkTrait_push_back_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypedLinkTrait >( m_data )->tuple;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypedLinkTrait >( m_data )->tuple;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypedLinkTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Invocations_Instructions_Instruction > >& Invocations_Instructions_InstructionGroup_push_back_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_InstructionGroup::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_Root::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_ParentDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_ChildDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_PolyBranch::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_PolyCase::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Graph_Vertex > >& Invocations_Variables_Reference_push_back_types(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Variables_Reference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Reference >( m_data )->types;
        case data::Operations::Invocations_Variables_Memory::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Reference >( m_data )->types;
        case data::Operations::Invocations_Variables_Parameter::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Reference >( m_data )->types;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::MemoryLayout_Part > >& MemoryLayout_Buffer_push_back_parts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Buffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        case data::MemoryLayout::MemoryLayout_NonSimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        case data::MemoryLayout::MemoryLayout_GPUBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Object > >& MemoryLayout_MemoryMap_push_back_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryLayout::MemoryLayout_MemoryMap::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocation > >& MemoryLayout_Part_push_back_allocation_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->allocation_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& MemoryLayout_Part_push_back_link_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->link_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& MemoryLayout_Part_push_back_user_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->user_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Invocations_Operations_Operation > >& Operations_Invocation_push_back_operations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Invocations_Variables_Variable > >& Operations_Invocation_push_back_variables(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::InvocationID, data::Ptr< data::Operations::Operations_Invocation > >& Operations_Invocations_insert_invocations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocations::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocations >( m_data )->invocations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::TypeName >& Parser_ArgumentList_push_back_args(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ArgumentList::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ArgumentList >( m_data )->args;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_ContextDef > >& Parser_ContextDef_push_back_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Dependency > >& Parser_ContextDef_push_back_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Dimension > >& Parser_ContextDef_push_back_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Include > >& Parser_ContextDef_push_back_includes(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Link > >& Parser_ContextDef_push_back_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Part > >& Parser_ContextDef_push_back_parts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Requirement > >& Parser_ContextDef_push_back_requirements(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& Parser_Inheritance_push_back_strings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Inheritance::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Inheritance >( m_data )->strings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& Parser_Part_push_back_identifiers(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Part::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Part >( m_data )->identifiers;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Identifier > >& Parser_ScopedIdentifier_push_back_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ScopedIdentifier::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ScopedIdentifier >( m_data )->ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::Type >& Parser_TypeList_push_back_args(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_TypeList::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_TypeList >( m_data )->args;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& Symbols_SymbolTable_insert_concrete_type_id_seq_alloc(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_id_seq_alloc;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& Symbols_SymbolTable_insert_concrete_type_id_sequences(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_id_sequences;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& Symbols_SymbolTable_insert_concrete_type_id_set_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_id_set_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeID, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& Symbols_SymbolTable_insert_concrete_type_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::SymbolTable::Symbols_InterfaceTypeID > >& Symbols_SymbolTable_insert_interface_type_id_sequences(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->interface_type_id_sequences;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeID, data::Ptr< data::SymbolTable::Symbols_InterfaceTypeID > >& Symbols_SymbolTable_insert_interface_type_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->interface_type_ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< std::string, data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& Symbols_SymbolTable_insert_symbol_names(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->symbol_names;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeID, data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& Symbols_SymbolTable_insert_symbol_type_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->symbol_type_ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& Symbols_SymbolTypeID_push_back_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > >& Symbols_SymbolTypeID_push_back_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_LinkTrait > >& Symbols_SymbolTypeID_push_back_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< data::Ptr< data::Concrete::Concrete_Dimensions_User >, data::Ptr< data::UnityAnalysis::UnityAnalysis_DataBinding > >& UnityAnalysis_Binding_insert_dataBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Binding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Binding >( m_data )->dataBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< data::Ptr< data::Concrete::Concrete_Dimensions_Link >, data::Ptr< data::UnityAnalysis::UnityAnalysis_LinkBinding > >& UnityAnalysis_Binding_insert_linkBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Binding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Binding >( m_data )->linkBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::UnityAnalysis::UnityAnalysis_DataBinding > >& UnityAnalysis_ObjectBinding_push_back_dataBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_ObjectBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->dataBindings;
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->dataBindings;
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->dataBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::UnityAnalysis::UnityAnalysis_LinkBinding > >& UnityAnalysis_ObjectBinding_push_back_linkBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_ObjectBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->linkBindings;
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->linkBindings;
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->linkBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& get_Alias_AliasDerivation_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_AliasDerivation::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_AliasDerivation >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Alias_Edge > >& get_Alias_AliasDerivation_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_AliasDerivation::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_AliasDerivation >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& get_Alias_Edge_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_Edge::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Edge >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Model::Alias_Step >& get_Alias_Edge_next(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_Edge::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Edge >( m_data )->next;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Alias_Edge > >& get_Alias_Step_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_Step::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->edges;
        case data::Model::Alias_And::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->edges;
        case data::Model::Alias_Or::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& get_Alias_Step_vertex(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_Step::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->vertex;
        case data::Model::Alias_And::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->vertex;
        case data::Model::Alias_Or::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->vertex;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& get_Allocators_Allocator_allocated_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Allocators_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_Nothing::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_Singleton::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_Range::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_Range32::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_Range64::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_RangeAny::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::optional< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocator > > >& get_Allocators_Allocator_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Allocators_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_Nothing::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_Singleton::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_Range::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_Range32::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_Range64::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_RangeAny::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Context > >& get_Allocators_Allocator_parent_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Allocators_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_Nothing::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_Singleton::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_Range::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_Range32::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_Range64::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_RangeAny::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& get_Components_Component_build_dir(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->build_dir;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& get_Components_Component_cpp_defines(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->cpp_defines;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& get_Components_Component_cpp_flags(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->cpp_flags;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::cppFilePath >& get_Components_Component_cpp_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->cpp_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& get_Components_Component_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::io::ComponentFilePath& get_Components_Component_file_path(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->file_path;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< boost::filesystem::path >& get_Components_Component_include_directories(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->include_directories;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::io::ComponentFilePath& get_Components_Component_init_file_path(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->init_file_path;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& get_Components_Component_mega_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->mega_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Components_Component_name(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->name;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::io::ComponentFilePath& get_Components_Component_python_file_path(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->python_file_path;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::schFilePath >& get_Components_Component_sch_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->sch_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& get_Components_Component_src_dir(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->src_dir;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::ComponentType& get_Components_Component_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Action >& get_Concrete_Action_interface_action(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Action >( m_data )->interface_action;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Component >& get_Concrete_Component_interface_component(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Component >( m_data )->interface_component;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Context > >& get_Concrete_ContextGroup_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocator > >& get_Concrete_Context_allocation_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::MemoryLayout::Allocators_Allocator >& get_Concrete_Context_allocator(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Components::Components_Component >& get_Concrete_Context_component(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::optional< data::Ptr< data::Concrete::Concrete_Object > > >& get_Concrete_Context_concrete_object(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& get_Concrete_Context_inheritance(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& get_Concrete_Context_interface(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_ContextGroup >& get_Concrete_Context_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Concrete_Dimensions_Allocation_concrete_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Concrete_Dimensions_Allocation::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Allocation >( m_data )->concrete_id;
        case data::MemoryLayout::Concrete_Dimensions_Allocator::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Allocation >( m_data )->concrete_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::U64 >& get_Concrete_Dimensions_Allocation_offset(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Concrete_Dimensions_Allocation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->offset;
        case data::MemoryLayout::Concrete_Dimensions_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->offset;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& get_Concrete_Dimensions_Allocation_parent_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Concrete_Dimensions_Allocation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->parent_context;
        case data::MemoryLayout::Concrete_Dimensions_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->parent_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::MemoryLayout::MemoryLayout_Part > >& get_Concrete_Dimensions_Allocation_part(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Concrete_Dimensions_Allocation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->part;
        case data::MemoryLayout::Concrete_Dimensions_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->part;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::MemoryLayout::Allocators_Allocator >& get_Concrete_Dimensions_Allocator_allocator(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Concrete_Dimensions_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocator >( m_data )->allocator;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Concrete_Dimensions_Link_singular(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->singular;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->singular;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->singular;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Clang::Interface_TypedLinkTrait >& get_Concrete_Dimensions_UserLink_interface_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_UserLink >( m_data )->interface_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Concrete_Dimensions_User_concrete_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_User >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Link >( m_data )->concrete_id;
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Link >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Link >( m_data )->concrete_id;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_DimensionTrait >& get_Concrete_Dimensions_User_interface_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_User >( m_data )->interface_dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Concrete_Dimensions_User_offset(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_User >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->offset;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& get_Concrete_Dimensions_User_parent_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_User >( m_data )->parent_context;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->parent_context;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->parent_context;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->parent_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::MemoryLayout::MemoryLayout_Part >& get_Concrete_Dimensions_User_part(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_User >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->part;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Event >& get_Concrete_Event_interface_event(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Event >( m_data )->interface_event;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Function >& get_Concrete_Function_interface_function(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Function >( m_data )->interface_function;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& get_Concrete_Graph_Edge_source(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Concrete_Graph_Edge::Object_Part_Type_ID:
            return data::convert< data::Model::Concrete_Graph_Edge >( m_data )->source;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& get_Concrete_Graph_Edge_target(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Concrete_Graph_Edge::Object_Part_Type_ID:
            return data::convert< data::Model::Concrete_Graph_Edge >( m_data )->target;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::EdgeType& get_Concrete_Graph_Edge_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Concrete_Graph_Edge::Object_Part_Type_ID:
            return data::convert< data::Model::Concrete_Graph_Edge >( m_data )->type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& get_Concrete_Graph_Vertex_in_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Graph_Vertex::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& get_Concrete_Graph_Vertex_out_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Graph_Vertex::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Interupt >& get_Concrete_Interupt_interface_interupt(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Interupt >( m_data )->interface_interupt;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >& get_Concrete_MemoryMappedObject_memory_map(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryRollout::Concrete_MemoryMappedObject >( m_data )->memory_map;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Namespace >& get_Concrete_Namespace_interface_namespace(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Namespace >( m_data )->interface_namespace;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::MemoryLayout_Buffer > >& get_Concrete_Object_buffers(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Object >( m_data )->buffers;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Object >( m_data )->buffers;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Object >& get_Concrete_Object_interface_object(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->interface_object;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->interface_object;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_UserDimensionContext > >& get_Concrete_Object_link_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Object >( m_data )->link_contexts;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Object >( m_data )->link_contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink > >& get_Concrete_Object_ownership_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->ownership_link;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->ownership_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Root >& get_Concrete_Root_interface_root(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Root >( m_data )->interface_root;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_State >& get_Concrete_State_interface_state(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_State >( m_data )->interface_state;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_State >( m_data )->interface_state;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_State >( m_data )->interface_state;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Concrete_State_local_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->local_size;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Event >( m_data )->local_size;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->local_size;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->local_size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Concrete_State_total_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->total_size;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Event >( m_data )->total_size;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->total_size;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->total_size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& get_Concrete_UserDimensionContext_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& get_Concrete_UserDimensionContext_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::io::cppFilePath, data::Ptr< data::DPGraph::Dependencies_TransitiveDependencies > >& get_Dependencies_Analysis_cpp_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->cpp_dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::io::megaFilePath, data::Ptr< data::DPGraph::Dependencies_TransitiveDependencies > >& get_Dependencies_Analysis_mega_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->mega_dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::DPGraph::Dependencies_SourceFileDependencies > >& get_Dependencies_Analysis_objects(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->objects;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& get_Dependencies_Analysis_topological_mega_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->topological_mega_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Dependencies_Glob_glob(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Glob::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Glob >( m_data )->glob;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& get_Dependencies_Glob_location(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Glob::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Glob >( m_data )->location;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::DPGraph::Dependencies_Glob > >& get_Dependencies_SourceFileDependencies_globs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_SourceFileDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_SourceFileDependencies >( m_data )->globs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Dependencies_SourceFileDependencies_hash_code(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_SourceFileDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_SourceFileDependencies >( m_data )->hash_code;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< boost::filesystem::path >& get_Dependencies_SourceFileDependencies_resolution(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_SourceFileDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_SourceFileDependencies >( m_data )->resolution;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::io::megaFilePath& get_Dependencies_SourceFileDependencies_source_file(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_SourceFileDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_SourceFileDependencies >( m_data )->source_file;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& get_Dependencies_TransitiveDependencies_mega_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_TransitiveDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_TransitiveDependencies >( m_data )->mega_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& get_Derivation_Edge_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Edge::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Edge >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Derivation_Edge_eliminated(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Edge::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Edge >( m_data )->eliminated;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Derivation_Step >& get_Derivation_Edge_next(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Edge::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Edge >( m_data )->next;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Graph_Vertex > >& get_Derivation_Root_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Root::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Root >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Derivation_Edge > >& get_Derivation_Root_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Root::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Root >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Derivation_Edge > >& get_Derivation_Step_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Step::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->edges;
        case data::Operations::Derivation_And::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->edges;
        case data::Operations::Derivation_Or::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& get_Derivation_Step_vertex(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Step::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->vertex;
        case data::Operations::Derivation_And::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->vertex;
        case data::Operations::Derivation_Or::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->vertex;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< data::Ptr< data::Clang::Interface_ObjectLinkTrait >, data::Ptr< data::Model::HyperGraph_Relation > >& get_HyperGraph_Graph_relations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_Graph::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Graph >( m_data )->relations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Clang::Interface_ObjectLinkTrait >& get_HyperGraph_NonOwningObjectRelation_source(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_NonOwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_NonOwningObjectRelation >( m_data )->source;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Clang::Interface_ObjectLinkTrait >& get_HyperGraph_NonOwningObjectRelation_target(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_NonOwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_NonOwningObjectRelation >( m_data )->target;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& get_HyperGraph_OwningObjectRelation_owned(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_OwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_OwningObjectRelation >( m_data )->owned;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_ObjectLinkTrait > >& get_HyperGraph_OwningObjectRelation_owners(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_OwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_OwningObjectRelation >( m_data )->owners;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::RelationID& get_HyperGraph_Relation_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_Relation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Relation >( m_data )->id;
        case data::Model::HyperGraph_ObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Relation >( m_data )->id;
        case data::Model::HyperGraph_OwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Relation >( m_data )->id;
        case data::Model::HyperGraph_NonOwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Relation >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Concrete::Concrete_Context > >& get_Inheritance_Mapping_inheritance_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->inheritance_contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_User > >& get_Inheritance_Mapping_inheritance_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->inheritance_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& get_Inheritance_Mapping_inheritance_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->inheritance_links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Derivations::Inheritance_ObjectMapping > >& get_Inheritance_Mapping_mappings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->mappings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Inheritance_ObjectMapping_hash_code(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->hash_code;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Concrete::Concrete_Context > >& get_Inheritance_ObjectMapping_inheritance_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->inheritance_contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_User > >& get_Inheritance_ObjectMapping_inheritance_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->inheritance_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& get_Inheritance_ObjectMapping_inheritance_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->inheritance_links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::io::megaFilePath& get_Inheritance_ObjectMapping_source_file(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->source_file;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_AbstractDef > >& get_Interface_Abstract_abstract_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->abstract_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::optional< data::Ptr< data::Tree::Interface_InheritanceTrait > > >& get_Interface_Abstract_inheritance_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->inheritance_trait;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->inheritance_trait;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Event >( m_data )->inheritance_trait;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->inheritance_trait;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->inheritance_trait;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->inheritance_trait;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->inheritance_trait;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->inheritance_trait;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->inheritance_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_LinkTrait > > >& get_Interface_Abstract_link_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->link_traits;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->link_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_RequirementTrait > > >& get_Interface_Abstract_requirement_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->requirement_traits;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::optional< data::Ptr< data::Tree::Interface_SizeTrait > > >& get_Interface_Abstract_size_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->size_trait;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->size_trait;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Event >( m_data )->size_trait;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->size_trait;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->size_trait;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->size_trait;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->size_trait;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->size_trait;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->size_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& get_Interface_ArgumentListTrait_canonical_types(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_ArgumentListTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_ArgumentListTrait >( m_data )->canonical_types;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& get_Interface_ContextGroup_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Root::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Interface_DimensionTrait_alignment(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->alignment;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Interface_DimensionTrait_canonical_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->canonical_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& get_Interface_DimensionTrait_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_DimensionTrait >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Interface_DimensionTrait_erased_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->erased_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Interface_DimensionTrait_interface_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_DimensionTrait >( m_data )->interface_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Components::MegaMangle_Mangle >& get_Interface_DimensionTrait_mangle(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->mangle;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& get_Interface_DimensionTrait_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_DimensionTrait >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Interface_DimensionTrait_simple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->simple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Interface_DimensionTrait_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Interface_DimensionTrait_symbol_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_DimensionTrait >( m_data )->symbol_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& get_Interface_DimensionTrait_symbols(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->symbols;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& get_Interface_EventTypeTrait_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_EventTypeTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_EventTypeTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_EventDef > >& get_Interface_Event_event_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Event >( m_data )->event_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_ArgumentListTrait > >& get_Interface_Function_arguments_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Function >( m_data )->arguments_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_FunctionDef > >& get_Interface_Function_function_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Function >( m_data )->function_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_ReturnTypeTrait > >& get_Interface_Function_return_type_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Function >( m_data )->return_type_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Components::Components_Component >& get_Interface_IContext_component(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Context > >& get_Interface_IContext_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Interface_IContext_identifier(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Interface_IContext_interface_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_ContextGroup >& get_Interface_IContext_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Interface_IContext_symbol_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& get_Interface_InheritanceTrait_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_InheritanceTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_InheritanceTrait >( m_data )->contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_EventTypeTrait > >& get_Interface_Interupt_events_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Interupt >( m_data )->events_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_InteruptDef > >& get_Interface_Interupt_interupt_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Interupt >( m_data )->interupt_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Locations::Interface_InvocationInstance > >& get_Interface_InvocationContext_invocation_instances(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::SourceLocation& get_Interface_InvocationContext_source_location(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Operations_Invocation >& get_Interface_InvocationInstance_invocation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Locations::Interface_InvocationInstance::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationInstance >( m_data )->invocation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::SourceLocation& get_Interface_InvocationInstance_source_location(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Locations::Interface_InvocationInstance::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationInstance >( m_data )->source_location;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& get_Interface_LinkTrait_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Interface_LinkTrait_interface_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->interface_id;
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->interface_id;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->interface_id;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->interface_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& get_Interface_LinkTrait_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_LinkTrait >( m_data )->parent;
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_LinkTrait >( m_data )->parent;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_LinkTrait >( m_data )->parent;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_LinkTrait >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Interface_LinkTrait_symbol_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->symbol_id;
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->symbol_id;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->symbol_id;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->symbol_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > > >& get_Interface_Namespace_dimension_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Namespace >( m_data )->dimension_traits;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->dimension_traits;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Event >( m_data )->dimension_traits;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->dimension_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Interface_Namespace_is_global(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Namespace >( m_data )->is_global;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_ContextDef > >& get_Interface_Namespace_namespace_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Namespace >( m_data )->namespace_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Model::HyperGraph_Relation >& get_Interface_ObjectLinkTrait_relation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Interface_ObjectLinkTrait >( m_data )->relation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_ObjectDef > >& get_Interface_Object_object_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->object_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& get_Interface_PartTrait_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_PartTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_PartTrait >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& get_Interface_RequirementTrait_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_RequirementTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_RequirementTrait >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& get_Interface_RequirementTrait_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_RequirementTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_RequirementTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Interface_ReturnTypeTrait_canonical_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_ReturnTypeTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_ReturnTypeTrait >( m_data )->canonical_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ObjectSourceRoot >& get_Interface_Root_root(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Root::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Root >( m_data )->root;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Interface_SizeTrait_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_SizeTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_SizeTrait >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_PartTrait > > >& get_Interface_State_part_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_StateDef > >& get_Interface_State_state_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::optional< data::Ptr< data::Tree::Interface_TransitionTypeTrait > > >& get_Interface_State_transition_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->transition_trait;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Interupt >( m_data )->transition_trait;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->transition_trait;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->transition_trait;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->transition_trait;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->transition_trait;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->transition_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& get_Interface_TransitionTypeTrait_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_TransitionTypeTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TransitionTypeTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePath > >& get_Interface_TypePathVariant_sequence(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Clang::Interface_TypePathVariant::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypePathVariant >( m_data )->sequence;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& get_Interface_TypePath_types(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Clang::Interface_TypePath::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypePath >( m_data )->types;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& get_Interface_TypedLinkTrait_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypedLinkTrait >( m_data )->tuple;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypedLinkTrait >( m_data )->tuple;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypedLinkTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Invocations_Instructions_Instruction > >& get_Invocations_Instructions_InstructionGroup_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_InstructionGroup::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_Root::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_ParentDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_ChildDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_PolyBranch::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_PolyCase::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Variable >& get_Invocations_Instructions_ParentDerivation_from(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_ParentDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_ParentDerivation >( m_data )->from;
        case data::Operations::Invocations_Instructions_ChildDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_ChildDerivation >( m_data )->from;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Stack >& get_Invocations_Instructions_ParentDerivation_to(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_ParentDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_ParentDerivation >( m_data )->to;
        case data::Operations::Invocations_Instructions_ChildDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_ChildDerivation >( m_data )->to;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Reference >& get_Invocations_Instructions_PolyBranch_from_reference(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_PolyBranch::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_PolyBranch >( m_data )->from_reference;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Reference >& get_Invocations_Instructions_PolyCase_reference(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_PolyCase::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_PolyCase >( m_data )->reference;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& get_Invocations_Instructions_PolyCase_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_PolyCase::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_PolyCase >( m_data )->type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Parameter >& get_Invocations_Instructions_Root_parameter(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_Root::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_Root >( m_data )->parameter;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Variable >& get_Invocations_Operations_Operation_variable(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Operations_Operation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Operations_Operation >( m_data )->variable;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Graph_Vertex > >& get_Invocations_Variables_Reference_types(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Variables_Reference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Reference >( m_data )->types;
        case data::Operations::Invocations_Variables_Memory::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Reference >( m_data )->types;
        case data::Operations::Invocations_Variables_Parameter::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Reference >( m_data )->types;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& get_Invocations_Variables_Stack_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Variables_Stack::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Stack >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Operations::Invocations_Variables_Variable > >& get_Invocations_Variables_Variable_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Variables_Variable::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Variable >( m_data )->parent;
        case data::Operations::Invocations_Variables_Stack::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Variable >( m_data )->parent;
        case data::Operations::Invocations_Variables_Reference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Variable >( m_data )->parent;
        case data::Operations::Invocations_Variables_Memory::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Variable >( m_data )->parent;
        case data::Operations::Invocations_Variables_Parameter::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Variable >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_MegaMangle_Mangle_blit(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::MegaMangle_Mangle::Object_Part_Type_ID:
            return data::convert< data::Components::MegaMangle_Mangle >( m_data )->blit;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_MegaMangle_Mangle_canon(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::MegaMangle_Mangle::Object_Part_Type_ID:
            return data::convert< data::Components::MegaMangle_Mangle >( m_data )->canon;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_MegaMangle_Mangle_impl(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::MegaMangle_Mangle::Object_Part_Type_ID:
            return data::convert< data::Components::MegaMangle_Mangle >( m_data )->impl;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_MegaMangle_Mangle_mangle(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::MegaMangle_Mangle::Object_Part_Type_ID:
            return data::convert< data::Components::MegaMangle_Mangle >( m_data )->mangle;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_MemoryLayout_Buffer_alignment(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Buffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->alignment;
        case data::MemoryLayout::MemoryLayout_NonSimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->alignment;
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->alignment;
        case data::MemoryLayout::MemoryLayout_GPUBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->alignment;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::MemoryLayout_Part > >& get_MemoryLayout_Buffer_parts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Buffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        case data::MemoryLayout::MemoryLayout_NonSimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        case data::MemoryLayout::MemoryLayout_GPUBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_MemoryLayout_Buffer_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Buffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->size;
        case data::MemoryLayout::MemoryLayout_NonSimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->size;
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->size;
        case data::MemoryLayout::MemoryLayout_GPUBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_MemoryLayout_MemoryMap_block_alignment(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryLayout::MemoryLayout_MemoryMap::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( m_data )->block_alignment;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_MemoryLayout_MemoryMap_block_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryLayout::MemoryLayout_MemoryMap::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( m_data )->block_size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Object > >& get_MemoryLayout_MemoryMap_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryLayout::MemoryLayout_MemoryMap::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_MemoryLayout_MemoryMap_fixed_allocation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryLayout::MemoryLayout_MemoryMap::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( m_data )->fixed_allocation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& get_MemoryLayout_MemoryMap_interface(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryLayout::MemoryLayout_MemoryMap::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( m_data )->interface;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::U64 >& get_MemoryLayout_Part_alignment(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->alignment;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocation > >& get_MemoryLayout_Part_allocation_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->allocation_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::MemoryLayout::MemoryLayout_Buffer > >& get_MemoryLayout_Part_buffer(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->buffer;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& get_MemoryLayout_Part_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& get_MemoryLayout_Part_link_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->link_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::U64 >& get_MemoryLayout_Part_offset(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->offset;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::U64 >& get_MemoryLayout_Part_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_MemoryLayout_Part_total_domain_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->total_domain_size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& get_MemoryLayout_Part_user_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->user_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Derivation_Root >& get_Operations_Invocation_derivation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::InvocationID& get_Operations_Invocation_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Invocations_Operations_Operation > >& get_Operations_Invocation_operations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Operations::Invocations_Instructions_Root > >& get_Operations_Invocation_root_instruction(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Invocations_Variables_Variable > >& get_Operations_Invocation_variables(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::InvocationID, data::Ptr< data::Operations::Operations_Invocation > >& get_Operations_Invocations_invocations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocations::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocations >( m_data )->invocations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Inheritance >& get_Parser_AbstractDef_inheritance(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_AbstractDef >( m_data )->inheritance;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->inheritance;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_EventDef >( m_data )->inheritance;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ObjectDef >( m_data )->inheritance;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->inheritance;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->inheritance;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Size >& get_Parser_AbstractDef_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_AbstractDef >( m_data )->size;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->size;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_EventDef >( m_data )->size;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ObjectDef >( m_data )->size;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->size;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::TypeName >& get_Parser_ArgumentList_args(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ArgumentList::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ArgumentList >( m_data )->args;
        case data::Tree::Interface_ArgumentListTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ArgumentList >( m_data )->args;
        case data::Tree::Interface_EventTypeTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ArgumentList >( m_data )->args;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& get_Parser_CPPInclude_cppSourceFilePath(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_CPPInclude::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_CPPInclude >( m_data )->cppSourceFilePath;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Parser_ContextDef_body(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_ContextDef > >& get_Parser_ContextDef_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Dependency > >& get_Parser_ContextDef_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Dimension > >& get_Parser_ContextDef_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ScopedIdentifier >& get_Parser_ContextDef_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Include > >& get_Parser_ContextDef_includes(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Link > >& get_Parser_ContextDef_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Part > >& get_Parser_ContextDef_parts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Requirement > >& get_Parser_ContextDef_requirements(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Parser_Dependency_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Dependency::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dependency >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Identifier >& get_Parser_Dimension_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Dimension::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->id;
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::AST::Parser_Initialiser > >& get_Parser_Dimension_initialiser(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Dimension::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->initialiser;
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->initialiser;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Parser_Dimension_isConst(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Dimension::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->isConst;
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->isConst;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Parser_Dimension_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Dimension::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->type;
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ReturnType >& get_Parser_FunctionDef_returnType(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_FunctionDef >( m_data )->returnType;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Parser_Identifier_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Identifier::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Identifier >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& get_Parser_Inheritance_strings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Inheritance::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Inheritance >( m_data )->strings;
        case data::Tree::Interface_InheritanceTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Inheritance >( m_data )->strings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Parser_Initialiser_initialiser(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Initialiser::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Initialiser >( m_data )->initialiser;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Parser_Initialiser_source_loc_end(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Initialiser::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Initialiser >( m_data )->source_loc_end;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Parser_Initialiser_source_loc_start(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Initialiser::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Initialiser >( m_data )->source_loc_start;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ArgumentList >& get_Parser_InteruptDef_argumentList(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_InteruptDef >( m_data )->argumentList;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_FunctionDef >( m_data )->argumentList;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::CardinalityRange >& get_Parser_Link_cardinality(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Link::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->cardinality;
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->cardinality;
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->cardinality;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->cardinality;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->cardinality;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Identifier >& get_Parser_Link_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Link::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->id;
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->id;
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->id;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->id;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Parser_Link_owning(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Link::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->owning;
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->owning;
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->owning;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->owning;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->owning;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_TypeList >& get_Parser_Link_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Link::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->type;
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->type;
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->type;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->type;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ScopedIdentifier >& get_Parser_MegaIncludeNested_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_MegaIncludeNested::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaIncludeNested >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& get_Parser_MegaInclude_megaSourceFilePath(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_MegaInclude::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->megaSourceFilePath;
        case data::AST::Parser_MegaIncludeInline::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->megaSourceFilePath;
        case data::AST::Parser_MegaIncludeNested::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->megaSourceFilePath;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::AST::Parser_IncludeRoot > >& get_Parser_MegaInclude_root(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_MegaInclude::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->root;
        case data::AST::Parser_MegaIncludeInline::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->root;
        case data::AST::Parser_MegaIncludeNested::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->root;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& get_Parser_Part_identifiers(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Part::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Part >( m_data )->identifiers;
        case data::Tree::Interface_PartTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Part >( m_data )->identifiers;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_TypeList >& get_Parser_Requirement_argumentList(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Requirement::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Requirement >( m_data )->argumentList;
        case data::Tree::Interface_RequirementTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Requirement >( m_data )->argumentList;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Identifier >& get_Parser_Requirement_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Requirement::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Requirement >( m_data )->id;
        case data::Tree::Interface_RequirementTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Requirement >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Parser_ReturnType_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ReturnType::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ReturnType >( m_data )->str;
        case data::Tree::Interface_ReturnTypeTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ReturnType >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Identifier > >& get_Parser_ScopedIdentifier_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ScopedIdentifier::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ScopedIdentifier >( m_data )->ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Parser_ScopedIdentifier_line_number(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ScopedIdentifier::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ScopedIdentifier >( m_data )->line_number;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Parser_ScopedIdentifier_source_file(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ScopedIdentifier::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ScopedIdentifier >( m_data )->source_file;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Parser_Size_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Size::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Size >( m_data )->str;
        case data::Tree::Interface_SizeTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Size >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ContextDef >& get_Parser_SourceRoot_ast(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_SourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->ast;
        case data::AST::Parser_IncludeRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->ast;
        case data::AST::Parser_ObjectSourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->ast;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Components::Components_Component >& get_Parser_SourceRoot_component(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_SourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->component;
        case data::AST::Parser_IncludeRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->component;
        case data::AST::Parser_ObjectSourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->component;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& get_Parser_SourceRoot_sourceFile(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_SourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->sourceFile;
        case data::AST::Parser_IncludeRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->sourceFile;
        case data::AST::Parser_ObjectSourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->sourceFile;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Transition >& get_Parser_StateDef_transition(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->transition;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_InteruptDef >( m_data )->transition;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->transition;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->transition;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Parser_SystemInclude_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_SystemInclude::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SystemInclude >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Parser_Transition_is_predecessor(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Transition::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Transition >( m_data )->is_predecessor;
        case data::Tree::Interface_TransitionTypeTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Transition >( m_data )->is_predecessor;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Parser_Transition_is_successor(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Transition::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Transition >( m_data )->is_successor;
        case data::Tree::Interface_TransitionTypeTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Transition >( m_data )->is_successor;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Parser_Transition_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Transition::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Transition >( m_data )->str;
        case data::Tree::Interface_TransitionTypeTrait::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Transition >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::Type >& get_Parser_TypeList_args(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_TypeList::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_TypeList >( m_data )->args;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Context > >& get_Symbols_ConcreteTypeID_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::ConcreteTable::Symbols_ConcreteTypeID::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_ConcreteTypeID >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocation > >& get_Symbols_ConcreteTypeID_dim_allocation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::ConcreteTable::Symbols_ConcreteTypeID::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_ConcreteTypeID >( m_data )->dim_allocation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& get_Symbols_ConcreteTypeID_dim_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::ConcreteTable::Symbols_ConcreteTypeID::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_ConcreteTypeID >( m_data )->dim_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& get_Symbols_ConcreteTypeID_dim_user(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::ConcreteTable::Symbols_ConcreteTypeID::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_ConcreteTypeID >( m_data )->dim_user;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Symbols_ConcreteTypeID_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::ConcreteTable::Symbols_ConcreteTypeID::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_ConcreteTypeID >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_IContext > >& get_Symbols_InterfaceTypeID_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_InterfaceTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_InterfaceTypeID >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_DimensionTrait > >& get_Symbols_InterfaceTypeID_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_InterfaceTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_InterfaceTypeID >( m_data )->dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Symbols_InterfaceTypeID_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_InterfaceTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_InterfaceTypeID >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_LinkTrait > >& get_Symbols_InterfaceTypeID_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_InterfaceTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_InterfaceTypeID >( m_data )->link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeIDSequence& get_Symbols_InterfaceTypeID_symbol_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_InterfaceTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_InterfaceTypeID >( m_data )->symbol_ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& get_Symbols_SymbolTable_concrete_type_id_seq_alloc(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_id_seq_alloc;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& get_Symbols_SymbolTable_concrete_type_id_sequences(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_id_sequences;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& get_Symbols_SymbolTable_concrete_type_id_set_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_id_set_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeID, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& get_Symbols_SymbolTable_concrete_type_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::SymbolTable::Symbols_InterfaceTypeID > >& get_Symbols_SymbolTable_interface_type_id_sequences(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->interface_type_id_sequences;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeID, data::Ptr< data::SymbolTable::Symbols_InterfaceTypeID > >& get_Symbols_SymbolTable_interface_type_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->interface_type_ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< std::string, data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& get_Symbols_SymbolTable_symbol_names(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->symbol_names;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeID, data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& get_Symbols_SymbolTable_symbol_type_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->symbol_type_ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& get_Symbols_SymbolTypeID_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > >& get_Symbols_SymbolTypeID_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Symbols_SymbolTypeID_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_LinkTrait > >& get_Symbols_SymbolTypeID_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_Symbols_SymbolTypeID_symbol(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->symbol;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::UnityAnalysis::UnityAnalysis_ObjectBinding >& get_UnityAnalysis_Binding_binding(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Binding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Binding >( m_data )->binding;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< data::Ptr< data::Concrete::Concrete_Dimensions_User >, data::Ptr< data::UnityAnalysis::UnityAnalysis_DataBinding > >& get_UnityAnalysis_Binding_dataBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Binding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Binding >( m_data )->dataBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< data::Ptr< data::Concrete::Concrete_Dimensions_Link >, data::Ptr< data::UnityAnalysis::UnityAnalysis_LinkBinding > >& get_UnityAnalysis_Binding_linkBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Binding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Binding >( m_data )->linkBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Object >& get_UnityAnalysis_Binding_object(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Binding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Binding >( m_data )->object;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_UnityAnalysis_DataBinding_interfaceTypeID(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_DataBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_DataBinding >( m_data )->interfaceTypeID;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_UnityAnalysis_DataBinding_typeName(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_DataBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_DataBinding >( m_data )->typeName;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_UnityAnalysis_LinkBinding_interfaceTypeID(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_LinkBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_LinkBinding >( m_data )->interfaceTypeID;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_UnityAnalysis_LinkBinding_typeName(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_LinkBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_LinkBinding >( m_data )->typeName;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_UnityAnalysis_Manual_name(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Manual >( m_data )->name;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::UnityAnalysis::UnityAnalysis_DataBinding > >& get_UnityAnalysis_ObjectBinding_dataBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_ObjectBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->dataBindings;
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->dataBindings;
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->dataBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_UnityAnalysis_ObjectBinding_interfaceTypeID(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_ObjectBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->interfaceTypeID;
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->interfaceTypeID;
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->interfaceTypeID;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::UnityAnalysis::UnityAnalysis_LinkBinding > >& get_UnityAnalysis_ObjectBinding_linkBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_ObjectBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->linkBindings;
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->linkBindings;
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->linkBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_UnityAnalysis_ObjectBinding_typeName(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_ObjectBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->typeName;
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->typeName;
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->typeName;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& get_UnityAnalysis_Prefab_guid(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Prefab >( m_data )->guid;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& set_Alias_AliasDerivation_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_AliasDerivation::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_AliasDerivation >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Alias_Edge > >& set_Alias_AliasDerivation_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_AliasDerivation::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_AliasDerivation >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& set_Alias_Edge_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_Edge::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Edge >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Model::Alias_Step >& set_Alias_Edge_next(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_Edge::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Edge >( m_data )->next;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Alias_Edge > >& set_Alias_Step_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_Step::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->edges;
        case data::Model::Alias_And::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->edges;
        case data::Model::Alias_Or::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& set_Alias_Step_vertex(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Alias_Step::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->vertex;
        case data::Model::Alias_And::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->vertex;
        case data::Model::Alias_Or::Object_Part_Type_ID:
            return data::convert< data::Model::Alias_Step >( m_data )->vertex;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& set_Allocators_Allocator_allocated_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Allocators_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_Nothing::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_Singleton::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_Range::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_Range32::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_Range64::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        case data::MemoryLayout::Allocators_RangeAny::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->allocated_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::optional< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocator > > >& set_Allocators_Allocator_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Allocators_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_Nothing::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_Singleton::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_Range::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_Range32::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_Range64::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        case data::MemoryLayout::Allocators_RangeAny::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Context > >& set_Allocators_Allocator_parent_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Allocators_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_Nothing::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_Singleton::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_Range::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_Range32::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_Range64::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        case data::MemoryLayout::Allocators_RangeAny::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Allocators_Allocator >( m_data )->parent_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& set_Components_Component_build_dir(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->build_dir;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& set_Components_Component_cpp_defines(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->cpp_defines;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& set_Components_Component_cpp_flags(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->cpp_flags;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::cppFilePath >& set_Components_Component_cpp_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->cpp_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& set_Components_Component_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::io::ComponentFilePath& set_Components_Component_file_path(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->file_path;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< boost::filesystem::path >& set_Components_Component_include_directories(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->include_directories;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::io::ComponentFilePath& set_Components_Component_init_file_path(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->init_file_path;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& set_Components_Component_mega_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->mega_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Components_Component_name(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->name;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::io::ComponentFilePath& set_Components_Component_python_file_path(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->python_file_path;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::schFilePath >& set_Components_Component_sch_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->sch_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& set_Components_Component_src_dir(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->src_dir;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::ComponentType& set_Components_Component_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::Components_Component::Object_Part_Type_ID:
            return data::convert< data::Components::Components_Component >( m_data )->type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Action >& set_Concrete_Action_interface_action(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Action >( m_data )->interface_action;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Component >& set_Concrete_Component_interface_component(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Component >( m_data )->interface_component;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Context > >& set_Concrete_ContextGroup_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocator > >& set_Concrete_Context_allocation_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocation_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::MemoryLayout::Allocators_Allocator >& set_Concrete_Context_allocator(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->allocator;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Components::Components_Component >& set_Concrete_Context_component(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::optional< data::Ptr< data::Concrete::Concrete_Object > > >& set_Concrete_Context_concrete_object(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->concrete_object;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& set_Concrete_Context_inheritance(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& set_Concrete_Context_interface(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->interface;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_ContextGroup >& set_Concrete_Context_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Concrete_Dimensions_Allocation_concrete_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Concrete_Dimensions_Allocation::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Allocation >( m_data )->concrete_id;
        case data::MemoryLayout::Concrete_Dimensions_Allocator::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Allocation >( m_data )->concrete_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::U64 >& set_Concrete_Dimensions_Allocation_offset(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Concrete_Dimensions_Allocation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->offset;
        case data::MemoryLayout::Concrete_Dimensions_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->offset;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& set_Concrete_Dimensions_Allocation_parent_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Concrete_Dimensions_Allocation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->parent_context;
        case data::MemoryLayout::Concrete_Dimensions_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->parent_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::MemoryLayout::MemoryLayout_Part > >& set_Concrete_Dimensions_Allocation_part(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Concrete_Dimensions_Allocation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->part;
        case data::MemoryLayout::Concrete_Dimensions_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocation >( m_data )->part;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::MemoryLayout::Allocators_Allocator >& set_Concrete_Dimensions_Allocator_allocator(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::Concrete_Dimensions_Allocator::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Allocator >( m_data )->allocator;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Concrete_Dimensions_Link_singular(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->singular;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->singular;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->singular;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Clang::Interface_TypedLinkTrait >& set_Concrete_Dimensions_UserLink_interface_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_UserLink >( m_data )->interface_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Concrete_Dimensions_User_concrete_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_User >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Link >( m_data )->concrete_id;
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Link >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Link >( m_data )->concrete_id;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Context >( m_data )->concrete_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_DimensionTrait >& set_Concrete_Dimensions_User_interface_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_User >( m_data )->interface_dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Concrete_Dimensions_User_offset(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_User >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->offset;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& set_Concrete_Dimensions_User_parent_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_User >( m_data )->parent_context;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->parent_context;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->parent_context;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->parent_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::MemoryLayout::MemoryLayout_Part >& set_Concrete_Dimensions_User_part(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_User >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Link >( m_data )->part;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Event >& set_Concrete_Event_interface_event(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Event >( m_data )->interface_event;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Function >& set_Concrete_Function_interface_function(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Function >( m_data )->interface_function;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& set_Concrete_Graph_Edge_source(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Concrete_Graph_Edge::Object_Part_Type_ID:
            return data::convert< data::Model::Concrete_Graph_Edge >( m_data )->source;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& set_Concrete_Graph_Edge_target(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Concrete_Graph_Edge::Object_Part_Type_ID:
            return data::convert< data::Model::Concrete_Graph_Edge >( m_data )->target;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::EdgeType& set_Concrete_Graph_Edge_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::Concrete_Graph_Edge::Object_Part_Type_ID:
            return data::convert< data::Model::Concrete_Graph_Edge >( m_data )->type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& set_Concrete_Graph_Vertex_in_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Graph_Vertex::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->in_edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& set_Concrete_Graph_Vertex_out_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Graph_Vertex::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Graph_Vertex >( m_data )->out_edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Interupt >& set_Concrete_Interupt_interface_interupt(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Interupt >( m_data )->interface_interupt;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >& set_Concrete_MemoryMappedObject_memory_map(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryRollout::Concrete_MemoryMappedObject >( m_data )->memory_map;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Namespace >& set_Concrete_Namespace_interface_namespace(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Namespace >( m_data )->interface_namespace;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::MemoryLayout_Buffer > >& set_Concrete_Object_buffers(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Object >( m_data )->buffers;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Object >& set_Concrete_Object_interface_object(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->interface_object;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_UserDimensionContext > >& set_Concrete_Object_link_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Object >( m_data )->link_contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink > >& set_Concrete_Object_ownership_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->ownership_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_Root >& set_Concrete_Root_interface_root(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Root >( m_data )->interface_root;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_State >& set_Concrete_State_interface_state(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_State >( m_data )->interface_state;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_State >( m_data )->interface_state;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_State >( m_data )->interface_state;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Concrete_State_local_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->local_size;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Event >( m_data )->local_size;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->local_size;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->local_size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Concrete_State_total_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->total_size;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Event >( m_data )->total_size;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->total_size;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_State >( m_data )->total_size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& set_Concrete_UserDimensionContext_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& set_Concrete_UserDimensionContext_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::io::cppFilePath, data::Ptr< data::DPGraph::Dependencies_TransitiveDependencies > >& set_Dependencies_Analysis_cpp_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->cpp_dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::io::megaFilePath, data::Ptr< data::DPGraph::Dependencies_TransitiveDependencies > >& set_Dependencies_Analysis_mega_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->mega_dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::DPGraph::Dependencies_SourceFileDependencies > >& set_Dependencies_Analysis_objects(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->objects;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& set_Dependencies_Analysis_topological_mega_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Analysis::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Analysis >( m_data )->topological_mega_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Dependencies_Glob_glob(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Glob::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Glob >( m_data )->glob;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& set_Dependencies_Glob_location(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_Glob::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_Glob >( m_data )->location;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::DPGraph::Dependencies_Glob > >& set_Dependencies_SourceFileDependencies_globs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_SourceFileDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_SourceFileDependencies >( m_data )->globs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Dependencies_SourceFileDependencies_hash_code(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_SourceFileDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_SourceFileDependencies >( m_data )->hash_code;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< boost::filesystem::path >& set_Dependencies_SourceFileDependencies_resolution(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_SourceFileDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_SourceFileDependencies >( m_data )->resolution;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::io::megaFilePath& set_Dependencies_SourceFileDependencies_source_file(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_SourceFileDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_SourceFileDependencies >( m_data )->source_file;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::io::megaFilePath >& set_Dependencies_TransitiveDependencies_mega_source_files(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::DPGraph::Dependencies_TransitiveDependencies::Object_Part_Type_ID:
            return data::convert< data::DPGraph::Dependencies_TransitiveDependencies >( m_data )->mega_source_files;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& set_Derivation_Edge_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Edge::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Edge >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Derivation_Edge_eliminated(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Edge::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Edge >( m_data )->eliminated;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Derivation_Step >& set_Derivation_Edge_next(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Edge::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Edge >( m_data )->next;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Graph_Vertex > >& set_Derivation_Root_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Root::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Root >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Derivation_Edge > >& set_Derivation_Root_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Root::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Root >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Derivation_Edge > >& set_Derivation_Step_edges(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Step::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->edges;
        case data::Operations::Derivation_And::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->edges;
        case data::Operations::Derivation_Or::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->edges;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& set_Derivation_Step_vertex(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Step::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->vertex;
        case data::Operations::Derivation_And::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->vertex;
        case data::Operations::Derivation_Or::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Step >( m_data )->vertex;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< data::Ptr< data::Clang::Interface_ObjectLinkTrait >, data::Ptr< data::Model::HyperGraph_Relation > >& set_HyperGraph_Graph_relations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_Graph::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Graph >( m_data )->relations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Clang::Interface_ObjectLinkTrait >& set_HyperGraph_NonOwningObjectRelation_source(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_NonOwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_NonOwningObjectRelation >( m_data )->source;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Clang::Interface_ObjectLinkTrait >& set_HyperGraph_NonOwningObjectRelation_target(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_NonOwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_NonOwningObjectRelation >( m_data )->target;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& set_HyperGraph_OwningObjectRelation_owned(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_OwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_OwningObjectRelation >( m_data )->owned;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_ObjectLinkTrait > >& set_HyperGraph_OwningObjectRelation_owners(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_OwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_OwningObjectRelation >( m_data )->owners;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::RelationID& set_HyperGraph_Relation_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_Relation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Relation >( m_data )->id;
        case data::Model::HyperGraph_ObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Relation >( m_data )->id;
        case data::Model::HyperGraph_OwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Relation >( m_data )->id;
        case data::Model::HyperGraph_NonOwningObjectRelation::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Relation >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Concrete::Concrete_Context > >& set_Inheritance_Mapping_inheritance_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->inheritance_contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_User > >& set_Inheritance_Mapping_inheritance_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->inheritance_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& set_Inheritance_Mapping_inheritance_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->inheritance_links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Derivations::Inheritance_ObjectMapping > >& set_Inheritance_Mapping_mappings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_Mapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_Mapping >( m_data )->mappings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Inheritance_ObjectMapping_hash_code(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->hash_code;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Concrete::Concrete_Context > >& set_Inheritance_ObjectMapping_inheritance_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->inheritance_contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_User > >& set_Inheritance_ObjectMapping_inheritance_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->inheritance_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& set_Inheritance_ObjectMapping_inheritance_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->inheritance_links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::io::megaFilePath& set_Inheritance_ObjectMapping_source_file(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Derivations::Inheritance_ObjectMapping::Object_Part_Type_ID:
            return data::convert< data::Derivations::Inheritance_ObjectMapping >( m_data )->source_file;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_AbstractDef > >& set_Interface_Abstract_abstract_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->abstract_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::optional< data::Ptr< data::Tree::Interface_InheritanceTrait > > >& set_Interface_Abstract_inheritance_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->inheritance_trait;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->inheritance_trait;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Event >( m_data )->inheritance_trait;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->inheritance_trait;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->inheritance_trait;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->inheritance_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_LinkTrait > > >& set_Interface_Abstract_link_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->link_traits;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->link_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_RequirementTrait > > >& set_Interface_Abstract_requirement_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->requirement_traits;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->requirement_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::optional< data::Ptr< data::Tree::Interface_SizeTrait > > >& set_Interface_Abstract_size_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->size_trait;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->size_trait;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Event >( m_data )->size_trait;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->size_trait;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->size_trait;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->size_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& set_Interface_ArgumentListTrait_canonical_types(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_ArgumentListTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_ArgumentListTrait >( m_data )->canonical_types;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& set_Interface_ContextGroup_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_ContextGroup::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Root::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_ContextGroup >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Interface_DimensionTrait_alignment(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->alignment;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Interface_DimensionTrait_canonical_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->canonical_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& set_Interface_DimensionTrait_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_DimensionTrait >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Interface_DimensionTrait_erased_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->erased_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Interface_DimensionTrait_interface_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_DimensionTrait >( m_data )->interface_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Components::MegaMangle_Mangle >& set_Interface_DimensionTrait_mangle(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->mangle;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& set_Interface_DimensionTrait_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_DimensionTrait >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Interface_DimensionTrait_simple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->simple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Interface_DimensionTrait_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Interface_DimensionTrait_symbol_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_DimensionTrait >( m_data )->symbol_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& set_Interface_DimensionTrait_symbols(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->symbols;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& set_Interface_EventTypeTrait_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_EventTypeTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_EventTypeTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_EventDef > >& set_Interface_Event_event_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Event >( m_data )->event_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_ArgumentListTrait > >& set_Interface_Function_arguments_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Function >( m_data )->arguments_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_FunctionDef > >& set_Interface_Function_function_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Function >( m_data )->function_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_ReturnTypeTrait > >& set_Interface_Function_return_type_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Function >( m_data )->return_type_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Components::Components_Component >& set_Interface_IContext_component(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->component;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Context > >& set_Interface_IContext_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Interface_IContext_identifier(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->identifier;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Interface_IContext_interface_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_ContextGroup >& set_Interface_IContext_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_IContext >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Interface_IContext_symbol_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_IContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& set_Interface_InheritanceTrait_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_InheritanceTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_InheritanceTrait >( m_data )->contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_EventTypeTrait > >& set_Interface_Interupt_events_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Interupt >( m_data )->events_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_InteruptDef > >& set_Interface_Interupt_interupt_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Interupt >( m_data )->interupt_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Locations::Interface_InvocationInstance > >& set_Interface_InvocationContext_invocation_instances(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::SourceLocation& set_Interface_InvocationContext_source_location(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_InvocationContext::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::MetaAnalysis::Meta_SequenceAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::MetaAnalysis::Meta_StackAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::MetaAnalysis::Meta_PlanAction::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Operations_Invocation >& set_Interface_InvocationInstance_invocation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Locations::Interface_InvocationInstance::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationInstance >( m_data )->invocation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::SourceLocation& set_Interface_InvocationInstance_source_location(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Locations::Interface_InvocationInstance::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationInstance >( m_data )->source_location;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_UserLink > >& set_Interface_LinkTrait_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Interface_LinkTrait_interface_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->interface_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& set_Interface_LinkTrait_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_LinkTrait >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Interface_LinkTrait_symbol_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->symbol_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > > >& set_Interface_Namespace_dimension_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Namespace >( m_data )->dimension_traits;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->dimension_traits;
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        case data::Tree::Interface_Event::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Event >( m_data )->dimension_traits;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->dimension_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Interface_Namespace_is_global(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Namespace >( m_data )->is_global;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_ContextDef > >& set_Interface_Namespace_namespace_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Namespace >( m_data )->namespace_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Model::HyperGraph_Relation >& set_Interface_ObjectLinkTrait_relation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Interface_ObjectLinkTrait >( m_data )->relation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_ObjectDef > >& set_Interface_Object_object_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Object::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Object >( m_data )->object_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& set_Interface_PartTrait_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_PartTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_PartTrait >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& set_Interface_RequirementTrait_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_RequirementTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_RequirementTrait >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& set_Interface_RequirementTrait_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_RequirementTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_RequirementTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Interface_ReturnTypeTrait_canonical_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_ReturnTypeTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_ReturnTypeTrait >( m_data )->canonical_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ObjectSourceRoot >& set_Interface_Root_root(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_Root::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Root >( m_data )->root;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Interface_SizeTrait_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_SizeTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_SizeTrait >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_PartTrait > > >& set_Interface_State_part_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->part_traits;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_StateDef > >& set_Interface_State_state_defs(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->state_defs;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::optional< data::Ptr< data::Tree::Interface_TransitionTypeTrait > > >& set_Interface_State_transition_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->transition_trait;
        case data::Tree::Interface_Interupt::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Interupt >( m_data )->transition_trait;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->transition_trait;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->transition_trait;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& set_Interface_TransitionTypeTrait_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_TransitionTypeTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TransitionTypeTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePath > >& set_Interface_TypePathVariant_sequence(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Clang::Interface_TypePathVariant::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypePathVariant >( m_data )->sequence;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& set_Interface_TypePath_types(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Clang::Interface_TypePath::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypePath >( m_data )->types;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& set_Interface_TypedLinkTrait_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Clang::Interface_TypedLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypedLinkTrait >( m_data )->tuple;
        case data::Clang::Interface_ObjectLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypedLinkTrait >( m_data )->tuple;
        case data::Clang::Interface_ComponentLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_TypedLinkTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Invocations_Instructions_Instruction > >& set_Invocations_Instructions_InstructionGroup_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_InstructionGroup::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_Root::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_ParentDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_ChildDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_PolyBranch::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_PolyCase::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Variable >& set_Invocations_Instructions_ParentDerivation_from(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_ParentDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_ParentDerivation >( m_data )->from;
        case data::Operations::Invocations_Instructions_ChildDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_ChildDerivation >( m_data )->from;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Stack >& set_Invocations_Instructions_ParentDerivation_to(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_ParentDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_ParentDerivation >( m_data )->to;
        case data::Operations::Invocations_Instructions_ChildDerivation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_ChildDerivation >( m_data )->to;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Reference >& set_Invocations_Instructions_PolyBranch_from_reference(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_PolyBranch::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_PolyBranch >( m_data )->from_reference;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Reference >& set_Invocations_Instructions_PolyCase_reference(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_PolyCase::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_PolyCase >( m_data )->reference;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& set_Invocations_Instructions_PolyCase_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_PolyCase::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_PolyCase >( m_data )->type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Parameter >& set_Invocations_Instructions_Root_parameter(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_Root::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_Root >( m_data )->parameter;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Variable >& set_Invocations_Operations_Operation_variable(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Operations_Operation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Operations_Operation >( m_data )->variable;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Graph_Vertex > >& set_Invocations_Variables_Reference_types(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Variables_Reference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Reference >( m_data )->types;
        case data::Operations::Invocations_Variables_Memory::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Reference >( m_data )->types;
        case data::Operations::Invocations_Variables_Parameter::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Reference >( m_data )->types;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& set_Invocations_Variables_Stack_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Variables_Stack::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Stack >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Operations::Invocations_Variables_Variable > >& set_Invocations_Variables_Variable_parent(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Variables_Variable::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Variable >( m_data )->parent;
        case data::Operations::Invocations_Variables_Stack::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Variable >( m_data )->parent;
        case data::Operations::Invocations_Variables_Reference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Variable >( m_data )->parent;
        case data::Operations::Invocations_Variables_Memory::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Variable >( m_data )->parent;
        case data::Operations::Invocations_Variables_Parameter::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_Variable >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_MegaMangle_Mangle_blit(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::MegaMangle_Mangle::Object_Part_Type_ID:
            return data::convert< data::Components::MegaMangle_Mangle >( m_data )->blit;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_MegaMangle_Mangle_canon(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::MegaMangle_Mangle::Object_Part_Type_ID:
            return data::convert< data::Components::MegaMangle_Mangle >( m_data )->canon;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_MegaMangle_Mangle_impl(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::MegaMangle_Mangle::Object_Part_Type_ID:
            return data::convert< data::Components::MegaMangle_Mangle >( m_data )->impl;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_MegaMangle_Mangle_mangle(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Components::MegaMangle_Mangle::Object_Part_Type_ID:
            return data::convert< data::Components::MegaMangle_Mangle >( m_data )->mangle;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_MemoryLayout_Buffer_alignment(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Buffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->alignment;
        case data::MemoryLayout::MemoryLayout_NonSimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->alignment;
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->alignment;
        case data::MemoryLayout::MemoryLayout_GPUBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->alignment;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::MemoryLayout_Part > >& set_MemoryLayout_Buffer_parts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Buffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        case data::MemoryLayout::MemoryLayout_NonSimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        case data::MemoryLayout::MemoryLayout_GPUBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->parts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_MemoryLayout_Buffer_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Buffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->size;
        case data::MemoryLayout::MemoryLayout_NonSimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->size;
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->size;
        case data::MemoryLayout::MemoryLayout_GPUBuffer::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Buffer >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_MemoryLayout_MemoryMap_block_alignment(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryLayout::MemoryLayout_MemoryMap::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( m_data )->block_alignment;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_MemoryLayout_MemoryMap_block_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryLayout::MemoryLayout_MemoryMap::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( m_data )->block_size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Object > >& set_MemoryLayout_MemoryMap_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryLayout::MemoryLayout_MemoryMap::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( m_data )->concrete;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_MemoryLayout_MemoryMap_fixed_allocation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryLayout::MemoryLayout_MemoryMap::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( m_data )->fixed_allocation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_IContext >& set_MemoryLayout_MemoryMap_interface(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::GlobalMemoryLayout::MemoryLayout_MemoryMap::Object_Part_Type_ID:
            return data::convert< data::GlobalMemoryLayout::MemoryLayout_MemoryMap >( m_data )->interface;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::U64 >& set_MemoryLayout_Part_alignment(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->alignment;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocation > >& set_MemoryLayout_Part_allocation_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->allocation_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::MemoryLayout::MemoryLayout_Buffer > >& set_MemoryLayout_Part_buffer(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->buffer;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& set_MemoryLayout_Part_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& set_MemoryLayout_Part_link_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->link_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::U64 >& set_MemoryLayout_Part_offset(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->offset;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::U64 >& set_MemoryLayout_Part_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_MemoryLayout_Part_total_domain_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->total_domain_size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& set_MemoryLayout_Part_user_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->user_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Derivation_Root >& set_Operations_Invocation_derivation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::InvocationID& set_Operations_Invocation_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Invocations_Operations_Operation > >& set_Operations_Invocation_operations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Operations::Invocations_Instructions_Root > >& set_Operations_Invocation_root_instruction(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Operations::Invocations_Variables_Variable > >& set_Operations_Invocation_variables(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Allocate::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Stop::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_GetAction::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_GetDimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_ReadLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_WriteLink::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::InvocationID, data::Ptr< data::Operations::Operations_Invocation > >& set_Operations_Invocations_invocations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocations::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocations >( m_data )->invocations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Inheritance >& set_Parser_AbstractDef_inheritance(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_AbstractDef >( m_data )->inheritance;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->inheritance;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_EventDef >( m_data )->inheritance;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ObjectDef >( m_data )->inheritance;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->inheritance;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->inheritance;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Size >& set_Parser_AbstractDef_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_AbstractDef >( m_data )->size;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->size;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_EventDef >( m_data )->size;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ObjectDef >( m_data )->size;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->size;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::TypeName >& set_Parser_ArgumentList_args(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ArgumentList::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ArgumentList >( m_data )->args;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& set_Parser_CPPInclude_cppSourceFilePath(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_CPPInclude::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_CPPInclude >( m_data )->cppSourceFilePath;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Parser_ContextDef_body(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::Body::Parser_ContextDef >( m_data )->body;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_ContextDef > >& set_Parser_ContextDef_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Dependency > >& set_Parser_ContextDef_dependencies(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dependencies;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Dimension > >& set_Parser_ContextDef_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ScopedIdentifier >& set_Parser_ContextDef_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Include > >& set_Parser_ContextDef_includes(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->includes;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Link > >& set_Parser_ContextDef_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Part > >& set_Parser_ContextDef_parts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->parts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Requirement > >& set_Parser_ContextDef_requirements(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ContextDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_NamespaceDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_AbstractDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_EventDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        case data::AST::Parser_ObjectDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ContextDef >( m_data )->requirements;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Parser_Dependency_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Dependency::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dependency >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Identifier >& set_Parser_Dimension_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Dimension::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::AST::Parser_Initialiser > >& set_Parser_Dimension_initialiser(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Dimension::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->initialiser;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Parser_Dimension_isConst(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Dimension::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->isConst;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Parser_Dimension_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Dimension::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Dimension >( m_data )->type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ReturnType >& set_Parser_FunctionDef_returnType(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_FunctionDef >( m_data )->returnType;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Parser_Identifier_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Identifier::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Identifier >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& set_Parser_Inheritance_strings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Inheritance::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Inheritance >( m_data )->strings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Parser_Initialiser_initialiser(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Initialiser::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Initialiser >( m_data )->initialiser;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Parser_Initialiser_source_loc_end(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Initialiser::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Initialiser >( m_data )->source_loc_end;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Parser_Initialiser_source_loc_start(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Initialiser::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Initialiser >( m_data )->source_loc_start;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ArgumentList >& set_Parser_InteruptDef_argumentList(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_InteruptDef >( m_data )->argumentList;
        case data::AST::Parser_FunctionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_FunctionDef >( m_data )->argumentList;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::CardinalityRange >& set_Parser_Link_cardinality(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Link::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->cardinality;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Identifier >& set_Parser_Link_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Link::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Parser_Link_owning(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Link::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->owning;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_TypeList >& set_Parser_Link_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Link::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Link >( m_data )->type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ScopedIdentifier >& set_Parser_MegaIncludeNested_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_MegaIncludeNested::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaIncludeNested >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& set_Parser_MegaInclude_megaSourceFilePath(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_MegaInclude::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->megaSourceFilePath;
        case data::AST::Parser_MegaIncludeInline::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->megaSourceFilePath;
        case data::AST::Parser_MegaIncludeNested::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->megaSourceFilePath;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::AST::Parser_IncludeRoot > >& set_Parser_MegaInclude_root(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_MegaInclude::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->root;
        case data::AST::Parser_MegaIncludeInline::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->root;
        case data::AST::Parser_MegaIncludeNested::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_MegaInclude >( m_data )->root;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< std::string >& set_Parser_Part_identifiers(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Part::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Part >( m_data )->identifiers;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_TypeList >& set_Parser_Requirement_argumentList(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Requirement::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Requirement >( m_data )->argumentList;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Identifier >& set_Parser_Requirement_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Requirement::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Requirement >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Parser_ReturnType_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ReturnType::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ReturnType >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AST::Parser_Identifier > >& set_Parser_ScopedIdentifier_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ScopedIdentifier::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ScopedIdentifier >( m_data )->ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Parser_ScopedIdentifier_line_number(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ScopedIdentifier::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ScopedIdentifier >( m_data )->line_number;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Parser_ScopedIdentifier_source_file(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_ScopedIdentifier::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_ScopedIdentifier >( m_data )->source_file;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Parser_Size_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Size::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Size >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_ContextDef >& set_Parser_SourceRoot_ast(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_SourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->ast;
        case data::AST::Parser_IncludeRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->ast;
        case data::AST::Parser_ObjectSourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->ast;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Components::Components_Component >& set_Parser_SourceRoot_component(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_SourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->component;
        case data::AST::Parser_IncludeRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->component;
        case data::AST::Parser_ObjectSourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->component;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
boost::filesystem::path& set_Parser_SourceRoot_sourceFile(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_SourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->sourceFile;
        case data::AST::Parser_IncludeRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->sourceFile;
        case data::AST::Parser_ObjectSourceRoot::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SourceRoot >( m_data )->sourceFile;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Transition >& set_Parser_StateDef_transition(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_StateDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->transition;
        case data::AST::Parser_InteruptDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_InteruptDef >( m_data )->transition;
        case data::AST::Parser_ActionDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->transition;
        case data::AST::Parser_ComponentDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_StateDef >( m_data )->transition;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Parser_SystemInclude_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_SystemInclude::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_SystemInclude >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Parser_Transition_is_predecessor(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Transition::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Transition >( m_data )->is_predecessor;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Parser_Transition_is_successor(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Transition::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Transition >( m_data )->is_successor;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Parser_Transition_str(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_Transition::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_Transition >( m_data )->str;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::Type >& set_Parser_TypeList_args(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AST::Parser_TypeList::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_TypeList >( m_data )->args;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Context > >& set_Symbols_ConcreteTypeID_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::ConcreteTable::Symbols_ConcreteTypeID::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_ConcreteTypeID >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::MemoryLayout::Concrete_Dimensions_Allocation > >& set_Symbols_ConcreteTypeID_dim_allocation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::ConcreteTable::Symbols_ConcreteTypeID::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_ConcreteTypeID >( m_data )->dim_allocation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& set_Symbols_ConcreteTypeID_dim_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::ConcreteTable::Symbols_ConcreteTypeID::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_ConcreteTypeID >( m_data )->dim_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& set_Symbols_ConcreteTypeID_dim_user(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::ConcreteTable::Symbols_ConcreteTypeID::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_ConcreteTypeID >( m_data )->dim_user;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Symbols_ConcreteTypeID_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::ConcreteTable::Symbols_ConcreteTypeID::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_ConcreteTypeID >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_IContext > >& set_Symbols_InterfaceTypeID_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_InterfaceTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_InterfaceTypeID >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_DimensionTrait > >& set_Symbols_InterfaceTypeID_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_InterfaceTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_InterfaceTypeID >( m_data )->dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Symbols_InterfaceTypeID_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_InterfaceTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_InterfaceTypeID >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Tree::Interface_LinkTrait > >& set_Symbols_InterfaceTypeID_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_InterfaceTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_InterfaceTypeID >( m_data )->link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeIDSequence& set_Symbols_InterfaceTypeID_symbol_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_InterfaceTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_InterfaceTypeID >( m_data )->symbol_ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& set_Symbols_SymbolTable_concrete_type_id_seq_alloc(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_id_seq_alloc;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& set_Symbols_SymbolTable_concrete_type_id_sequences(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_id_sequences;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& set_Symbols_SymbolTable_concrete_type_id_set_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_id_set_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeID, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& set_Symbols_SymbolTable_concrete_type_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::ConcreteTable::Symbols_SymbolTable >( m_data )->concrete_type_ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeIDSequence, data::Ptr< data::SymbolTable::Symbols_InterfaceTypeID > >& set_Symbols_SymbolTable_interface_type_id_sequences(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->interface_type_id_sequences;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeID, data::Ptr< data::SymbolTable::Symbols_InterfaceTypeID > >& set_Symbols_SymbolTable_interface_type_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->interface_type_ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< std::string, data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& set_Symbols_SymbolTable_symbol_names(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->symbol_names;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< mega::TypeID, data::Ptr< data::SymbolTable::Symbols_SymbolTypeID > >& set_Symbols_SymbolTable_symbol_type_ids(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTable::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTable >( m_data )->symbol_type_ids;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& set_Symbols_SymbolTypeID_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > >& set_Symbols_SymbolTypeID_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Symbols_SymbolTypeID_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_LinkTrait > >& set_Symbols_SymbolTypeID_links(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->links;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_Symbols_SymbolTypeID_symbol(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::SymbolTable::Symbols_SymbolTypeID::Object_Part_Type_ID:
            return data::convert< data::SymbolTable::Symbols_SymbolTypeID >( m_data )->symbol;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::UnityAnalysis::UnityAnalysis_ObjectBinding >& set_UnityAnalysis_Binding_binding(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Binding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Binding >( m_data )->binding;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< data::Ptr< data::Concrete::Concrete_Dimensions_User >, data::Ptr< data::UnityAnalysis::UnityAnalysis_DataBinding > >& set_UnityAnalysis_Binding_dataBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Binding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Binding >( m_data )->dataBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::map< data::Ptr< data::Concrete::Concrete_Dimensions_Link >, data::Ptr< data::UnityAnalysis::UnityAnalysis_LinkBinding > >& set_UnityAnalysis_Binding_linkBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Binding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Binding >( m_data )->linkBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Object >& set_UnityAnalysis_Binding_object(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Binding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Binding >( m_data )->object;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_UnityAnalysis_DataBinding_interfaceTypeID(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_DataBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_DataBinding >( m_data )->interfaceTypeID;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_UnityAnalysis_DataBinding_typeName(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_DataBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_DataBinding >( m_data )->typeName;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_UnityAnalysis_LinkBinding_interfaceTypeID(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_LinkBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_LinkBinding >( m_data )->interfaceTypeID;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_UnityAnalysis_LinkBinding_typeName(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_LinkBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_LinkBinding >( m_data )->typeName;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_UnityAnalysis_Manual_name(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Manual >( m_data )->name;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::UnityAnalysis::UnityAnalysis_DataBinding > >& set_UnityAnalysis_ObjectBinding_dataBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_ObjectBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->dataBindings;
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->dataBindings;
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->dataBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_UnityAnalysis_ObjectBinding_interfaceTypeID(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_ObjectBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->interfaceTypeID;
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->interfaceTypeID;
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->interfaceTypeID;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::UnityAnalysis::UnityAnalysis_LinkBinding > >& set_UnityAnalysis_ObjectBinding_linkBindings(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_ObjectBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->linkBindings;
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->linkBindings;
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->linkBindings;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_UnityAnalysis_ObjectBinding_typeName(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_ObjectBinding::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->typeName;
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->typeName;
        case data::UnityAnalysis::UnityAnalysis_Manual::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_ObjectBinding >( m_data )->typeName;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::string& set_UnityAnalysis_Prefab_guid(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::UnityAnalysis::UnityAnalysis_Prefab::Object_Part_Type_ID:
            return data::convert< data::UnityAnalysis::UnityAnalysis_Prefab >( m_data )->guid;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}

mega::io::Object* Factory::create( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
{
    switch( objectInfo.getType() )
    {
        case 0: return new Components::Components_Component( loader, objectInfo );
        case 1: return new Components::MegaMangle_Mangle( loader, objectInfo );
        case 2: return new AST::Parser_Identifier( loader, objectInfo );
        case 3: return new AST::Parser_ScopedIdentifier( loader, objectInfo );
        case 4: return new AST::Parser_TypeList( loader, objectInfo );
        case 5: return new AST::Parser_ArgumentList( loader, objectInfo );
        case 6: return new AST::Parser_ReturnType( loader, objectInfo );
        case 7: return new AST::Parser_Transition( loader, objectInfo );
        case 8: return new AST::Parser_Inheritance( loader, objectInfo );
        case 9: return new AST::Parser_Size( loader, objectInfo );
        case 10: return new AST::Parser_Initialiser( loader, objectInfo );
        case 11: return new AST::Parser_Link( loader, objectInfo );
        case 12: return new AST::Parser_Dimension( loader, objectInfo );
        case 13: return new AST::Parser_Requirement( loader, objectInfo );
        case 14: return new AST::Parser_Part( loader, objectInfo );
        case 15: return new AST::Parser_Include( loader, objectInfo );
        case 16: return new AST::Parser_SystemInclude( loader, objectInfo );
        case 17: return new AST::Parser_MegaInclude( loader, objectInfo );
        case 18: return new AST::Parser_MegaIncludeInline( loader, objectInfo );
        case 19: return new AST::Parser_MegaIncludeNested( loader, objectInfo );
        case 20: return new AST::Parser_CPPInclude( loader, objectInfo );
        case 21: return new AST::Parser_Dependency( loader, objectInfo );
        case 22: return new AST::Parser_ContextDef( loader, objectInfo );
        case 24: return new AST::Parser_NamespaceDef( loader, objectInfo );
        case 25: return new AST::Parser_AbstractDef( loader, objectInfo );
        case 26: return new AST::Parser_StateDef( loader, objectInfo );
        case 27: return new AST::Parser_ActionDef( loader, objectInfo );
        case 28: return new AST::Parser_ComponentDef( loader, objectInfo );
        case 29: return new AST::Parser_EventDef( loader, objectInfo );
        case 30: return new AST::Parser_InteruptDef( loader, objectInfo );
        case 31: return new AST::Parser_FunctionDef( loader, objectInfo );
        case 32: return new AST::Parser_ObjectDef( loader, objectInfo );
        case 33: return new AST::Parser_SourceRoot( loader, objectInfo );
        case 34: return new AST::Parser_IncludeRoot( loader, objectInfo );
        case 35: return new AST::Parser_ObjectSourceRoot( loader, objectInfo );
        case 23: return new Body::Parser_ContextDef( loader, objectInfo );
        case 38: return new Tree::Interface_DimensionTrait( loader, objectInfo );
        case 42: return new Tree::Interface_LinkTrait( loader, objectInfo );
        case 49: return new Tree::Interface_InheritanceTrait( loader, objectInfo );
        case 51: return new Tree::Interface_ReturnTypeTrait( loader, objectInfo );
        case 53: return new Tree::Interface_ArgumentListTrait( loader, objectInfo );
        case 55: return new Tree::Interface_PartTrait( loader, objectInfo );
        case 56: return new Tree::Interface_RequirementTrait( loader, objectInfo );
        case 58: return new Tree::Interface_TransitionTypeTrait( loader, objectInfo );
        case 60: return new Tree::Interface_EventTypeTrait( loader, objectInfo );
        case 62: return new Tree::Interface_SizeTrait( loader, objectInfo );
        case 64: return new Tree::Interface_ContextGroup( loader, objectInfo );
        case 65: return new Tree::Interface_Root( loader, objectInfo );
        case 66: return new Tree::Interface_IContext( loader, objectInfo );
        case 70: return new Tree::Interface_InvocationContext( loader, objectInfo );
        case 72: return new Tree::Interface_Namespace( loader, objectInfo );
        case 73: return new Tree::Interface_Abstract( loader, objectInfo );
        case 74: return new Tree::Interface_State( loader, objectInfo );
        case 75: return new Tree::Interface_Action( loader, objectInfo );
        case 76: return new Tree::Interface_Component( loader, objectInfo );
        case 77: return new Tree::Interface_Event( loader, objectInfo );
        case 78: return new Tree::Interface_Interupt( loader, objectInfo );
        case 79: return new Tree::Interface_Function( loader, objectInfo );
        case 80: return new Tree::Interface_Object( loader, objectInfo );
        case 180: return new MetaAnalysis::Meta_SequenceAction( loader, objectInfo );
        case 181: return new MetaAnalysis::Meta_StackAction( loader, objectInfo );
        case 182: return new MetaAnalysis::Meta_PlanAction( loader, objectInfo );
        case 151: return new DPGraph::Dependencies_Glob( loader, objectInfo );
        case 152: return new DPGraph::Dependencies_SourceFileDependencies( loader, objectInfo );
        case 153: return new DPGraph::Dependencies_TransitiveDependencies( loader, objectInfo );
        case 154: return new DPGraph::Dependencies_Analysis( loader, objectInfo );
        case 155: return new SymbolTable::Symbols_SymbolTypeID( loader, objectInfo );
        case 156: return new SymbolTable::Symbols_InterfaceTypeID( loader, objectInfo );
        case 158: return new SymbolTable::Symbols_SymbolTable( loader, objectInfo );
        case 39: return new PerSourceSymbols::Interface_DimensionTrait( loader, objectInfo );
        case 43: return new PerSourceSymbols::Interface_LinkTrait( loader, objectInfo );
        case 67: return new PerSourceSymbols::Interface_IContext( loader, objectInfo );
        case 36: return new Clang::Interface_TypePath( loader, objectInfo );
        case 37: return new Clang::Interface_TypePathVariant( loader, objectInfo );
        case 41: return new Clang::Interface_DimensionTrait( loader, objectInfo );
        case 45: return new Clang::Interface_TypedLinkTrait( loader, objectInfo );
        case 46: return new Clang::Interface_ObjectLinkTrait( loader, objectInfo );
        case 48: return new Clang::Interface_ComponentLinkTrait( loader, objectInfo );
        case 50: return new Clang::Interface_InheritanceTrait( loader, objectInfo );
        case 52: return new Clang::Interface_ReturnTypeTrait( loader, objectInfo );
        case 54: return new Clang::Interface_ArgumentListTrait( loader, objectInfo );
        case 57: return new Clang::Interface_RequirementTrait( loader, objectInfo );
        case 59: return new Clang::Interface_TransitionTypeTrait( loader, objectInfo );
        case 61: return new Clang::Interface_EventTypeTrait( loader, objectInfo );
        case 63: return new Clang::Interface_SizeTrait( loader, objectInfo );
        case 94: return new Concrete::Concrete_Graph_Vertex( loader, objectInfo );
        case 97: return new Concrete::Concrete_Dimensions_User( loader, objectInfo );
        case 100: return new Concrete::Concrete_Dimensions_Link( loader, objectInfo );
        case 103: return new Concrete::Concrete_Dimensions_UserLink( loader, objectInfo );
        case 104: return new Concrete::Concrete_Dimensions_OwnershipLink( loader, objectInfo );
        case 108: return new Concrete::Concrete_ContextGroup( loader, objectInfo );
        case 109: return new Concrete::Concrete_Context( loader, objectInfo );
        case 112: return new Concrete::Concrete_Interupt( loader, objectInfo );
        case 113: return new Concrete::Concrete_Function( loader, objectInfo );
        case 114: return new Concrete::Concrete_UserDimensionContext( loader, objectInfo );
        case 115: return new Concrete::Concrete_Namespace( loader, objectInfo );
        case 116: return new Concrete::Concrete_State( loader, objectInfo );
        case 118: return new Concrete::Concrete_Action( loader, objectInfo );
        case 119: return new Concrete::Concrete_Component( loader, objectInfo );
        case 120: return new Concrete::Concrete_Event( loader, objectInfo );
        case 122: return new Concrete::Concrete_Object( loader, objectInfo );
        case 126: return new Concrete::Concrete_Root( loader, objectInfo );
        case 160: return new Derivations::Inheritance_ObjectMapping( loader, objectInfo );
        case 161: return new Derivations::Inheritance_Mapping( loader, objectInfo );
        case 40: return new PerSourceDerivations::Interface_DimensionTrait( loader, objectInfo );
        case 44: return new PerSourceDerivations::Interface_LinkTrait( loader, objectInfo );
        case 68: return new PerSourceDerivations::Interface_IContext( loader, objectInfo );
        case 96: return new Model::Concrete_Graph_Edge( loader, objectInfo );
        case 162: return new Model::HyperGraph_Relation( loader, objectInfo );
        case 163: return new Model::HyperGraph_ObjectRelation( loader, objectInfo );
        case 164: return new Model::HyperGraph_OwningObjectRelation( loader, objectInfo );
        case 165: return new Model::HyperGraph_NonOwningObjectRelation( loader, objectInfo );
        case 166: return new Model::HyperGraph_Graph( loader, objectInfo );
        case 47: return new PerSourceModel::Interface_ObjectLinkTrait( loader, objectInfo );
        case 95: return new PerSourceModel::Concrete_Graph_Vertex( loader, objectInfo );
        case 124: return new PerSourceModel::Concrete_Object( loader, objectInfo );
        case 127: return new Model::Alias_Edge( loader, objectInfo );
        case 128: return new Model::Alias_Step( loader, objectInfo );
        case 129: return new Model::Alias_And( loader, objectInfo );
        case 130: return new Model::Alias_Or( loader, objectInfo );
        case 131: return new Model::Alias_AliasDerivation( loader, objectInfo );
        case 99: return new MemoryLayout::Concrete_Dimensions_User( loader, objectInfo );
        case 102: return new MemoryLayout::Concrete_Dimensions_Link( loader, objectInfo );
        case 105: return new MemoryLayout::Concrete_Dimensions_Allocation( loader, objectInfo );
        case 107: return new MemoryLayout::Concrete_Dimensions_Allocator( loader, objectInfo );
        case 111: return new MemoryLayout::Concrete_Context( loader, objectInfo );
        case 117: return new MemoryLayout::Concrete_State( loader, objectInfo );
        case 121: return new MemoryLayout::Concrete_Event( loader, objectInfo );
        case 123: return new MemoryLayout::Concrete_Object( loader, objectInfo );
        case 167: return new MemoryLayout::Allocators_Allocator( loader, objectInfo );
        case 168: return new MemoryLayout::Allocators_Nothing( loader, objectInfo );
        case 169: return new MemoryLayout::Allocators_Singleton( loader, objectInfo );
        case 170: return new MemoryLayout::Allocators_Range( loader, objectInfo );
        case 171: return new MemoryLayout::Allocators_Range32( loader, objectInfo );
        case 172: return new MemoryLayout::Allocators_Range64( loader, objectInfo );
        case 173: return new MemoryLayout::Allocators_RangeAny( loader, objectInfo );
        case 174: return new MemoryLayout::MemoryLayout_Part( loader, objectInfo );
        case 175: return new MemoryLayout::MemoryLayout_Buffer( loader, objectInfo );
        case 176: return new MemoryLayout::MemoryLayout_NonSimpleBuffer( loader, objectInfo );
        case 177: return new MemoryLayout::MemoryLayout_SimpleBuffer( loader, objectInfo );
        case 178: return new MemoryLayout::MemoryLayout_GPUBuffer( loader, objectInfo );
        case 179: return new GlobalMemoryLayout::MemoryLayout_MemoryMap( loader, objectInfo );
        case 125: return new GlobalMemoryRollout::Concrete_MemoryMappedObject( loader, objectInfo );
        case 157: return new ConcreteTable::Symbols_ConcreteTypeID( loader, objectInfo );
        case 159: return new ConcreteTable::Symbols_SymbolTable( loader, objectInfo );
        case 98: return new PerSourceConcreteTable::Concrete_Dimensions_User( loader, objectInfo );
        case 101: return new PerSourceConcreteTable::Concrete_Dimensions_Link( loader, objectInfo );
        case 106: return new PerSourceConcreteTable::Concrete_Dimensions_Allocation( loader, objectInfo );
        case 110: return new PerSourceConcreteTable::Concrete_Context( loader, objectInfo );
        case 81: return new Operations::Invocations_Variables_Variable( loader, objectInfo );
        case 82: return new Operations::Invocations_Variables_Stack( loader, objectInfo );
        case 83: return new Operations::Invocations_Variables_Reference( loader, objectInfo );
        case 84: return new Operations::Invocations_Variables_Memory( loader, objectInfo );
        case 85: return new Operations::Invocations_Variables_Parameter( loader, objectInfo );
        case 86: return new Operations::Invocations_Instructions_Instruction( loader, objectInfo );
        case 87: return new Operations::Invocations_Instructions_InstructionGroup( loader, objectInfo );
        case 88: return new Operations::Invocations_Instructions_Root( loader, objectInfo );
        case 89: return new Operations::Invocations_Instructions_ParentDerivation( loader, objectInfo );
        case 90: return new Operations::Invocations_Instructions_ChildDerivation( loader, objectInfo );
        case 91: return new Operations::Invocations_Instructions_PolyBranch( loader, objectInfo );
        case 92: return new Operations::Invocations_Instructions_PolyCase( loader, objectInfo );
        case 93: return new Operations::Invocations_Operations_Operation( loader, objectInfo );
        case 132: return new Operations::Derivation_Edge( loader, objectInfo );
        case 133: return new Operations::Derivation_Step( loader, objectInfo );
        case 134: return new Operations::Derivation_And( loader, objectInfo );
        case 135: return new Operations::Derivation_Or( loader, objectInfo );
        case 136: return new Operations::Derivation_Root( loader, objectInfo );
        case 137: return new Operations::Operations_Invocation( loader, objectInfo );
        case 138: return new Operations::Operations_Allocate( loader, objectInfo );
        case 139: return new Operations::Operations_Call( loader, objectInfo );
        case 140: return new Operations::Operations_Start( loader, objectInfo );
        case 141: return new Operations::Operations_Stop( loader, objectInfo );
        case 142: return new Operations::Operations_Move( loader, objectInfo );
        case 143: return new Operations::Operations_GetAction( loader, objectInfo );
        case 144: return new Operations::Operations_GetDimension( loader, objectInfo );
        case 145: return new Operations::Operations_Read( loader, objectInfo );
        case 146: return new Operations::Operations_Write( loader, objectInfo );
        case 147: return new Operations::Operations_ReadLink( loader, objectInfo );
        case 148: return new Operations::Operations_WriteLink( loader, objectInfo );
        case 149: return new Operations::Operations_Range( loader, objectInfo );
        case 150: return new Operations::Operations_Invocations( loader, objectInfo );
        case 69: return new Locations::Interface_InvocationInstance( loader, objectInfo );
        case 71: return new Locations::Interface_InvocationContext( loader, objectInfo );
        case 183: return new UnityAnalysis::UnityAnalysis_DataBinding( loader, objectInfo );
        case 184: return new UnityAnalysis::UnityAnalysis_LinkBinding( loader, objectInfo );
        case 185: return new UnityAnalysis::UnityAnalysis_ObjectBinding( loader, objectInfo );
        case 186: return new UnityAnalysis::UnityAnalysis_Prefab( loader, objectInfo );
        case 187: return new UnityAnalysis::UnityAnalysis_Manual( loader, objectInfo );
        case 188: return new UnityAnalysis::UnityAnalysis_Binding( loader, objectInfo );
        default:
            THROW_RTE( "Unrecognised object type ID" );
    }
}

}
