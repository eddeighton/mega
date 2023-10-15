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
        
    // struct Parser_Link : public mega::io::Object
    Parser_Link::Parser_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_Link >( loader, this ) )          , id( loader )
          , type( loader )
    {
    }
    Parser_Link::Parser_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Identifier >& id, const data::Ptr< data::AST::Parser_TypeList >& type, const bool& owning, const mega::CardinalityRange& cardinality)
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
        
    // struct Parser_ContextDef : public mega::io::Object
    Parser_ContextDef::Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ContextDef >( loader, this ) )          , p_Body_Parser_ContextDef( loader )
          , id( loader )
    {
    }
    Parser_ContextDef::Parser_ContextDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_ScopedIdentifier >& id, const std::vector< data::Ptr< data::AST::Parser_ContextDef > >& children, const std::vector< data::Ptr< data::AST::Parser_Dimension > >& dimensions, const std::vector< data::Ptr< data::AST::Parser_Link > >& links, const std::vector< data::Ptr< data::AST::Parser_Include > >& includes, const std::vector< data::Ptr< data::AST::Parser_Dependency > >& dependencies, const std::vector< data::Ptr< data::AST::Parser_Part > >& parts)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_ContextDef >( loader, this ) )          , p_Body_Parser_ContextDef( loader )
          , id( id )
          , children( children )
          , dimensions( dimensions )
          , links( links )
          , includes( includes )
          , dependencies( dependencies )
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
        storer.store( parts );
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
        
    // struct Parser_RequirementDef : public mega::io::Object
    Parser_RequirementDef::Parser_RequirementDef( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AST::Parser_RequirementDef >( loader, this ) )          , p_AST_Parser_InteruptDef( loader )
    {
    }
    bool Parser_RequirementDef::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AST::Parser_RequirementDef >( loader, const_cast< Parser_RequirementDef* >( this ) ) };
    }
    void Parser_RequirementDef::set_inheritance_pointer()
    {
        p_AST_Parser_InteruptDef->m_inheritance = data::Ptr< data::AST::Parser_RequirementDef >( p_AST_Parser_InteruptDef, this );
    }
    void Parser_RequirementDef::load( mega::io::Loader& loader )
    {
        loader.load( p_AST_Parser_InteruptDef );
    }
    void Parser_RequirementDef::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AST_Parser_InteruptDef );
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
        
}
namespace Tree
{
    // struct Interface_DimensionTrait : public mega::io::Object
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_DimensionTrait >( loader, this ) )          , p_PerSourceSymbols_Interface_DimensionTrait( loader )
          , p_PerSourceDerivations_Interface_DimensionTrait( loader )
          , p_Clang_Interface_DimensionTrait( loader )
          , parent( loader )
    {
    }
    Interface_DimensionTrait::Interface_DimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_IContext >& parent)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_DimensionTrait >( loader, this ) )          , p_PerSourceSymbols_Interface_DimensionTrait( loader )
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
    }
    void Interface_DimensionTrait::load( mega::io::Loader& loader )
    {
        loader.load( parent );
    }
    void Interface_DimensionTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( parent );
    }
        
    // struct Interface_UserDimensionTrait : public mega::io::Object
    Interface_UserDimensionTrait::Interface_UserDimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_UserDimensionTrait >( loader, this ) )          , p_Tree_Interface_DimensionTrait( loader )
          , parser_dimension( loader )
    {
    }
    Interface_UserDimensionTrait::Interface_UserDimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Dimension >& parser_dimension)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_UserDimensionTrait >( loader, this ) )          , p_Tree_Interface_DimensionTrait( loader )
          , parser_dimension( parser_dimension )
    {
    }
    bool Interface_UserDimensionTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_UserDimensionTrait >( loader, const_cast< Interface_UserDimensionTrait* >( this ) ) };
    }
    void Interface_UserDimensionTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_DimensionTrait->m_inheritance = data::Ptr< data::Tree::Interface_UserDimensionTrait >( p_Tree_Interface_DimensionTrait, this );
    }
    void Interface_UserDimensionTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_DimensionTrait );
        loader.load( parser_dimension );
    }
    void Interface_UserDimensionTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_DimensionTrait );
        storer.store( parser_dimension );
    }
        
    // struct Interface_CompilerDimensionTrait : public mega::io::Object
    Interface_CompilerDimensionTrait::Interface_CompilerDimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_CompilerDimensionTrait >( loader, this ) )          , p_Tree_Interface_DimensionTrait( loader )
    {
    }
    Interface_CompilerDimensionTrait::Interface_CompilerDimensionTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::string& identifier)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_CompilerDimensionTrait >( loader, this ) )          , p_Tree_Interface_DimensionTrait( loader )
          , identifier( identifier )
    {
    }
    bool Interface_CompilerDimensionTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_CompilerDimensionTrait >( loader, const_cast< Interface_CompilerDimensionTrait* >( this ) ) };
    }
    void Interface_CompilerDimensionTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_DimensionTrait->m_inheritance = data::Ptr< data::Tree::Interface_CompilerDimensionTrait >( p_Tree_Interface_DimensionTrait, this );
    }
    void Interface_CompilerDimensionTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_DimensionTrait );
        loader.load( identifier );
    }
    void Interface_CompilerDimensionTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_DimensionTrait );
        storer.store( identifier );
    }
        
    // struct Interface_LinkTrait : public mega::io::Object
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_LinkTrait >( loader, this ) )          , p_PerSourceSymbols_Interface_LinkTrait( loader )
          , p_PerSourceDerivations_Interface_LinkTrait( loader )
          , p_PerSourceModel_Interface_LinkTrait( loader )
          , parent( loader )
    {
    }
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_IContext >& parent)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_LinkTrait >( loader, this ) )          , p_PerSourceSymbols_Interface_LinkTrait( loader )
          , p_PerSourceDerivations_Interface_LinkTrait( loader )
          , p_PerSourceModel_Interface_LinkTrait( loader )
          , parent( parent )
    {
    }
    bool Interface_LinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_LinkTrait >( loader, const_cast< Interface_LinkTrait* >( this ) ) };
    }
    void Interface_LinkTrait::set_inheritance_pointer()
    {
    }
    void Interface_LinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( parent );
    }
    void Interface_LinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( parent );
    }
        
    // struct Interface_UserLinkTrait : public mega::io::Object
    Interface_UserLinkTrait::Interface_UserLinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_UserLinkTrait >( loader, this ) )          , p_Tree_Interface_LinkTrait( loader )
          , p_Clang_Interface_UserLinkTrait( loader )
          , parser_link( loader )
    {
    }
    Interface_UserLinkTrait::Interface_UserLinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::AST::Parser_Link >& parser_link)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_UserLinkTrait >( loader, this ) )          , p_Tree_Interface_LinkTrait( loader )
          , p_Clang_Interface_UserLinkTrait( loader )
          , parser_link( parser_link )
    {
    }
    bool Interface_UserLinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_UserLinkTrait >( loader, const_cast< Interface_UserLinkTrait* >( this ) ) };
    }
    void Interface_UserLinkTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_LinkTrait->m_inheritance = data::Ptr< data::Tree::Interface_UserLinkTrait >( p_Tree_Interface_LinkTrait, this );
    }
    void Interface_UserLinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_LinkTrait );
        loader.load( parser_link );
    }
    void Interface_UserLinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_LinkTrait );
        storer.store( parser_link );
    }
        
    // struct Interface_OwnershipLinkTrait : public mega::io::Object
    Interface_OwnershipLinkTrait::Interface_OwnershipLinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_OwnershipLinkTrait >( loader, this ) )          , p_Tree_Interface_LinkTrait( loader )
    {
    }
    bool Interface_OwnershipLinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_OwnershipLinkTrait >( loader, const_cast< Interface_OwnershipLinkTrait* >( this ) ) };
    }
    void Interface_OwnershipLinkTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_LinkTrait->m_inheritance = data::Ptr< data::Tree::Interface_OwnershipLinkTrait >( p_Tree_Interface_LinkTrait, this );
    }
    void Interface_OwnershipLinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_LinkTrait );
    }
    void Interface_OwnershipLinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_LinkTrait );
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
    }
        
    // struct Interface_State : public mega::io::Object
    Interface_State::Interface_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_State >( loader, this ) )          , p_Tree_Interface_InvocationContext( loader )
          , p_MetaAnalysis_Interface_State( loader )
    {
    }
    Interface_State::Interface_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::AST::Parser_StateDef > >& state_defs)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_State >( loader, this ) )          , p_Tree_Interface_InvocationContext( loader )
          , p_MetaAnalysis_Interface_State( loader )
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
        
    // struct Interface_Requirement : public mega::io::Object
    Interface_Requirement::Interface_Requirement( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Tree::Interface_Requirement >( loader, this ) )          , p_Tree_Interface_Interupt( loader )
    {
    }
    bool Interface_Requirement::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Tree::Interface_Requirement >( loader, const_cast< Interface_Requirement* >( this ) ) };
    }
    void Interface_Requirement::set_inheritance_pointer()
    {
        p_Tree_Interface_Interupt->m_inheritance = data::Ptr< data::Tree::Interface_Requirement >( p_Tree_Interface_Interupt, this );
    }
    void Interface_Requirement::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_Interupt );
    }
    void Interface_Requirement::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_Interupt );
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
        
}
namespace MetaAnalysis
{
    // struct Interface_State : public mega::io::Object
    Interface_State::Interface_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_State( loader )
    {
    }
    Interface_State::Interface_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_State > p_Tree_Interface_State, const bool& is_or_state)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_State( p_Tree_Interface_State )
          , is_or_state( is_or_state )
    {
    }
    bool Interface_State::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_State::set_inheritance_pointer()
    {
        p_Tree_Interface_State->p_MetaAnalysis_Interface_State = data::Ptr< data::MetaAnalysis::Interface_State >( p_Tree_Interface_State, this );
    }
    void Interface_State::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_State );
        loader.load( is_or_state );
    }
    void Interface_State::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_State );
        storer.store( is_or_state );
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
        
    // struct Interface_UserLinkTrait : public mega::io::Object
    Interface_UserLinkTrait::Interface_UserLinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_UserLinkTrait( loader )
    {
    }
    Interface_UserLinkTrait::Interface_UserLinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_UserLinkTrait > p_Tree_Interface_UserLinkTrait, const std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& tuple)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_UserLinkTrait( p_Tree_Interface_UserLinkTrait )
          , tuple( tuple )
    {
    }
    bool Interface_UserLinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_UserLinkTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_UserLinkTrait->p_Clang_Interface_UserLinkTrait = data::Ptr< data::Clang::Interface_UserLinkTrait >( p_Tree_Interface_UserLinkTrait, this );
    }
    void Interface_UserLinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_UserLinkTrait );
        loader.load( tuple );
    }
    void Interface_UserLinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_UserLinkTrait );
        storer.store( tuple );
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
        
    // struct Concrete_Dimensions_User : public mega::io::Object
    Concrete_Dimensions_User::Concrete_Dimensions_User( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_User >( loader, this ) )          , p_Concrete_Concrete_Graph_Vertex( loader )
          , p_MemoryLayout_Concrete_Dimensions_User( loader )
          , p_PerSourceConcreteTable_Concrete_Dimensions_User( loader )
          , parent_context( loader )
          , interface_dimension( loader )
    {
    }
    Concrete_Dimensions_User::Concrete_Dimensions_User( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Context >& parent_context, const data::Ptr< data::Tree::Interface_DimensionTrait >& interface_dimension)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_User >( loader, this ) )          , p_Concrete_Concrete_Graph_Vertex( loader )
          , p_MemoryLayout_Concrete_Dimensions_User( loader )
          , p_PerSourceConcreteTable_Concrete_Dimensions_User( loader )
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
        
    // struct Concrete_Dimensions_Link : public mega::io::Object
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_Link >( loader, this ) )          , p_Concrete_Concrete_Graph_Vertex( loader )
          , p_PerSourceModel_Concrete_Dimensions_Link( loader )
          , p_MemoryLayout_Concrete_Dimensions_Link( loader )
          , p_PerSourceConcreteTable_Concrete_Dimensions_Link( loader )
          , parent_context( loader )
          , interface_link( loader )
          , link_type( loader )
    {
    }
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Context >& parent_context, const data::Ptr< data::Tree::Interface_LinkTrait >& interface_link)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_Link >( loader, this ) )          , p_Concrete_Concrete_Graph_Vertex( loader )
          , p_PerSourceModel_Concrete_Dimensions_Link( loader )
          , p_MemoryLayout_Concrete_Dimensions_Link( loader )
          , p_PerSourceConcreteTable_Concrete_Dimensions_Link( loader )
          , parent_context( parent_context )
          , interface_link( interface_link )
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
        loader.load( interface_link );
        loader.load( link_type );
    }
    void Concrete_Dimensions_Link::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Graph_Vertex );
        storer.store( parent_context );
        storer.store( interface_link );
        VERIFY_RTE_MSG( link_type.has_value(), "Concrete::Concrete_Dimensions_Link.link_type has NOT been set" );
        storer.store( link_type );
    }
        
    // struct Concrete_Dimensions_UserLink : public mega::io::Object
    Concrete_Dimensions_UserLink::Concrete_Dimensions_UserLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_UserLink >( loader, this ) )          , p_Concrete_Concrete_Dimensions_Link( loader )
          , interface_user_link( loader )
    {
    }
    Concrete_Dimensions_UserLink::Concrete_Dimensions_UserLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_UserLinkTrait >& interface_user_link)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_UserLink >( loader, this ) )          , p_Concrete_Concrete_Dimensions_Link( loader )
          , interface_user_link( interface_user_link )
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
        loader.load( interface_user_link );
    }
    void Concrete_Dimensions_UserLink::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Link );
        storer.store( interface_user_link );
    }
        
    // struct Concrete_Dimensions_OwnershipLink : public mega::io::Object
    Concrete_Dimensions_OwnershipLink::Concrete_Dimensions_OwnershipLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink >( loader, this ) )          , p_Concrete_Concrete_Dimensions_Link( loader )
          , interface_owner_link( loader )
    {
    }
    Concrete_Dimensions_OwnershipLink::Concrete_Dimensions_OwnershipLink( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_OwnershipLinkTrait >& interface_owner_link)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink >( loader, this ) )          , p_Concrete_Concrete_Dimensions_Link( loader )
          , interface_owner_link( interface_owner_link )
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
        loader.load( interface_owner_link );
    }
    void Concrete_Dimensions_OwnershipLink::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Link );
        storer.store( interface_owner_link );
    }
        
    // struct Concrete_Dimensions_LinkType : public mega::io::Object
    Concrete_Dimensions_LinkType::Concrete_Dimensions_LinkType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_LinkType >( loader, this ) )          , p_MemoryLayout_Concrete_Dimensions_LinkType( loader )
          , parent_context( loader )
          , link( loader )
    {
    }
    Concrete_Dimensions_LinkType::Concrete_Dimensions_LinkType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Context >& parent_context, const data::Ptr< data::Concrete::Concrete_Dimensions_Link >& link)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_LinkType >( loader, this ) )          , p_MemoryLayout_Concrete_Dimensions_LinkType( loader )
          , parent_context( parent_context )
          , link( link )
    {
    }
    bool Concrete_Dimensions_LinkType::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Dimensions_LinkType >( loader, const_cast< Concrete_Dimensions_LinkType* >( this ) ) };
    }
    void Concrete_Dimensions_LinkType::set_inheritance_pointer()
    {
    }
    void Concrete_Dimensions_LinkType::load( mega::io::Loader& loader )
    {
        loader.load( parent_context );
        loader.load( link );
    }
    void Concrete_Dimensions_LinkType::store( mega::io::Storer& storer ) const
    {
        storer.store( parent_context );
        storer.store( link );
    }
        
    // struct Concrete_Dimensions_Bitset : public mega::io::Object
    Concrete_Dimensions_Bitset::Concrete_Dimensions_Bitset( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_Bitset >( loader, this ) )          , p_MemoryLayout_Concrete_Dimensions_Bitset( loader )
          , p_PerSourceConcreteTable_Concrete_Dimensions_Bitset( loader )
          , parent_object( loader )
          , interface_compiler_dimension( loader )
    {
    }
    Concrete_Dimensions_Bitset::Concrete_Dimensions_Bitset( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Object >& parent_object, const data::Ptr< data::Tree::Interface_CompilerDimensionTrait >& interface_compiler_dimension)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_Bitset >( loader, this ) )          , p_MemoryLayout_Concrete_Dimensions_Bitset( loader )
          , p_PerSourceConcreteTable_Concrete_Dimensions_Bitset( loader )
          , parent_object( parent_object )
          , interface_compiler_dimension( interface_compiler_dimension )
    {
    }
    bool Concrete_Dimensions_Bitset::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Dimensions_Bitset >( loader, const_cast< Concrete_Dimensions_Bitset* >( this ) ) };
    }
    void Concrete_Dimensions_Bitset::set_inheritance_pointer()
    {
    }
    void Concrete_Dimensions_Bitset::load( mega::io::Loader& loader )
    {
        loader.load( parent_object );
        loader.load( interface_compiler_dimension );
    }
    void Concrete_Dimensions_Bitset::store( mega::io::Storer& storer ) const
    {
        storer.store( parent_object );
        storer.store( interface_compiler_dimension );
    }
        
    // struct Concrete_Dimensions_Configuration : public mega::io::Object
    Concrete_Dimensions_Configuration::Concrete_Dimensions_Configuration( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_Configuration >( loader, this ) )          , p_Concrete_Concrete_Dimensions_Bitset( loader )
    {
    }
    bool Concrete_Dimensions_Configuration::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Dimensions_Configuration >( loader, const_cast< Concrete_Dimensions_Configuration* >( this ) ) };
    }
    void Concrete_Dimensions_Configuration::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_Bitset->m_inheritance = data::Ptr< data::Concrete::Concrete_Dimensions_Configuration >( p_Concrete_Concrete_Dimensions_Bitset, this );
    }
    void Concrete_Dimensions_Configuration::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_Bitset );
    }
    void Concrete_Dimensions_Configuration::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Bitset );
    }
        
    // struct Concrete_Dimensions_Activation : public mega::io::Object
    Concrete_Dimensions_Activation::Concrete_Dimensions_Activation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_Activation >( loader, this ) )          , p_Concrete_Concrete_Dimensions_Bitset( loader )
    {
    }
    bool Concrete_Dimensions_Activation::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Dimensions_Activation >( loader, const_cast< Concrete_Dimensions_Activation* >( this ) ) };
    }
    void Concrete_Dimensions_Activation::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_Bitset->m_inheritance = data::Ptr< data::Concrete::Concrete_Dimensions_Activation >( p_Concrete_Concrete_Dimensions_Bitset, this );
    }
    void Concrete_Dimensions_Activation::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_Bitset );
    }
    void Concrete_Dimensions_Activation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Bitset );
    }
        
    // struct Concrete_Dimensions_Enablement : public mega::io::Object
    Concrete_Dimensions_Enablement::Concrete_Dimensions_Enablement( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_Enablement >( loader, this ) )          , p_Concrete_Concrete_Dimensions_Bitset( loader )
    {
    }
    bool Concrete_Dimensions_Enablement::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Dimensions_Enablement >( loader, const_cast< Concrete_Dimensions_Enablement* >( this ) ) };
    }
    void Concrete_Dimensions_Enablement::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_Bitset->m_inheritance = data::Ptr< data::Concrete::Concrete_Dimensions_Enablement >( p_Concrete_Concrete_Dimensions_Bitset, this );
    }
    void Concrete_Dimensions_Enablement::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_Bitset );
    }
    void Concrete_Dimensions_Enablement::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Bitset );
    }
        
    // struct Concrete_Dimensions_History : public mega::io::Object
    Concrete_Dimensions_History::Concrete_Dimensions_History( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Dimensions_History >( loader, this ) )          , p_Concrete_Concrete_Dimensions_Bitset( loader )
    {
    }
    bool Concrete_Dimensions_History::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Dimensions_History >( loader, const_cast< Concrete_Dimensions_History* >( this ) ) };
    }
    void Concrete_Dimensions_History::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_Bitset->m_inheritance = data::Ptr< data::Concrete::Concrete_Dimensions_History >( p_Concrete_Concrete_Dimensions_Bitset, this );
    }
    void Concrete_Dimensions_History::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_Bitset );
    }
    void Concrete_Dimensions_History::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Bitset );
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
        
    // struct Concrete_Context : public mega::io::Object
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Context >( loader, this ) )          , p_Concrete_Concrete_ContextGroup( loader )
          , p_MemoryLayout_Concrete_Context( loader )
          , p_PerSourceConcreteTable_Concrete_Context( loader )
          , component( loader )
          , parent( loader )
          , interface( loader )
    {
    }
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Components::Components_Component >& component, const data::Ptr< data::Concrete::Concrete_ContextGroup >& parent, const data::Ptr< data::Tree::Interface_IContext >& interface, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& inheritance)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Context >( loader, this ) )          , p_Concrete_Concrete_ContextGroup( loader )
          , p_MemoryLayout_Concrete_Context( loader )
          , p_PerSourceConcreteTable_Concrete_Context( loader )
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
        
    // struct Concrete_Requirement : public mega::io::Object
    Concrete_Requirement::Concrete_Requirement( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Requirement >( loader, this ) )          , p_Concrete_Concrete_Interupt( loader )
    {
    }
    bool Concrete_Requirement::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Concrete::Concrete_Requirement >( loader, const_cast< Concrete_Requirement* >( this ) ) };
    }
    void Concrete_Requirement::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Interupt->m_inheritance = data::Ptr< data::Concrete::Concrete_Requirement >( p_Concrete_Concrete_Interupt, this );
    }
    void Concrete_Requirement::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Interupt );
    }
    void Concrete_Requirement::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Interupt );
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
        
    // struct Concrete_UserDimensionContext : public mega::io::Object
    Concrete_UserDimensionContext::Concrete_UserDimensionContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_UserDimensionContext >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
    {
    }
    Concrete_UserDimensionContext::Concrete_UserDimensionContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& dimensions, const std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& links, const std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Bitset > >& bitsets)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_UserDimensionContext >( loader, this ) )          , p_Concrete_Concrete_Context( loader )
          , dimensions( dimensions )
          , links( links )
          , bitsets( bitsets )
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
        loader.load( bitsets );
    }
    void Concrete_UserDimensionContext::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( dimensions );
        storer.store( links );
        storer.store( bitsets );
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
        
    // struct Concrete_State : public mega::io::Object
    Concrete_State::Concrete_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_State >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , p_AutomataAnalysis_Concrete_State( loader )
          , interface_state( loader )
    {
    }
    Concrete_State::Concrete_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_State >& interface_state)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_State >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , p_AutomataAnalysis_Concrete_State( loader )
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
        
    // struct Concrete_Event : public mega::io::Object
    Concrete_Event::Concrete_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Event >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , interface_event( loader )
    {
    }
    Concrete_Event::Concrete_Event( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Event >& interface_event)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Event >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
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
        
    // struct Concrete_Object : public mega::io::Object
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Object >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , p_MemoryLayout_Concrete_Object( loader )
          , p_PerSourceModel_Concrete_Object( loader )
          , p_AutomataAnalysis_Concrete_Object( loader )
          , interface_object( loader )
          , ownership_link( loader )
          , configuration( loader )
          , activation( loader )
          , enablement( loader )
          , history( loader )
    {
    }
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_Object >& interface_object)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Concrete::Concrete_Object >( loader, this ) )          , p_Concrete_Concrete_UserDimensionContext( loader )
          , p_MemoryLayout_Concrete_Object( loader )
          , p_PerSourceModel_Concrete_Object( loader )
          , p_AutomataAnalysis_Concrete_Object( loader )
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
        loader.load( configuration );
        loader.load( activation );
        loader.load( enablement );
        loader.load( history );
    }
    void Concrete_Object::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_UserDimensionContext );
        storer.store( interface_object );
        VERIFY_RTE_MSG( ownership_link.has_value(), "Concrete::Concrete_Object.ownership_link has NOT been set" );
        storer.store( ownership_link );
        VERIFY_RTE_MSG( configuration.has_value(), "Concrete::Concrete_Object.configuration has NOT been set" );
        storer.store( configuration );
        VERIFY_RTE_MSG( activation.has_value(), "Concrete::Concrete_Object.activation has NOT been set" );
        storer.store( activation );
        VERIFY_RTE_MSG( enablement.has_value(), "Concrete::Concrete_Object.enablement has NOT been set" );
        storer.store( enablement );
        VERIFY_RTE_MSG( history.has_value(), "Concrete::Concrete_Object.history has NOT been set" );
        storer.store( history );
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
        
}
namespace Derivations
{
    // struct Inheritance_ObjectMapping : public mega::io::Object
    Inheritance_ObjectMapping::Inheritance_ObjectMapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Derivations::Inheritance_ObjectMapping >( loader, this ) )    {
    }
    Inheritance_ObjectMapping::Inheritance_ObjectMapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::io::megaFilePath& source_file, const mega::U64& hash_code, const std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Concrete::Concrete_Context > >& inheritance_contexts, const std::multimap< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_User > >& inheritance_dimensions, const std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& inheritance_links)
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
        
    // struct Inheritance_Mapping : public mega::io::Object
    Inheritance_Mapping::Inheritance_Mapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Derivations::Inheritance_Mapping >( loader, this ) )    {
    }
    Inheritance_Mapping::Inheritance_Mapping( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Derivations::Inheritance_ObjectMapping > >& mappings, const std::multimap< data::Ptr< data::Tree::Interface_IContext >, data::Ptr< data::Concrete::Concrete_Context > >& inheritance_contexts, const std::multimap< data::Ptr< data::Tree::Interface_DimensionTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_User > >& inheritance_dimensions, const std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& inheritance_links)
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
        
    // struct Interface_LinkTrait : public mega::io::Object
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_LinkTrait( loader )
    {
    }
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_LinkTrait > p_Tree_Interface_LinkTrait, const std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& concrete)
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
        
    // struct HyperGraph_OwningObjectRelation : public mega::io::Object
    HyperGraph_OwningObjectRelation::HyperGraph_OwningObjectRelation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_OwningObjectRelation >( loader, this ) )          , p_Model_HyperGraph_Relation( loader )
    {
    }
    HyperGraph_OwningObjectRelation::HyperGraph_OwningObjectRelation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::multimap< data::Ptr< data::Tree::Interface_UserLinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink > >& owners, const std::multimap< data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink >, data::Ptr< data::Tree::Interface_UserLinkTrait > >& owned)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_OwningObjectRelation >( loader, this ) )          , p_Model_HyperGraph_Relation( loader )
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
        p_Model_HyperGraph_Relation->m_inheritance = data::Ptr< data::Model::HyperGraph_OwningObjectRelation >( p_Model_HyperGraph_Relation, this );
    }
    void HyperGraph_OwningObjectRelation::load( mega::io::Loader& loader )
    {
        loader.load( p_Model_HyperGraph_Relation );
        loader.load( owners );
        loader.load( owned );
    }
    void HyperGraph_OwningObjectRelation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Model_HyperGraph_Relation );
        storer.store( owners );
        storer.store( owned );
    }
        
    // struct HyperGraph_NonOwningObjectRelation : public mega::io::Object
    HyperGraph_NonOwningObjectRelation::HyperGraph_NonOwningObjectRelation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_NonOwningObjectRelation >( loader, this ) )          , p_Model_HyperGraph_Relation( loader )
          , source( loader )
          , target( loader )
    {
    }
    HyperGraph_NonOwningObjectRelation::HyperGraph_NonOwningObjectRelation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Tree::Interface_UserLinkTrait >& source, const data::Ptr< data::Tree::Interface_UserLinkTrait >& target)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_NonOwningObjectRelation >( loader, this ) )          , p_Model_HyperGraph_Relation( loader )
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
        p_Model_HyperGraph_Relation->m_inheritance = data::Ptr< data::Model::HyperGraph_NonOwningObjectRelation >( p_Model_HyperGraph_Relation, this );
    }
    void HyperGraph_NonOwningObjectRelation::load( mega::io::Loader& loader )
    {
        loader.load( p_Model_HyperGraph_Relation );
        loader.load( source );
        loader.load( target );
    }
    void HyperGraph_NonOwningObjectRelation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Model_HyperGraph_Relation );
        storer.store( source );
        storer.store( target );
    }
        
    // struct HyperGraph_Graph : public mega::io::Object
    HyperGraph_Graph::HyperGraph_Graph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_Graph >( loader, this ) )          , owning_relation( loader )
    {
    }
    HyperGraph_Graph::HyperGraph_Graph( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Model::HyperGraph_OwningObjectRelation >& owning_relation, const std::map< data::Ptr< data::Tree::Interface_UserLinkTrait >, data::Ptr< data::Model::HyperGraph_NonOwningObjectRelation > >& non_owning_relations)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Model::HyperGraph_Graph >( loader, this ) )          , owning_relation( owning_relation )
          , non_owning_relations( non_owning_relations )
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
        loader.load( owning_relation );
        loader.load( non_owning_relations );
    }
    void HyperGraph_Graph::store( mega::io::Storer& storer ) const
    {
        storer.store( owning_relation );
        storer.store( non_owning_relations );
    }
        
}
namespace PerSourceModel
{
    // struct Interface_LinkTrait : public mega::io::Object
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_LinkTrait( loader )
          , relation( loader )
    {
    }
    Interface_LinkTrait::Interface_LinkTrait( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Tree::Interface_LinkTrait > p_Tree_Interface_LinkTrait, const data::Ptr< data::Model::HyperGraph_Relation >& relation)
        :   mega::io::Object( objectInfo )          , p_Tree_Interface_LinkTrait( p_Tree_Interface_LinkTrait )
          , relation( relation )
    {
    }
    bool Interface_LinkTrait::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Interface_LinkTrait::set_inheritance_pointer()
    {
        p_Tree_Interface_LinkTrait->p_PerSourceModel_Interface_LinkTrait = data::Ptr< data::PerSourceModel::Interface_LinkTrait >( p_Tree_Interface_LinkTrait, this );
    }
    void Interface_LinkTrait::load( mega::io::Loader& loader )
    {
        loader.load( p_Tree_Interface_LinkTrait );
        loader.load( relation );
    }
    void Interface_LinkTrait::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Tree_Interface_LinkTrait );
        storer.store( relation );
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
        
    // struct Concrete_Dimensions_Link : public mega::io::Object
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Link( loader )
          , relation( loader )
    {
    }
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Dimensions_Link > p_Concrete_Concrete_Dimensions_Link, const data::Ptr< data::Model::HyperGraph_Relation >& relation, const bool& owning, const bool& owned, const bool& source, const bool& singular)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Link( p_Concrete_Concrete_Dimensions_Link )
          , relation( relation )
          , owning( owning )
          , owned( owned )
          , source( source )
          , singular( singular )
    {
    }
    bool Concrete_Dimensions_Link::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Dimensions_Link::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_Link->p_PerSourceModel_Concrete_Dimensions_Link = data::Ptr< data::PerSourceModel::Concrete_Dimensions_Link >( p_Concrete_Concrete_Dimensions_Link, this );
    }
    void Concrete_Dimensions_Link::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_Link );
        loader.load( relation );
        loader.load( owning );
        loader.load( owned );
        loader.load( source );
        loader.load( singular );
    }
    void Concrete_Dimensions_Link::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Link );
        storer.store( relation );
        storer.store( owning );
        storer.store( owned );
        storer.store( source );
        storer.store( singular );
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
        
}
namespace PerSourceModel
{
}
namespace ConcreteTable
{
    // struct Symbols_ConcreteTypeID : public mega::io::Object
    Symbols_ConcreteTypeID::Symbols_ConcreteTypeID( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID >( loader, this ) )    {
    }
    Symbols_ConcreteTypeID::Symbols_ConcreteTypeID( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::TypeID& id, const std::optional< data::Ptr< data::Concrete::Concrete_Context > >& context, const std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_User > >& dim_user, const std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& dim_link)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID >( loader, this ) )          , id( id )
          , context( context )
          , dim_user( dim_user )
          , dim_link( dim_link )
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
    }
    void Symbols_ConcreteTypeID::store( mega::io::Storer& storer ) const
    {
        storer.store( id );
        storer.store( context );
        storer.store( dim_user );
        storer.store( dim_link );
    }
        
    // struct Symbols_SymbolTable : public mega::io::Object
    Symbols_SymbolTable::Symbols_SymbolTable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_SymbolTable_Symbols_SymbolTable( loader )
    {
    }
    Symbols_SymbolTable::Symbols_SymbolTable( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< SymbolTable::Symbols_SymbolTable > p_SymbolTable_Symbols_SymbolTable, const std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& concrete_type_id_sequences, const std::map< mega::TypeIDSequence, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& concrete_type_id_set_link, const std::map< mega::TypeID, data::Ptr< data::ConcreteTable::Symbols_ConcreteTypeID > >& concrete_type_ids)
        :   mega::io::Object( objectInfo )          , p_SymbolTable_Symbols_SymbolTable( p_SymbolTable_Symbols_SymbolTable )
          , concrete_type_id_sequences( concrete_type_id_sequences )
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
        loader.load( concrete_type_id_set_link );
        loader.load( concrete_type_ids );
    }
    void Symbols_SymbolTable::store( mega::io::Storer& storer ) const
    {
        storer.store( p_SymbolTable_Symbols_SymbolTable );
        storer.store( concrete_type_id_sequences );
        storer.store( concrete_type_id_set_link );
        storer.store( concrete_type_ids );
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
        
    // struct Concrete_Dimensions_Bitset : public mega::io::Object
    Concrete_Dimensions_Bitset::Concrete_Dimensions_Bitset( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Bitset( loader )
    {
    }
    Concrete_Dimensions_Bitset::Concrete_Dimensions_Bitset( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Dimensions_Bitset > p_Concrete_Concrete_Dimensions_Bitset, const mega::TypeID& concrete_id)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Bitset( p_Concrete_Concrete_Dimensions_Bitset )
          , concrete_id( concrete_id )
    {
    }
    bool Concrete_Dimensions_Bitset::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Dimensions_Bitset::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_Bitset->p_PerSourceConcreteTable_Concrete_Dimensions_Bitset = data::Ptr< data::PerSourceConcreteTable::Concrete_Dimensions_Bitset >( p_Concrete_Concrete_Dimensions_Bitset, this );
    }
    void Concrete_Dimensions_Bitset::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_Bitset );
        loader.load( concrete_id );
    }
    void Concrete_Dimensions_Bitset::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Bitset );
        storer.store( concrete_id );
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
        
}
namespace MemoryLayout
{
    // struct Concrete_Dimensions_User : public mega::io::Object
    Concrete_Dimensions_User::Concrete_Dimensions_User( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_User( loader )
          , part( loader )
    {
    }
    Concrete_Dimensions_User::Concrete_Dimensions_User( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Dimensions_User > p_Concrete_Concrete_Dimensions_User, const data::Ptr< data::MemoryLayout::MemoryLayout_Part >& part, const mega::U64& offset)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_User( p_Concrete_Concrete_Dimensions_User )
          , part( part )
          , offset( offset )
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
        loader.load( part );
        loader.load( offset );
    }
    void Concrete_Dimensions_User::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_User );
        storer.store( part );
        storer.store( offset );
    }
        
    // struct Concrete_Dimensions_Link : public mega::io::Object
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Link( loader )
          , part( loader )
    {
    }
    Concrete_Dimensions_Link::Concrete_Dimensions_Link( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Dimensions_Link > p_Concrete_Concrete_Dimensions_Link, const data::Ptr< data::MemoryLayout::MemoryLayout_Part >& part, const mega::U64& offset)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Link( p_Concrete_Concrete_Dimensions_Link )
          , part( part )
          , offset( offset )
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
        loader.load( part );
        loader.load( offset );
    }
    void Concrete_Dimensions_Link::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Link );
        storer.store( part );
        storer.store( offset );
    }
        
    // struct Concrete_Dimensions_LinkType : public mega::io::Object
    Concrete_Dimensions_LinkType::Concrete_Dimensions_LinkType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_LinkType( loader )
          , part( loader )
    {
    }
    Concrete_Dimensions_LinkType::Concrete_Dimensions_LinkType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Dimensions_LinkType > p_Concrete_Concrete_Dimensions_LinkType, const data::Ptr< data::MemoryLayout::MemoryLayout_Part >& part, const mega::U64& offset)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_LinkType( p_Concrete_Concrete_Dimensions_LinkType )
          , part( part )
          , offset( offset )
    {
    }
    bool Concrete_Dimensions_LinkType::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Dimensions_LinkType::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_LinkType->p_MemoryLayout_Concrete_Dimensions_LinkType = data::Ptr< data::MemoryLayout::Concrete_Dimensions_LinkType >( p_Concrete_Concrete_Dimensions_LinkType, this );
    }
    void Concrete_Dimensions_LinkType::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_LinkType );
        loader.load( part );
        loader.load( offset );
    }
    void Concrete_Dimensions_LinkType::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_LinkType );
        storer.store( part );
        storer.store( offset );
    }
        
    // struct Concrete_Dimensions_Bitset : public mega::io::Object
    Concrete_Dimensions_Bitset::Concrete_Dimensions_Bitset( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Bitset( loader )
          , part( loader )
    {
    }
    Concrete_Dimensions_Bitset::Concrete_Dimensions_Bitset( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Dimensions_Bitset > p_Concrete_Concrete_Dimensions_Bitset, const mega::U64& size, const data::Ptr< data::MemoryLayout::MemoryLayout_Part >& part, const mega::U64& offset)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Dimensions_Bitset( p_Concrete_Concrete_Dimensions_Bitset )
          , size( size )
          , part( part )
          , offset( offset )
    {
    }
    bool Concrete_Dimensions_Bitset::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Dimensions_Bitset::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Dimensions_Bitset->p_MemoryLayout_Concrete_Dimensions_Bitset = data::Ptr< data::MemoryLayout::Concrete_Dimensions_Bitset >( p_Concrete_Concrete_Dimensions_Bitset, this );
    }
    void Concrete_Dimensions_Bitset::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Dimensions_Bitset );
        loader.load( size );
        loader.load( part );
        loader.load( offset );
    }
    void Concrete_Dimensions_Bitset::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Dimensions_Bitset );
        storer.store( size );
        storer.store( part );
        storer.store( offset );
    }
        
    // struct Concrete_Context : public mega::io::Object
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Context( loader )
    {
    }
    Concrete_Context::Concrete_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Context > p_Concrete_Concrete_Context, const mega::Instance& local_size, const mega::Instance& total_size)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Context( p_Concrete_Concrete_Context )
          , local_size( local_size )
          , total_size( total_size )
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
        loader.load( local_size );
        loader.load( total_size );
    }
    void Concrete_Context::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Context );
        storer.store( local_size );
        storer.store( total_size );
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
        
    // struct MemoryLayout_Part : public mega::io::Object
    MemoryLayout_Part::MemoryLayout_Part( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::MemoryLayout_Part >( loader, this ) )          , buffer( loader )
    {
    }
    MemoryLayout_Part::MemoryLayout_Part( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::U64& total_domain_size)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::MemoryLayout::MemoryLayout_Part >( loader, this ) )          , total_domain_size( total_domain_size )
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
        loader.load( size );
        loader.load( alignment );
        loader.load( user_dimensions );
        loader.load( link_dimensions );
        loader.load( bitset_dimensions );
        loader.load( offset );
        loader.load( buffer );
    }
    void MemoryLayout_Part::store( mega::io::Storer& storer ) const
    {
        storer.store( total_domain_size );
        VERIFY_RTE_MSG( size.has_value(), "MemoryLayout::MemoryLayout_Part.size has NOT been set" );
        storer.store( size );
        VERIFY_RTE_MSG( alignment.has_value(), "MemoryLayout::MemoryLayout_Part.alignment has NOT been set" );
        storer.store( alignment );
        VERIFY_RTE_MSG( user_dimensions.has_value(), "MemoryLayout::MemoryLayout_Part.user_dimensions has NOT been set" );
        storer.store( user_dimensions );
        VERIFY_RTE_MSG( link_dimensions.has_value(), "MemoryLayout::MemoryLayout_Part.link_dimensions has NOT been set" );
        storer.store( link_dimensions );
        VERIFY_RTE_MSG( bitset_dimensions.has_value(), "MemoryLayout::MemoryLayout_Part.bitset_dimensions has NOT been set" );
        storer.store( bitset_dimensions );
        VERIFY_RTE_MSG( offset.has_value(), "MemoryLayout::MemoryLayout_Part.offset has NOT been set" );
        storer.store( offset );
        VERIFY_RTE_MSG( buffer.has_value(), "MemoryLayout::MemoryLayout_Part.buffer has NOT been set" );
        storer.store( buffer );
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
        
}
namespace AutomataAnalysis
{
    // struct Concrete_State : public mega::io::Object
    Concrete_State::Concrete_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_State( loader )
          , automata_vertex( loader )
    {
    }
    Concrete_State::Concrete_State( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_State > p_Concrete_Concrete_State, const data::Ptr< data::AutomataAnalysis::Automata_Vertex >& automata_vertex)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_State( p_Concrete_Concrete_State )
          , automata_vertex( automata_vertex )
    {
    }
    bool Concrete_State::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_State::set_inheritance_pointer()
    {
        p_Concrete_Concrete_State->p_AutomataAnalysis_Concrete_State = data::Ptr< data::AutomataAnalysis::Concrete_State >( p_Concrete_Concrete_State, this );
    }
    void Concrete_State::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_State );
        loader.load( automata_vertex );
    }
    void Concrete_State::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_State );
        storer.store( automata_vertex );
    }
        
    // struct Concrete_Object : public mega::io::Object
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Object( loader )
          , automata_root( loader )
          , automata_enum( loader )
    {
    }
    Concrete_Object::Concrete_Object( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, Ptr< Concrete::Concrete_Object > p_Concrete_Concrete_Object, const data::Ptr< data::AutomataAnalysis::Automata_Vertex >& automata_root, const data::Ptr< data::AutomataAnalysis::Automata_Enum >& automata_enum, const mega::U64& total_index)
        :   mega::io::Object( objectInfo )          , p_Concrete_Concrete_Object( p_Concrete_Concrete_Object )
          , automata_root( automata_root )
          , automata_enum( automata_enum )
          , total_index( total_index )
    {
    }
    bool Concrete_Object::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return false;
    }
    void Concrete_Object::set_inheritance_pointer()
    {
        p_Concrete_Concrete_Object->p_AutomataAnalysis_Concrete_Object = data::Ptr< data::AutomataAnalysis::Concrete_Object >( p_Concrete_Concrete_Object, this );
    }
    void Concrete_Object::load( mega::io::Loader& loader )
    {
        loader.load( p_Concrete_Concrete_Object );
        loader.load( automata_root );
        loader.load( automata_enum );
        loader.load( total_index );
    }
    void Concrete_Object::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Concrete_Concrete_Object );
        storer.store( automata_root );
        storer.store( automata_enum );
        storer.store( total_index );
    }
        
    // struct Automata_Vertex : public mega::io::Object
    Automata_Vertex::Automata_Vertex( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AutomataAnalysis::Automata_Vertex >( loader, this ) )          , context( loader )
    {
    }
    Automata_Vertex::Automata_Vertex( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::U32& index_base, const mega::U32& relative_domain, const data::Ptr< data::Concrete::Concrete_Context >& context, const std::vector< data::Ptr< data::AutomataAnalysis::Automata_Vertex > >& children)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AutomataAnalysis::Automata_Vertex >( loader, this ) )          , index_base( index_base )
          , relative_domain( relative_domain )
          , context( context )
          , children( children )
    {
    }
    bool Automata_Vertex::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AutomataAnalysis::Automata_Vertex >( loader, const_cast< Automata_Vertex* >( this ) ) };
    }
    void Automata_Vertex::set_inheritance_pointer()
    {
    }
    void Automata_Vertex::load( mega::io::Loader& loader )
    {
        loader.load( index_base );
        loader.load( relative_domain );
        loader.load( context );
        loader.load( children );
    }
    void Automata_Vertex::store( mega::io::Storer& storer ) const
    {
        storer.store( index_base );
        storer.store( relative_domain );
        storer.store( context );
        storer.store( children );
    }
        
    // struct Automata_And : public mega::io::Object
    Automata_And::Automata_And( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AutomataAnalysis::Automata_And >( loader, this ) )          , p_AutomataAnalysis_Automata_Vertex( loader )
    {
    }
    bool Automata_And::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AutomataAnalysis::Automata_And >( loader, const_cast< Automata_And* >( this ) ) };
    }
    void Automata_And::set_inheritance_pointer()
    {
        p_AutomataAnalysis_Automata_Vertex->m_inheritance = data::Ptr< data::AutomataAnalysis::Automata_And >( p_AutomataAnalysis_Automata_Vertex, this );
    }
    void Automata_And::load( mega::io::Loader& loader )
    {
        loader.load( p_AutomataAnalysis_Automata_Vertex );
    }
    void Automata_And::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AutomataAnalysis_Automata_Vertex );
    }
        
    // struct Automata_Or : public mega::io::Object
    Automata_Or::Automata_Or( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AutomataAnalysis::Automata_Or >( loader, this ) )          , p_AutomataAnalysis_Automata_Vertex( loader )
    {
    }
    bool Automata_Or::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AutomataAnalysis::Automata_Or >( loader, const_cast< Automata_Or* >( this ) ) };
    }
    void Automata_Or::set_inheritance_pointer()
    {
        p_AutomataAnalysis_Automata_Vertex->m_inheritance = data::Ptr< data::AutomataAnalysis::Automata_Or >( p_AutomataAnalysis_Automata_Vertex, this );
    }
    void Automata_Or::load( mega::io::Loader& loader )
    {
        loader.load( p_AutomataAnalysis_Automata_Vertex );
    }
    void Automata_Or::store( mega::io::Storer& storer ) const
    {
        storer.store( p_AutomataAnalysis_Automata_Vertex );
    }
        
    // struct Automata_Enum : public mega::io::Object
    Automata_Enum::Automata_Enum( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AutomataAnalysis::Automata_Enum >( loader, this ) )    {
    }
    Automata_Enum::Automata_Enum( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::U32& switch_index, const mega::U32& bitset_index, const std::vector< mega::U32 >& indices, const bool& is_or, const bool& has_action, const mega::SubTypeInstance& sub_type_instance, const std::vector< data::Ptr< data::AutomataAnalysis::Automata_Enum > >& children)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::AutomataAnalysis::Automata_Enum >( loader, this ) )          , switch_index( switch_index )
          , bitset_index( bitset_index )
          , indices( indices )
          , is_or( is_or )
          , has_action( has_action )
          , sub_type_instance( sub_type_instance )
          , children( children )
    {
    }
    bool Automata_Enum::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::AutomataAnalysis::Automata_Enum >( loader, const_cast< Automata_Enum* >( this ) ) };
    }
    void Automata_Enum::set_inheritance_pointer()
    {
    }
    void Automata_Enum::load( mega::io::Loader& loader )
    {
        loader.load( switch_index );
        loader.load( bitset_index );
        loader.load( indices );
        loader.load( is_or );
        loader.load( has_action );
        loader.load( sub_type_instance );
        loader.load( children );
    }
    void Automata_Enum::store( mega::io::Storer& storer ) const
    {
        storer.store( switch_index );
        storer.store( bitset_index );
        storer.store( indices );
        storer.store( is_or );
        storer.store( has_action );
        storer.store( sub_type_instance );
        storer.store( children );
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
        
    // struct Invocations_Variables_LinkType : public mega::io::Object
    Invocations_Variables_LinkType::Invocations_Variables_LinkType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_LinkType >( loader, this ) )          , p_Operations_Invocations_Variables_Variable( loader )
          , link_type( loader )
    {
    }
    Invocations_Variables_LinkType::Invocations_Variables_LinkType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Concrete::Concrete_Dimensions_LinkType >& link_type)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Variables_LinkType >( loader, this ) )          , p_Operations_Invocations_Variables_Variable( loader )
          , link_type( link_type )
    {
    }
    bool Invocations_Variables_LinkType::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Variables_LinkType >( loader, const_cast< Invocations_Variables_LinkType* >( this ) ) };
    }
    void Invocations_Variables_LinkType::set_inheritance_pointer()
    {
        p_Operations_Invocations_Variables_Variable->m_inheritance = data::Ptr< data::Operations::Invocations_Variables_LinkType >( p_Operations_Invocations_Variables_Variable, this );
    }
    void Invocations_Variables_LinkType::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Variables_Variable );
        loader.load( link_type );
    }
    void Invocations_Variables_LinkType::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Variables_Variable );
        storer.store( link_type );
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
        
    // struct Invocations_Instructions_Dereference : public mega::io::Object
    Invocations_Instructions_Dereference::Invocations_Instructions_Dereference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Dereference >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , instance( loader )
          , ref( loader )
          , link_type( loader )
          , link_dimension( loader )
    {
    }
    Invocations_Instructions_Dereference::Invocations_Instructions_Dereference( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Variable >& instance, const data::Ptr< data::Operations::Invocations_Variables_Memory >& ref, const data::Ptr< data::Operations::Invocations_Variables_LinkType >& link_type, const data::Ptr< data::Concrete::Concrete_Dimensions_Link >& link_dimension)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_Dereference >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , instance( instance )
          , ref( ref )
          , link_type( link_type )
          , link_dimension( link_dimension )
    {
    }
    bool Invocations_Instructions_Dereference::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Instructions_Dereference >( loader, const_cast< Invocations_Instructions_Dereference* >( this ) ) };
    }
    void Invocations_Instructions_Dereference::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_Dereference >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_Dereference::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( instance );
        loader.load( ref );
        loader.load( link_type );
        loader.load( link_dimension );
    }
    void Invocations_Instructions_Dereference::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( instance );
        storer.store( ref );
        storer.store( link_type );
        storer.store( link_dimension );
    }
        
    // struct Invocations_Instructions_LinkBranch : public mega::io::Object
    Invocations_Instructions_LinkBranch::Invocations_Instructions_LinkBranch( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_LinkBranch >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , link_type( loader )
    {
    }
    Invocations_Instructions_LinkBranch::Invocations_Instructions_LinkBranch( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_LinkType >& link_type)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_LinkBranch >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , link_type( link_type )
    {
    }
    bool Invocations_Instructions_LinkBranch::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Invocations_Instructions_LinkBranch >( loader, const_cast< Invocations_Instructions_LinkBranch* >( this ) ) };
    }
    void Invocations_Instructions_LinkBranch::set_inheritance_pointer()
    {
        p_Operations_Invocations_Instructions_InstructionGroup->m_inheritance = data::Ptr< data::Operations::Invocations_Instructions_LinkBranch >( p_Operations_Invocations_Instructions_InstructionGroup, this );
    }
    void Invocations_Instructions_LinkBranch::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Invocations_Instructions_InstructionGroup );
        loader.load( link_type );
    }
    void Invocations_Instructions_LinkBranch::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( link_type );
    }
        
    // struct Invocations_Instructions_PolyBranch : public mega::io::Object
    Invocations_Instructions_PolyBranch::Invocations_Instructions_PolyBranch( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_PolyBranch >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , parameter( loader )
    {
    }
    Invocations_Instructions_PolyBranch::Invocations_Instructions_PolyBranch( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Parameter >& parameter)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_PolyBranch >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , parameter( parameter )
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
        loader.load( parameter );
    }
    void Invocations_Instructions_PolyBranch::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_InstructionGroup );
        storer.store( parameter );
    }
        
    // struct Invocations_Instructions_PolyCase : public mega::io::Object
    Invocations_Instructions_PolyCase::Invocations_Instructions_PolyCase( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Instructions_PolyCase >( loader, this ) )          , p_Operations_Invocations_Instructions_InstructionGroup( loader )
          , reference( loader )
          , type( loader )
    {
    }
    Invocations_Instructions_PolyCase::Invocations_Instructions_PolyCase( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Variable >& reference, const data::Ptr< data::Concrete::Concrete_Graph_Vertex >& type)
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
        
    // struct Invocations_Operations_Operation : public mega::io::Object
    Invocations_Operations_Operation::Invocations_Operations_Operation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Operation >( loader, this ) )          , p_Operations_Invocations_Instructions_Instruction( loader )
          , variable( loader )
          , context( loader )
    {
    }
    Invocations_Operations_Operation::Invocations_Operations_Operation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Invocations_Variables_Variable >& variable, const data::Ptr< data::Concrete::Concrete_Graph_Vertex >& context)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Invocations_Operations_Operation >( loader, this ) )          , p_Operations_Invocations_Instructions_Instruction( loader )
          , variable( variable )
          , context( context )
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
        loader.load( context );
    }
    void Invocations_Operations_Operation::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Invocations_Instructions_Instruction );
        storer.store( variable );
        storer.store( context );
    }
        
    // struct Derivation_Edge : public mega::io::Object
    Derivation_Edge::Derivation_Edge( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Derivation_Edge >( loader, this ) )          , next( loader )
    {
    }
    Derivation_Edge::Derivation_Edge( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Derivation_Step >& next, const bool& eliminated, const bool& backtracked, const int& precedence, const std::vector< data::Ptr< data::Model::Concrete_Graph_Edge > >& edges)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Derivation_Edge >( loader, this ) )          , next( next )
          , eliminated( eliminated )
          , backtracked( backtracked )
          , precedence( precedence )
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
        loader.load( backtracked );
        loader.load( precedence );
        loader.load( edges );
    }
    void Derivation_Edge::store( mega::io::Storer& storer ) const
    {
        storer.store( next );
        storer.store( eliminated );
        storer.store( backtracked );
        storer.store( precedence );
        storer.store( edges );
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
        
    // struct Operations_ReturnTypes_ReturnType : public mega::io::Object
    Operations_ReturnTypes_ReturnType::Operations_ReturnTypes_ReturnType( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ReturnTypes_ReturnType >( loader, this ) )    {
    }
    bool Operations_ReturnTypes_ReturnType::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_ReturnTypes_ReturnType >( loader, const_cast< Operations_ReturnTypes_ReturnType* >( this ) ) };
    }
    void Operations_ReturnTypes_ReturnType::set_inheritance_pointer()
    {
    }
    void Operations_ReturnTypes_ReturnType::load( mega::io::Loader& loader )
    {
        loader.load( canonical_type );
    }
    void Operations_ReturnTypes_ReturnType::store( mega::io::Storer& storer ) const
    {
        VERIFY_RTE_MSG( canonical_type.has_value(), "Operations::Operations_ReturnTypes_ReturnType.canonical_type has NOT been set" );
        storer.store( canonical_type );
    }
        
    // struct Operations_ReturnTypes_Void : public mega::io::Object
    Operations_ReturnTypes_Void::Operations_ReturnTypes_Void( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ReturnTypes_Void >( loader, this ) )          , p_Operations_Operations_ReturnTypes_ReturnType( loader )
    {
    }
    bool Operations_ReturnTypes_Void::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_ReturnTypes_Void >( loader, const_cast< Operations_ReturnTypes_Void* >( this ) ) };
    }
    void Operations_ReturnTypes_Void::set_inheritance_pointer()
    {
        p_Operations_Operations_ReturnTypes_ReturnType->m_inheritance = data::Ptr< data::Operations::Operations_ReturnTypes_Void >( p_Operations_Operations_ReturnTypes_ReturnType, this );
    }
    void Operations_ReturnTypes_Void::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_ReturnTypes_ReturnType );
    }
    void Operations_ReturnTypes_Void::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_ReturnTypes_ReturnType );
    }
        
    // struct Operations_ReturnTypes_Dimension : public mega::io::Object
    Operations_ReturnTypes_Dimension::Operations_ReturnTypes_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ReturnTypes_Dimension >( loader, this ) )          , p_Operations_Operations_ReturnTypes_ReturnType( loader )
    {
    }
    Operations_ReturnTypes_Dimension::Operations_ReturnTypes_Dimension( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > >& dimensions, const bool& homogeneous)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ReturnTypes_Dimension >( loader, this ) )          , p_Operations_Operations_ReturnTypes_ReturnType( loader )
          , dimensions( dimensions )
          , homogeneous( homogeneous )
    {
    }
    bool Operations_ReturnTypes_Dimension::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_ReturnTypes_Dimension >( loader, const_cast< Operations_ReturnTypes_Dimension* >( this ) ) };
    }
    void Operations_ReturnTypes_Dimension::set_inheritance_pointer()
    {
        p_Operations_Operations_ReturnTypes_ReturnType->m_inheritance = data::Ptr< data::Operations::Operations_ReturnTypes_Dimension >( p_Operations_Operations_ReturnTypes_ReturnType, this );
    }
    void Operations_ReturnTypes_Dimension::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_ReturnTypes_ReturnType );
        loader.load( dimensions );
        loader.load( homogeneous );
    }
    void Operations_ReturnTypes_Dimension::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_ReturnTypes_ReturnType );
        storer.store( dimensions );
        storer.store( homogeneous );
    }
        
    // struct Operations_ReturnTypes_Function : public mega::io::Object
    Operations_ReturnTypes_Function::Operations_ReturnTypes_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ReturnTypes_Function >( loader, this ) )          , p_Operations_Operations_ReturnTypes_ReturnType( loader )
    {
    }
    Operations_ReturnTypes_Function::Operations_ReturnTypes_Function( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Tree::Interface_Function > >& functions, const bool& homogeneous)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ReturnTypes_Function >( loader, this ) )          , p_Operations_Operations_ReturnTypes_ReturnType( loader )
          , functions( functions )
          , homogeneous( homogeneous )
    {
    }
    bool Operations_ReturnTypes_Function::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_ReturnTypes_Function >( loader, const_cast< Operations_ReturnTypes_Function* >( this ) ) };
    }
    void Operations_ReturnTypes_Function::set_inheritance_pointer()
    {
        p_Operations_Operations_ReturnTypes_ReturnType->m_inheritance = data::Ptr< data::Operations::Operations_ReturnTypes_Function >( p_Operations_Operations_ReturnTypes_ReturnType, this );
    }
    void Operations_ReturnTypes_Function::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_ReturnTypes_ReturnType );
        loader.load( functions );
        loader.load( homogeneous );
    }
    void Operations_ReturnTypes_Function::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_ReturnTypes_ReturnType );
        storer.store( functions );
        storer.store( homogeneous );
    }
        
    // struct Operations_ReturnTypes_Context : public mega::io::Object
    Operations_ReturnTypes_Context::Operations_ReturnTypes_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ReturnTypes_Context >( loader, this ) )          , p_Operations_Operations_ReturnTypes_ReturnType( loader )
    {
    }
    Operations_ReturnTypes_Context::Operations_ReturnTypes_Context( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::vector< data::Ptr< data::Tree::Interface_IContext > >& contexts)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ReturnTypes_Context >( loader, this ) )          , p_Operations_Operations_ReturnTypes_ReturnType( loader )
          , contexts( contexts )
    {
    }
    bool Operations_ReturnTypes_Context::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_ReturnTypes_Context >( loader, const_cast< Operations_ReturnTypes_Context* >( this ) ) };
    }
    void Operations_ReturnTypes_Context::set_inheritance_pointer()
    {
        p_Operations_Operations_ReturnTypes_ReturnType->m_inheritance = data::Ptr< data::Operations::Operations_ReturnTypes_Context >( p_Operations_Operations_ReturnTypes_ReturnType, this );
    }
    void Operations_ReturnTypes_Context::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_ReturnTypes_ReturnType );
        loader.load( contexts );
    }
    void Operations_ReturnTypes_Context::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_ReturnTypes_ReturnType );
        storer.store( contexts );
    }
        
    // struct Operations_ReturnTypes_Range : public mega::io::Object
    Operations_ReturnTypes_Range::Operations_ReturnTypes_Range( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_ReturnTypes_Range >( loader, this ) )          , p_Operations_Operations_ReturnTypes_Context( loader )
    {
    }
    bool Operations_ReturnTypes_Range::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_ReturnTypes_Range >( loader, const_cast< Operations_ReturnTypes_Range* >( this ) ) };
    }
    void Operations_ReturnTypes_Range::set_inheritance_pointer()
    {
        p_Operations_Operations_ReturnTypes_Context->m_inheritance = data::Ptr< data::Operations::Operations_ReturnTypes_Range >( p_Operations_Operations_ReturnTypes_Context, this );
    }
    void Operations_ReturnTypes_Range::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_ReturnTypes_Context );
    }
    void Operations_ReturnTypes_Range::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_ReturnTypes_Context );
    }
        
    // struct Operations_Operator : public mega::io::Object
    Operations_Operator::Operations_Operator( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Operator >( loader, this ) )    {
    }
    Operations_Operator::Operations_Operator( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const mega::OperatorID& id)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Operator >( loader, this ) )          , id( id )
    {
    }
    bool Operations_Operator::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Operator >( loader, const_cast< Operations_Operator* >( this ) ) };
    }
    void Operations_Operator::set_inheritance_pointer()
    {
    }
    void Operations_Operator::load( mega::io::Loader& loader )
    {
        loader.load( id );
    }
    void Operations_Operator::store( mega::io::Storer& storer ) const
    {
        storer.store( id );
    }
        
    // struct Operations_New : public mega::io::Object
    Operations_New::Operations_New( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_New >( loader, this ) )          , p_Operations_Operations_Operator( loader )
    {
    }
    bool Operations_New::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_New >( loader, const_cast< Operations_New* >( this ) ) };
    }
    void Operations_New::set_inheritance_pointer()
    {
        p_Operations_Operations_Operator->m_inheritance = data::Ptr< data::Operations::Operations_New >( p_Operations_Operations_Operator, this );
    }
    void Operations_New::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Operator );
    }
    void Operations_New::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Operator );
    }
        
    // struct Operations_Delete : public mega::io::Object
    Operations_Delete::Operations_Delete( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Delete >( loader, this ) )          , p_Operations_Operations_Operator( loader )
    {
    }
    bool Operations_Delete::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Delete >( loader, const_cast< Operations_Delete* >( this ) ) };
    }
    void Operations_Delete::set_inheritance_pointer()
    {
        p_Operations_Operations_Operator->m_inheritance = data::Ptr< data::Operations::Operations_Delete >( p_Operations_Operations_Operator, this );
    }
    void Operations_Delete::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Operator );
    }
    void Operations_Delete::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Operator );
    }
        
    // struct Operations_Cast : public mega::io::Object
    Operations_Cast::Operations_Cast( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Cast >( loader, this ) )          , p_Operations_Operations_Operator( loader )
    {
    }
    bool Operations_Cast::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Cast >( loader, const_cast< Operations_Cast* >( this ) ) };
    }
    void Operations_Cast::set_inheritance_pointer()
    {
        p_Operations_Operations_Operator->m_inheritance = data::Ptr< data::Operations::Operations_Cast >( p_Operations_Operations_Operator, this );
    }
    void Operations_Cast::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Operator );
    }
    void Operations_Cast::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Operator );
    }
        
    // struct Operations_Active : public mega::io::Object
    Operations_Active::Operations_Active( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Active >( loader, this ) )          , p_Operations_Operations_Operator( loader )
    {
    }
    bool Operations_Active::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Active >( loader, const_cast< Operations_Active* >( this ) ) };
    }
    void Operations_Active::set_inheritance_pointer()
    {
        p_Operations_Operations_Operator->m_inheritance = data::Ptr< data::Operations::Operations_Active >( p_Operations_Operations_Operator, this );
    }
    void Operations_Active::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Operator );
    }
    void Operations_Active::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Operator );
    }
        
    // struct Operations_Enabled : public mega::io::Object
    Operations_Enabled::Operations_Enabled( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Enabled >( loader, this ) )          , p_Operations_Operations_Operator( loader )
    {
    }
    bool Operations_Enabled::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Enabled >( loader, const_cast< Operations_Enabled* >( this ) ) };
    }
    void Operations_Enabled::set_inheritance_pointer()
    {
        p_Operations_Operations_Operator->m_inheritance = data::Ptr< data::Operations::Operations_Enabled >( p_Operations_Operations_Operator, this );
    }
    void Operations_Enabled::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Operator );
    }
    void Operations_Enabled::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Operator );
    }
        
    // struct Operations_Invocation : public mega::io::Object
    Operations_Invocation::Operations_Invocation( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Invocation >( loader, this ) )          , derivation( loader )
          , root_instruction( loader )
          , return_type( loader )
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
        loader.load( explicit_operation );
        loader.load( return_type );
        loader.load( file_offsets );
        loader.load( canonical_context );
        loader.load( canonical_type_path );
        loader.load( canonical_operation );
    }
    void Operations_Invocation::store( mega::io::Storer& storer ) const
    {
        storer.store( id );
        storer.store( derivation );
        VERIFY_RTE_MSG( root_instruction.has_value(), "Operations::Operations_Invocation.root_instruction has NOT been set" );
        storer.store( root_instruction );
        storer.store( variables );
        storer.store( operations );
        VERIFY_RTE_MSG( explicit_operation.has_value(), "Operations::Operations_Invocation.explicit_operation has NOT been set" );
        storer.store( explicit_operation );
        VERIFY_RTE_MSG( return_type.has_value(), "Operations::Operations_Invocation.return_type has NOT been set" );
        storer.store( return_type );
        VERIFY_RTE_MSG( file_offsets.has_value(), "Operations::Operations_Invocation.file_offsets has NOT been set" );
        storer.store( file_offsets );
        VERIFY_RTE_MSG( canonical_context.has_value(), "Operations::Operations_Invocation.canonical_context has NOT been set" );
        storer.store( canonical_context );
        VERIFY_RTE_MSG( canonical_type_path.has_value(), "Operations::Operations_Invocation.canonical_type_path has NOT been set" );
        storer.store( canonical_type_path );
        VERIFY_RTE_MSG( canonical_operation.has_value(), "Operations::Operations_Invocation.canonical_operation has NOT been set" );
        storer.store( canonical_operation );
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
        
    // struct Operations_Signal : public mega::io::Object
    Operations_Signal::Operations_Signal( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Signal >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_Signal::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_Signal >( loader, const_cast< Operations_Signal* >( this ) ) };
    }
    void Operations_Signal::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_Signal >( p_Operations_Operations_Invocation, this );
    }
    void Operations_Signal::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_Signal::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
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
        
    // struct Operations_GetContext : public mega::io::Object
    Operations_GetContext::Operations_GetContext( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_GetContext >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_GetContext::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_GetContext >( loader, const_cast< Operations_GetContext* >( this ) ) };
    }
    void Operations_GetContext::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_GetContext >( p_Operations_Operations_Invocation, this );
    }
    void Operations_GetContext::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_GetContext::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
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
        
    // struct Operations_Write : public mega::io::Object
    Operations_Write::Operations_Write( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Write >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
          , parameter_type( loader )
    {
    }
    Operations_Write::Operations_Write( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const data::Ptr< data::Operations::Operations_ReturnTypes_ReturnType >& parameter_type)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Write >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
          , parameter_type( parameter_type )
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
        loader.load( parameter_type );
    }
    void Operations_Write::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
        storer.store( parameter_type );
    }
        
    // struct Operations_LinkRead : public mega::io::Object
    Operations_LinkRead::Operations_LinkRead( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_LinkRead >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_LinkRead::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_LinkRead >( loader, const_cast< Operations_LinkRead* >( this ) ) };
    }
    void Operations_LinkRead::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_LinkRead >( p_Operations_Operations_Invocation, this );
    }
    void Operations_LinkRead::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_LinkRead::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
        
    // struct Operations_LinkAdd : public mega::io::Object
    Operations_LinkAdd::Operations_LinkAdd( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_LinkAdd >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_LinkAdd::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_LinkAdd >( loader, const_cast< Operations_LinkAdd* >( this ) ) };
    }
    void Operations_LinkAdd::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_LinkAdd >( p_Operations_Operations_Invocation, this );
    }
    void Operations_LinkAdd::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_LinkAdd::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
        
    // struct Operations_LinkRemove : public mega::io::Object
    Operations_LinkRemove::Operations_LinkRemove( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_LinkRemove >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_LinkRemove::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_LinkRemove >( loader, const_cast< Operations_LinkRemove* >( this ) ) };
    }
    void Operations_LinkRemove::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_LinkRemove >( p_Operations_Operations_Invocation, this );
    }
    void Operations_LinkRemove::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_LinkRemove::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
    }
        
    // struct Operations_LinkClear : public mega::io::Object
    Operations_LinkClear::Operations_LinkClear( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_LinkClear >( loader, this ) )          , p_Operations_Operations_Invocation( loader )
    {
    }
    bool Operations_LinkClear::test_inheritance_pointer( ObjectPartLoader &loader ) const
    {
        return m_inheritance == data::Variant{ data::Ptr< data::Operations::Operations_LinkClear >( loader, const_cast< Operations_LinkClear* >( this ) ) };
    }
    void Operations_LinkClear::set_inheritance_pointer()
    {
        p_Operations_Operations_Invocation->m_inheritance = data::Ptr< data::Operations::Operations_LinkClear >( p_Operations_Operations_Invocation, this );
    }
    void Operations_LinkClear::load( mega::io::Loader& loader )
    {
        loader.load( p_Operations_Operations_Invocation );
    }
    void Operations_LinkClear::store( mega::io::Storer& storer ) const
    {
        storer.store( p_Operations_Operations_Invocation );
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
        
    // struct Operations_Invocations : public mega::io::Object
    Operations_Invocations::Operations_Invocations( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo )
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Invocations >( loader, this ) )    {
    }
    Operations_Invocations::Operations_Invocations( ObjectPartLoader& loader, const mega::io::ObjectInfo& objectInfo, const std::map< mega::InvocationID, data::Ptr< data::Operations::Operations_Invocation > >& invocations, const std::map< mega::OperatorID, data::Ptr< data::Operations::Operations_Operator > >& operators)
        :   mega::io::Object( objectInfo ), m_inheritance( data::Ptr< data::Operations::Operations_Invocations >( loader, this ) )          , invocations( invocations )
          , operators( operators )
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
        loader.load( operators );
    }
    void Operations_Invocations::store( mega::io::Storer& storer ) const
    {
        storer.store( invocations );
        storer.store( operators );
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
std::vector< data::Ptr< data::AutomataAnalysis::Automata_Enum > >& Automata_Enum_push_back_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::U32 >& Automata_Enum_push_back_indices(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->indices;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AutomataAnalysis::Automata_Vertex > >& Automata_Vertex_push_back_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Vertex::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->children;
        case data::AutomataAnalysis::Automata_And::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->children;
        case data::AutomataAnalysis::Automata_Or::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->children;
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
std::vector< data::Ptr< data::Tree::Interface_IContext > >& Concrete_Context_push_back_inheritance(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->inheritance;
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Bitset > >& Concrete_UserDimensionContext_push_back_bitsets(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
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
std::map< data::Ptr< data::Tree::Interface_UserLinkTrait >, data::Ptr< data::Model::HyperGraph_NonOwningObjectRelation > >& HyperGraph_Graph_insert_non_owning_relations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_Graph::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Graph >( m_data )->non_owning_relations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::multimap< data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink >, data::Ptr< data::Tree::Interface_UserLinkTrait > >& HyperGraph_OwningObjectRelation_insert_owned(data::Variant& m_data)
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
std::multimap< data::Ptr< data::Tree::Interface_UserLinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink > >& HyperGraph_OwningObjectRelation_insert_owners(data::Variant& m_data)
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
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& Inheritance_Mapping_insert_inheritance_links(data::Variant& m_data)
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
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& Inheritance_ObjectMapping_insert_inheritance_links(data::Variant& m_data)
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_DimensionTrait >( m_data )->concrete;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->symbols;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& Interface_LinkTrait_push_back_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Tree::Interface_OwnershipLinkTrait::Object_Part_Type_ID:
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
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& Interface_UserLinkTrait_push_back_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_UserLinkTrait >( m_data )->tuple;
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
        case data::Operations::Invocations_Instructions_Dereference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_LinkBranch::Object_Part_Type_ID:
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
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
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
std::optional< std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Bitset > > >& MemoryLayout_Part_push_back_bitset_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->bitset_dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > > >& MemoryLayout_Part_push_back_link_dimensions(data::Variant& m_data)
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
std::optional< std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > > >& MemoryLayout_Part_push_back_user_dimensions(data::Variant& m_data)
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
std::optional< std::vector< mega::SourceLocation > >& Operations_Invocation_push_back_file_offsets(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
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
std::map< mega::OperatorID, data::Ptr< data::Operations::Operations_Operator > >& Operations_Invocations_insert_operators(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocations::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocations >( m_data )->operators;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& Operations_ReturnTypes_Context_push_back_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_Context::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Context >( m_data )->contexts;
        case data::Operations::Operations_ReturnTypes_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Context >( m_data )->contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > >& Operations_ReturnTypes_Dimension_push_back_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_Dimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Dimension >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_Function > >& Operations_ReturnTypes_Function_push_back_functions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_Function::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Function >( m_data )->functions;
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
mega::U32& get_Automata_Enum_bitset_index(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->bitset_index;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AutomataAnalysis::Automata_Enum > >& get_Automata_Enum_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Automata_Enum_has_action(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->has_action;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::U32 >& get_Automata_Enum_indices(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->indices;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Automata_Enum_is_or(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->is_or;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::SubTypeInstance& get_Automata_Enum_sub_type_instance(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->sub_type_instance;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U32& get_Automata_Enum_switch_index(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->switch_index;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AutomataAnalysis::Automata_Vertex > >& get_Automata_Vertex_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Vertex::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->children;
        case data::AutomataAnalysis::Automata_And::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->children;
        case data::AutomataAnalysis::Automata_Or::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& get_Automata_Vertex_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Vertex::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->context;
        case data::AutomataAnalysis::Automata_And::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->context;
        case data::AutomataAnalysis::Automata_Or::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U32& get_Automata_Vertex_index_base(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Vertex::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->index_base;
        case data::AutomataAnalysis::Automata_And::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->index_base;
        case data::AutomataAnalysis::Automata_Or::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->index_base;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U32& get_Automata_Vertex_relative_domain(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Vertex::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->relative_domain;
        case data::AutomataAnalysis::Automata_And::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->relative_domain;
        case data::AutomataAnalysis::Automata_Or::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->relative_domain;
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
        case data::Concrete::Concrete_Root::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_ContextGroup >( m_data )->children;
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
mega::Instance& get_Concrete_Context_local_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
mega::Instance& get_Concrete_Context_total_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& get_Concrete_Dimensions_Bitset_concrete_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Bitset >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Bitset >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Bitset >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Bitset >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Bitset >( m_data )->concrete_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_CompilerDimensionTrait >& get_Concrete_Dimensions_Bitset_interface_compiler_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->interface_compiler_dimension;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->interface_compiler_dimension;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->interface_compiler_dimension;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->interface_compiler_dimension;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->interface_compiler_dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Concrete_Dimensions_Bitset_offset(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->offset;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Object >& get_Concrete_Dimensions_Bitset_parent_object(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->parent_object;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->parent_object;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->parent_object;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->parent_object;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->parent_object;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::MemoryLayout::MemoryLayout_Part >& get_Concrete_Dimensions_Bitset_part(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->part;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Concrete_Dimensions_Bitset_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->size;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->size;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->size;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->size;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Dimensions_Link >& get_Concrete_Dimensions_LinkType_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_LinkType::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_LinkType >( m_data )->link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& get_Concrete_Dimensions_LinkType_offset(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_LinkType::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_LinkType >( m_data )->offset;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& get_Concrete_Dimensions_LinkType_parent_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_LinkType::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_LinkType >( m_data )->parent_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::MemoryLayout::MemoryLayout_Part >& get_Concrete_Dimensions_LinkType_part(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_LinkType::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_LinkType >( m_data )->part;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_LinkTrait >& get_Concrete_Dimensions_Link_interface_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->interface_link;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->interface_link;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->interface_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_LinkType > >& get_Concrete_Dimensions_Link_link_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->link_type;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->link_type;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->link_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Concrete_Dimensions_Link_owned(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owned;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owned;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owned;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Concrete_Dimensions_Link_owning(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owning;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owning;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owning;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& get_Concrete_Dimensions_Link_parent_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->parent_context;
        case data::Concrete::Concrete_Dimensions_User::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_User >( m_data )->parent_context;
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
data::Ptr< data::Model::HyperGraph_Relation >& get_Concrete_Dimensions_Link_relation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->relation;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->relation;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->relation;
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
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->singular;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->singular;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->singular;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Concrete_Dimensions_Link_source(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->source;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->source;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->source;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_OwnershipLinkTrait >& get_Concrete_Dimensions_OwnershipLink_interface_owner_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_OwnershipLink >( m_data )->interface_owner_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_UserLinkTrait >& get_Concrete_Dimensions_UserLink_interface_user_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_UserLink >( m_data )->interface_user_link;
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_Activation > >& get_Concrete_Object_activation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->activation;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->activation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AutomataAnalysis::Automata_Enum >& get_Concrete_Object_automata_enum(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->automata_enum;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->automata_enum;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AutomataAnalysis::Automata_Vertex >& get_Concrete_Object_automata_root(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->automata_root;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->automata_root;
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
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_Configuration > >& get_Concrete_Object_configuration(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->configuration;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->configuration;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_Enablement > >& get_Concrete_Object_enablement(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->enablement;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->enablement;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_History > >& get_Concrete_Object_history(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->history;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->history;
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
mega::U64& get_Concrete_Object_total_index(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->total_index;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->total_index;
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
data::Ptr< data::AutomataAnalysis::Automata_Vertex >& get_Concrete_State_automata_vertex(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_State >( m_data )->automata_vertex;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_State >( m_data )->automata_vertex;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_State >( m_data )->automata_vertex;
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
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Bitset > >& get_Concrete_UserDimensionContext_bitsets(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
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
bool& get_Derivation_Edge_backtracked(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Edge::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Edge >( m_data )->backtracked;
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
int& get_Derivation_Edge_precedence(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Edge::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Edge >( m_data )->precedence;
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
std::map< data::Ptr< data::Tree::Interface_UserLinkTrait >, data::Ptr< data::Model::HyperGraph_NonOwningObjectRelation > >& get_HyperGraph_Graph_non_owning_relations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_Graph::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Graph >( m_data )->non_owning_relations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Model::HyperGraph_OwningObjectRelation >& get_HyperGraph_Graph_owning_relation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_Graph::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Graph >( m_data )->owning_relation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_UserLinkTrait >& get_HyperGraph_NonOwningObjectRelation_source(data::Variant& m_data)
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
data::Ptr< data::Tree::Interface_UserLinkTrait >& get_HyperGraph_NonOwningObjectRelation_target(data::Variant& m_data)
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
std::multimap< data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink >, data::Ptr< data::Tree::Interface_UserLinkTrait > >& get_HyperGraph_OwningObjectRelation_owned(data::Variant& m_data)
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
std::multimap< data::Ptr< data::Tree::Interface_UserLinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink > >& get_HyperGraph_OwningObjectRelation_owners(data::Variant& m_data)
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
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& get_Inheritance_Mapping_inheritance_links(data::Variant& m_data)
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
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& get_Inheritance_ObjectMapping_inheritance_links(data::Variant& m_data)
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
std::string& get_Interface_CompilerDimensionTrait_identifier(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_CompilerDimensionTrait >( m_data )->identifier;
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
mega::U64& get_Interface_DimensionTrait_alignment(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_DimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->alignment;
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->alignment;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->canonical_type;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_DimensionTrait >( m_data )->concrete;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->erased_type;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_DimensionTrait >( m_data )->interface_id;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->mangle;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_DimensionTrait >( m_data )->parent;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->simple;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->size;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_DimensionTrait >( m_data )->symbol_id;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->symbols;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
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
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& get_Interface_LinkTrait_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Tree::Interface_OwnershipLinkTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->interface_id;
        case data::Tree::Interface_OwnershipLinkTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_LinkTrait >( m_data )->parent;
        case data::Tree::Interface_OwnershipLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_LinkTrait >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Model::HyperGraph_Relation >& get_Interface_LinkTrait_relation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Interface_LinkTrait >( m_data )->relation;
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Interface_LinkTrait >( m_data )->relation;
        case data::Tree::Interface_OwnershipLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Interface_LinkTrait >( m_data )->relation;
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
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->symbol_id;
        case data::Tree::Interface_OwnershipLinkTrait::Object_Part_Type_ID:
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
std::optional< std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > > >& get_Interface_State_dimension_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->dimension_traits;
        case data::Tree::Interface_Namespace::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Namespace >( m_data )->dimension_traits;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->dimension_traits;
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
std::optional< std::optional< data::Ptr< data::Tree::Interface_InheritanceTrait > > >& get_Interface_State_inheritance_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->inheritance_trait;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->inheritance_trait;
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
bool& get_Interface_State_is_or_state(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::MetaAnalysis::Interface_State >( m_data )->is_or_state;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::MetaAnalysis::Interface_State >( m_data )->is_or_state;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::MetaAnalysis::Interface_State >( m_data )->is_or_state;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< data::Ptr< data::Tree::Interface_LinkTrait > > >& get_Interface_State_link_traits(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->link_traits;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->link_traits;
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
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::optional< data::Ptr< data::Tree::Interface_SizeTrait > > >& get_Interface_State_size_trait(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_State >( m_data )->size_trait;
        case data::Tree::Interface_Abstract::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Abstract >( m_data )->size_trait;
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Interupt >( m_data )->transition_trait;
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
data::Ptr< data::AST::Parser_Dimension >& get_Interface_UserDimensionTrait_parser_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_UserDimensionTrait >( m_data )->parser_dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Link >& get_Interface_UserLinkTrait_parser_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_UserLinkTrait >( m_data )->parser_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& get_Interface_UserLinkTrait_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_UserLinkTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Variable >& get_Invocations_Instructions_Dereference_instance(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_Dereference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_Dereference >( m_data )->instance;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Dimensions_Link >& get_Invocations_Instructions_Dereference_link_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_Dereference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_Dereference >( m_data )->link_dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_LinkType >& get_Invocations_Instructions_Dereference_link_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_Dereference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_Dereference >( m_data )->link_type;
        case data::Operations::Invocations_Instructions_LinkBranch::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_LinkBranch >( m_data )->link_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Memory >& get_Invocations_Instructions_Dereference_ref(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_Dereference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_Dereference >( m_data )->ref;
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
        case data::Operations::Invocations_Instructions_Dereference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_LinkBranch::Object_Part_Type_ID:
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
data::Ptr< data::Operations::Invocations_Variables_Variable >& get_Invocations_Instructions_PolyCase_reference(data::Variant& m_data)
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
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& get_Invocations_Instructions_PolyCase_type(data::Variant& m_data)
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
        case data::Operations::Invocations_Instructions_PolyBranch::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_PolyBranch >( m_data )->parameter;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& get_Invocations_Operations_Operation_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Operations_Operation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Operations_Operation >( m_data )->context;
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
data::Ptr< data::Concrete::Concrete_Dimensions_LinkType >& get_Invocations_Variables_LinkType_link_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Variables_LinkType::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_LinkType >( m_data )->link_type;
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
        case data::Operations::Invocations_Variables_LinkType::Object_Part_Type_ID:
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
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
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
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
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
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
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
std::optional< std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Bitset > > >& get_MemoryLayout_Part_bitset_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->bitset_dimensions;
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
std::optional< std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > > >& get_MemoryLayout_Part_link_dimensions(data::Variant& m_data)
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
std::optional< std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > > >& get_MemoryLayout_Part_user_dimensions(data::Variant& m_data)
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
std::optional< std::string >& get_Operations_Invocation_canonical_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::string >& get_Operations_Invocation_canonical_operation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::string >& get_Operations_Invocation_canonical_type_path(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::ExplicitOperationID >& get_Operations_Invocation_explicit_operation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< mega::SourceLocation > >& get_Operations_Invocation_file_offsets(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Operations::Operations_ReturnTypes_ReturnType > >& get_Operations_Invocation_return_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
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
std::map< mega::OperatorID, data::Ptr< data::Operations::Operations_Operator > >& get_Operations_Invocations_operators(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocations::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocations >( m_data )->operators;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::OperatorID& get_Operations_Operator_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Operator::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        case data::Operations::Operations_New::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        case data::Operations::Operations_Delete::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        case data::Operations::Operations_Cast::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        case data::Operations::Operations_Active::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        case data::Operations::Operations_Enabled::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& get_Operations_ReturnTypes_Context_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_Context::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Context >( m_data )->contexts;
        case data::Operations::Operations_ReturnTypes_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Context >( m_data )->contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > >& get_Operations_ReturnTypes_Dimension_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_Dimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Dimension >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& get_Operations_ReturnTypes_Dimension_homogeneous(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_Dimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Dimension >( m_data )->homogeneous;
        case data::Operations::Operations_ReturnTypes_Function::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Function >( m_data )->homogeneous;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_Function > >& get_Operations_ReturnTypes_Function_functions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_Function::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Function >( m_data )->functions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::string >& get_Operations_ReturnTypes_ReturnType_canonical_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_ReturnType::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        case data::Operations::Operations_ReturnTypes_Void::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        case data::Operations::Operations_ReturnTypes_Dimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        case data::Operations::Operations_ReturnTypes_Function::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        case data::Operations::Operations_ReturnTypes_Context::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        case data::Operations::Operations_ReturnTypes_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Operations_ReturnTypes_ReturnType >& get_Operations_Write_parameter_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Write >( m_data )->parameter_type;
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_InteruptDef >( m_data )->argumentList;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::CardinalityRange& get_Parser_Link_cardinality(data::Variant& m_data)
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
data::Ptr< data::AST::Parser_Identifier >& get_Parser_Link_id(data::Variant& m_data)
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
bool& get_Parser_Link_owning(data::Variant& m_data)
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
data::Ptr< data::AST::Parser_TypeList >& get_Parser_Link_type(data::Variant& m_data)
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_InteruptDef >( m_data )->transition;
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
mega::U32& set_Automata_Enum_bitset_index(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->bitset_index;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AutomataAnalysis::Automata_Enum > >& set_Automata_Enum_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Automata_Enum_has_action(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->has_action;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< mega::U32 >& set_Automata_Enum_indices(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->indices;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Automata_Enum_is_or(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->is_or;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::SubTypeInstance& set_Automata_Enum_sub_type_instance(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->sub_type_instance;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U32& set_Automata_Enum_switch_index(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Enum::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Enum >( m_data )->switch_index;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::AutomataAnalysis::Automata_Vertex > >& set_Automata_Vertex_children(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Vertex::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->children;
        case data::AutomataAnalysis::Automata_And::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->children;
        case data::AutomataAnalysis::Automata_Or::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->children;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& set_Automata_Vertex_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Vertex::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->context;
        case data::AutomataAnalysis::Automata_And::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->context;
        case data::AutomataAnalysis::Automata_Or::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U32& set_Automata_Vertex_index_base(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Vertex::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->index_base;
        case data::AutomataAnalysis::Automata_And::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->index_base;
        case data::AutomataAnalysis::Automata_Or::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->index_base;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U32& set_Automata_Vertex_relative_domain(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::AutomataAnalysis::Automata_Vertex::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->relative_domain;
        case data::AutomataAnalysis::Automata_And::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->relative_domain;
        case data::AutomataAnalysis::Automata_Or::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Automata_Vertex >( m_data )->relative_domain;
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
data::Ptr< data::Components::Components_Component >& set_Concrete_Context_component(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Context >( m_data )->component;
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
mega::Instance& set_Concrete_Context_local_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->local_size;
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
mega::Instance& set_Concrete_Context_total_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Context::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Interupt::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Function::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Context >( m_data )->total_size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::TypeID& set_Concrete_Dimensions_Bitset_concrete_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Bitset >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Bitset >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Bitset >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Bitset >( m_data )->concrete_id;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::PerSourceConcreteTable::Concrete_Dimensions_Bitset >( m_data )->concrete_id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_CompilerDimensionTrait >& set_Concrete_Dimensions_Bitset_interface_compiler_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->interface_compiler_dimension;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->interface_compiler_dimension;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->interface_compiler_dimension;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->interface_compiler_dimension;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->interface_compiler_dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Concrete_Dimensions_Bitset_offset(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->offset;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->offset;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Object >& set_Concrete_Dimensions_Bitset_parent_object(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->parent_object;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->parent_object;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->parent_object;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->parent_object;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Bitset >( m_data )->parent_object;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::MemoryLayout::MemoryLayout_Part >& set_Concrete_Dimensions_Bitset_part(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->part;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->part;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Concrete_Dimensions_Bitset_size(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Bitset::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->size;
        case data::Concrete::Concrete_Dimensions_Configuration::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->size;
        case data::Concrete::Concrete_Dimensions_Activation::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->size;
        case data::Concrete::Concrete_Dimensions_Enablement::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->size;
        case data::Concrete::Concrete_Dimensions_History::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_Bitset >( m_data )->size;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Dimensions_Link >& set_Concrete_Dimensions_LinkType_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_LinkType::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_LinkType >( m_data )->link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::U64& set_Concrete_Dimensions_LinkType_offset(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_LinkType::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_LinkType >( m_data )->offset;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Context >& set_Concrete_Dimensions_LinkType_parent_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_LinkType::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_LinkType >( m_data )->parent_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::MemoryLayout::MemoryLayout_Part >& set_Concrete_Dimensions_LinkType_part(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_LinkType::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::Concrete_Dimensions_LinkType >( m_data )->part;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_LinkTrait >& set_Concrete_Dimensions_Link_interface_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->interface_link;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->interface_link;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->interface_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_LinkType > >& set_Concrete_Dimensions_Link_link_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->link_type;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->link_type;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_Link >( m_data )->link_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Concrete_Dimensions_Link_owned(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owned;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owned;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owned;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Concrete_Dimensions_Link_owning(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owning;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owning;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->owning;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Model::HyperGraph_Relation >& set_Concrete_Dimensions_Link_relation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->relation;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->relation;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->relation;
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
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->singular;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->singular;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->singular;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Concrete_Dimensions_Link_source(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_Link::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->source;
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->source;
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Concrete_Dimensions_Link >( m_data )->source;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_OwnershipLinkTrait >& set_Concrete_Dimensions_OwnershipLink_interface_owner_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_OwnershipLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_OwnershipLink >( m_data )->interface_owner_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_UserLinkTrait >& set_Concrete_Dimensions_UserLink_interface_user_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Dimensions_UserLink::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Dimensions_UserLink >( m_data )->interface_user_link;
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
        case data::Concrete::Concrete_Requirement::Object_Part_Type_ID:
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
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_Activation > >& set_Concrete_Object_activation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->activation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AutomataAnalysis::Automata_Enum >& set_Concrete_Object_automata_enum(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->automata_enum;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->automata_enum;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AutomataAnalysis::Automata_Vertex >& set_Concrete_Object_automata_root(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->automata_root;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->automata_root;
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
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_Configuration > >& set_Concrete_Object_configuration(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->configuration;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_Enablement > >& set_Concrete_Object_enablement(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->enablement;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Concrete::Concrete_Dimensions_History > >& set_Concrete_Object_history(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_Object >( m_data )->history;
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
mega::U64& set_Concrete_Object_total_index(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->total_index;
        case data::GlobalMemoryRollout::Concrete_MemoryMappedObject::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_Object >( m_data )->total_index;
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
data::Ptr< data::AutomataAnalysis::Automata_Vertex >& set_Concrete_State_automata_vertex(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_State >( m_data )->automata_vertex;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_State >( m_data )->automata_vertex;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::AutomataAnalysis::Concrete_State >( m_data )->automata_vertex;
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
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Bitset > >& set_Concrete_UserDimensionContext_bitsets(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Concrete::Concrete_UserDimensionContext::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Namespace::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_State::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Action::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Component::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Event::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
        case data::Concrete::Concrete_Object::Object_Part_Type_ID:
            return data::convert< data::Concrete::Concrete_UserDimensionContext >( m_data )->bitsets;
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
bool& set_Derivation_Edge_backtracked(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Edge::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Edge >( m_data )->backtracked;
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
int& set_Derivation_Edge_precedence(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Derivation_Edge::Object_Part_Type_ID:
            return data::convert< data::Operations::Derivation_Edge >( m_data )->precedence;
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
std::map< data::Ptr< data::Tree::Interface_UserLinkTrait >, data::Ptr< data::Model::HyperGraph_NonOwningObjectRelation > >& set_HyperGraph_Graph_non_owning_relations(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_Graph::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Graph >( m_data )->non_owning_relations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Model::HyperGraph_OwningObjectRelation >& set_HyperGraph_Graph_owning_relation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Model::HyperGraph_Graph::Object_Part_Type_ID:
            return data::convert< data::Model::HyperGraph_Graph >( m_data )->owning_relation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Tree::Interface_UserLinkTrait >& set_HyperGraph_NonOwningObjectRelation_source(data::Variant& m_data)
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
data::Ptr< data::Tree::Interface_UserLinkTrait >& set_HyperGraph_NonOwningObjectRelation_target(data::Variant& m_data)
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
std::multimap< data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink >, data::Ptr< data::Tree::Interface_UserLinkTrait > >& set_HyperGraph_OwningObjectRelation_owned(data::Variant& m_data)
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
std::multimap< data::Ptr< data::Tree::Interface_UserLinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_OwnershipLink > >& set_HyperGraph_OwningObjectRelation_owners(data::Variant& m_data)
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
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& set_Inheritance_Mapping_inheritance_links(data::Variant& m_data)
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
std::multimap< data::Ptr< data::Tree::Interface_LinkTrait >, data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& set_Inheritance_ObjectMapping_inheritance_links(data::Variant& m_data)
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
std::string& set_Interface_CompilerDimensionTrait_identifier(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_CompilerDimensionTrait >( m_data )->identifier;
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->alignment;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->canonical_type;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_DimensionTrait >( m_data )->concrete;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->erased_type;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_DimensionTrait >( m_data )->interface_id;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->mangle;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_DimensionTrait >( m_data )->parent;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->simple;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->size;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_DimensionTrait >( m_data )->symbol_id;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_DimensionTrait >( m_data )->symbols;
        case data::Tree::Interface_CompilerDimensionTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_IContext >( m_data )->concrete;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->interface_id;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_IContext >( m_data )->symbol_id;
        case data::Tree::Interface_Object::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->invocation_instances;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::Locations::Interface_InvocationContext >( m_data )->source_location;
        case data::Tree::Interface_Function::Object_Part_Type_ID:
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
std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > >& set_Interface_LinkTrait_concrete(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceDerivations::Interface_LinkTrait >( m_data )->concrete;
        case data::Tree::Interface_OwnershipLinkTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->interface_id;
        case data::Tree::Interface_OwnershipLinkTrait::Object_Part_Type_ID:
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
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_LinkTrait >( m_data )->parent;
        case data::Tree::Interface_OwnershipLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_LinkTrait >( m_data )->parent;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Model::HyperGraph_Relation >& set_Interface_LinkTrait_relation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_LinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Interface_LinkTrait >( m_data )->relation;
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Interface_LinkTrait >( m_data )->relation;
        case data::Tree::Interface_OwnershipLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceModel::Interface_LinkTrait >( m_data )->relation;
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
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::PerSourceSymbols::Interface_LinkTrait >( m_data )->symbol_id;
        case data::Tree::Interface_OwnershipLinkTrait::Object_Part_Type_ID:
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
bool& set_Interface_State_is_or_state(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_State::Object_Part_Type_ID:
            return data::convert< data::MetaAnalysis::Interface_State >( m_data )->is_or_state;
        case data::Tree::Interface_Action::Object_Part_Type_ID:
            return data::convert< data::MetaAnalysis::Interface_State >( m_data )->is_or_state;
        case data::Tree::Interface_Component::Object_Part_Type_ID:
            return data::convert< data::MetaAnalysis::Interface_State >( m_data )->is_or_state;
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
        case data::Tree::Interface_Requirement::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_Interupt >( m_data )->transition_trait;
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
data::Ptr< data::AST::Parser_Dimension >& set_Interface_UserDimensionTrait_parser_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_UserDimensionTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_UserDimensionTrait >( m_data )->parser_dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::AST::Parser_Link >& set_Interface_UserLinkTrait_parser_link(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Tree::Interface_UserLinkTrait >( m_data )->parser_link;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Clang::Interface_TypePathVariant > >& set_Interface_UserLinkTrait_tuple(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Tree::Interface_UserLinkTrait::Object_Part_Type_ID:
            return data::convert< data::Clang::Interface_UserLinkTrait >( m_data )->tuple;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Variable >& set_Invocations_Instructions_Dereference_instance(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_Dereference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_Dereference >( m_data )->instance;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Dimensions_Link >& set_Invocations_Instructions_Dereference_link_dimension(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_Dereference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_Dereference >( m_data )->link_dimension;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_LinkType >& set_Invocations_Instructions_Dereference_link_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_Dereference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_Dereference >( m_data )->link_type;
        case data::Operations::Invocations_Instructions_LinkBranch::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_LinkBranch >( m_data )->link_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Invocations_Variables_Memory >& set_Invocations_Instructions_Dereference_ref(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Instructions_Dereference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_Dereference >( m_data )->ref;
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
        case data::Operations::Invocations_Instructions_Dereference::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_InstructionGroup >( m_data )->children;
        case data::Operations::Invocations_Instructions_LinkBranch::Object_Part_Type_ID:
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
data::Ptr< data::Operations::Invocations_Variables_Variable >& set_Invocations_Instructions_PolyCase_reference(data::Variant& m_data)
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
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& set_Invocations_Instructions_PolyCase_type(data::Variant& m_data)
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
        case data::Operations::Invocations_Instructions_PolyBranch::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Instructions_PolyBranch >( m_data )->parameter;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Concrete::Concrete_Graph_Vertex >& set_Invocations_Operations_Operation_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Operations_Operation::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Operations_Operation >( m_data )->context;
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
data::Ptr< data::Concrete::Concrete_Dimensions_LinkType >& set_Invocations_Variables_LinkType_link_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Invocations_Variables_LinkType::Object_Part_Type_ID:
            return data::convert< data::Operations::Invocations_Variables_LinkType >( m_data )->link_type;
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
        case data::Operations::Invocations_Variables_LinkType::Object_Part_Type_ID:
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
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
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
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
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
        case data::MemoryLayout::MemoryLayout_SimpleBuffer::Object_Part_Type_ID:
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
std::optional< std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Bitset > > >& set_MemoryLayout_Part_bitset_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::MemoryLayout::MemoryLayout_Part::Object_Part_Type_ID:
            return data::convert< data::MemoryLayout::MemoryLayout_Part >( m_data )->bitset_dimensions;
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
std::optional< std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_Link > > >& set_MemoryLayout_Part_link_dimensions(data::Variant& m_data)
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
std::optional< std::vector< data::Ptr< data::Concrete::Concrete_Dimensions_User > > >& set_MemoryLayout_Part_user_dimensions(data::Variant& m_data)
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
std::optional< std::string >& set_Operations_Invocation_canonical_context(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_context;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::string >& set_Operations_Invocation_canonical_operation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_operation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::string >& set_Operations_Invocation_canonical_type_path(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->canonical_type_path;
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->derivation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< mega::ExplicitOperationID >& set_Operations_Invocation_explicit_operation(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->explicit_operation;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::vector< mega::SourceLocation > >& set_Operations_Invocation_file_offsets(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->file_offsets;
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->id;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->operations;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< data::Ptr< data::Operations::Operations_ReturnTypes_ReturnType > >& set_Operations_Invocation_return_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocation::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
        case data::Operations::Operations_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->return_type;
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->root_instruction;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
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
        case data::Operations::Operations_Start::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Call::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Signal::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Move::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_GetContext::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Read::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkRead::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkAdd::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkRemove::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocation >( m_data )->variables;
        case data::Operations::Operations_LinkClear::Object_Part_Type_ID:
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
std::map< mega::OperatorID, data::Ptr< data::Operations::Operations_Operator > >& set_Operations_Invocations_operators(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Invocations::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Invocations >( m_data )->operators;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::OperatorID& set_Operations_Operator_id(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Operator::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        case data::Operations::Operations_New::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        case data::Operations::Operations_Delete::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        case data::Operations::Operations_Cast::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        case data::Operations::Operations_Active::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        case data::Operations::Operations_Enabled::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Operator >( m_data )->id;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_IContext > >& set_Operations_ReturnTypes_Context_contexts(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_Context::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Context >( m_data )->contexts;
        case data::Operations::Operations_ReturnTypes_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Context >( m_data )->contexts;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_DimensionTrait > >& set_Operations_ReturnTypes_Dimension_dimensions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_Dimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Dimension >( m_data )->dimensions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
bool& set_Operations_ReturnTypes_Dimension_homogeneous(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_Dimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Dimension >( m_data )->homogeneous;
        case data::Operations::Operations_ReturnTypes_Function::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Function >( m_data )->homogeneous;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::vector< data::Ptr< data::Tree::Interface_Function > >& set_Operations_ReturnTypes_Function_functions(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_Function::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_Function >( m_data )->functions;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
std::optional< std::string >& set_Operations_ReturnTypes_ReturnType_canonical_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_ReturnTypes_ReturnType::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        case data::Operations::Operations_ReturnTypes_Void::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        case data::Operations::Operations_ReturnTypes_Dimension::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        case data::Operations::Operations_ReturnTypes_Function::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        case data::Operations::Operations_ReturnTypes_Context::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        case data::Operations::Operations_ReturnTypes_Range::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_ReturnTypes_ReturnType >( m_data )->canonical_type;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
data::Ptr< data::Operations::Operations_ReturnTypes_ReturnType >& set_Operations_Write_parameter_type(data::Variant& m_data)
{
    switch( m_data.getType() )
    {
        case data::Operations::Operations_Write::Object_Part_Type_ID:
            return data::convert< data::Operations::Operations_Write >( m_data )->parameter_type;
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_InteruptDef >( m_data )->argumentList;
        default:
        {
            THROW_RTE( "Database used with incorrect type" );
        }
    }
}
mega::CardinalityRange& set_Parser_Link_cardinality(data::Variant& m_data)
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
        case data::AST::Parser_RequirementDef::Object_Part_Type_ID:
            return data::convert< data::AST::Parser_InteruptDef >( m_data )->transition;
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
        case 13: return new AST::Parser_Part( loader, objectInfo );
        case 14: return new AST::Parser_Include( loader, objectInfo );
        case 15: return new AST::Parser_SystemInclude( loader, objectInfo );
        case 16: return new AST::Parser_MegaInclude( loader, objectInfo );
        case 17: return new AST::Parser_MegaIncludeInline( loader, objectInfo );
        case 18: return new AST::Parser_MegaIncludeNested( loader, objectInfo );
        case 19: return new AST::Parser_CPPInclude( loader, objectInfo );
        case 20: return new AST::Parser_Dependency( loader, objectInfo );
        case 21: return new AST::Parser_ContextDef( loader, objectInfo );
        case 23: return new AST::Parser_NamespaceDef( loader, objectInfo );
        case 24: return new AST::Parser_AbstractDef( loader, objectInfo );
        case 25: return new AST::Parser_StateDef( loader, objectInfo );
        case 26: return new AST::Parser_ActionDef( loader, objectInfo );
        case 27: return new AST::Parser_ComponentDef( loader, objectInfo );
        case 28: return new AST::Parser_EventDef( loader, objectInfo );
        case 29: return new AST::Parser_InteruptDef( loader, objectInfo );
        case 30: return new AST::Parser_RequirementDef( loader, objectInfo );
        case 31: return new AST::Parser_FunctionDef( loader, objectInfo );
        case 32: return new AST::Parser_ObjectDef( loader, objectInfo );
        case 33: return new AST::Parser_SourceRoot( loader, objectInfo );
        case 34: return new AST::Parser_IncludeRoot( loader, objectInfo );
        case 35: return new AST::Parser_ObjectSourceRoot( loader, objectInfo );
        case 22: return new Body::Parser_ContextDef( loader, objectInfo );
        case 38: return new Tree::Interface_DimensionTrait( loader, objectInfo );
        case 42: return new Tree::Interface_UserDimensionTrait( loader, objectInfo );
        case 43: return new Tree::Interface_CompilerDimensionTrait( loader, objectInfo );
        case 44: return new Tree::Interface_LinkTrait( loader, objectInfo );
        case 48: return new Tree::Interface_UserLinkTrait( loader, objectInfo );
        case 50: return new Tree::Interface_OwnershipLinkTrait( loader, objectInfo );
        case 51: return new Tree::Interface_InheritanceTrait( loader, objectInfo );
        case 53: return new Tree::Interface_ReturnTypeTrait( loader, objectInfo );
        case 55: return new Tree::Interface_ArgumentListTrait( loader, objectInfo );
        case 57: return new Tree::Interface_PartTrait( loader, objectInfo );
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
        case 76: return new Tree::Interface_Action( loader, objectInfo );
        case 77: return new Tree::Interface_Component( loader, objectInfo );
        case 78: return new Tree::Interface_Event( loader, objectInfo );
        case 79: return new Tree::Interface_Interupt( loader, objectInfo );
        case 80: return new Tree::Interface_Requirement( loader, objectInfo );
        case 81: return new Tree::Interface_Function( loader, objectInfo );
        case 82: return new Tree::Interface_Object( loader, objectInfo );
        case 176: return new DPGraph::Dependencies_Glob( loader, objectInfo );
        case 177: return new DPGraph::Dependencies_SourceFileDependencies( loader, objectInfo );
        case 178: return new DPGraph::Dependencies_TransitiveDependencies( loader, objectInfo );
        case 179: return new DPGraph::Dependencies_Analysis( loader, objectInfo );
        case 180: return new SymbolTable::Symbols_SymbolTypeID( loader, objectInfo );
        case 181: return new SymbolTable::Symbols_InterfaceTypeID( loader, objectInfo );
        case 183: return new SymbolTable::Symbols_SymbolTable( loader, objectInfo );
        case 39: return new PerSourceSymbols::Interface_DimensionTrait( loader, objectInfo );
        case 45: return new PerSourceSymbols::Interface_LinkTrait( loader, objectInfo );
        case 67: return new PerSourceSymbols::Interface_IContext( loader, objectInfo );
        case 75: return new MetaAnalysis::Interface_State( loader, objectInfo );
        case 36: return new Clang::Interface_TypePath( loader, objectInfo );
        case 37: return new Clang::Interface_TypePathVariant( loader, objectInfo );
        case 41: return new Clang::Interface_DimensionTrait( loader, objectInfo );
        case 49: return new Clang::Interface_UserLinkTrait( loader, objectInfo );
        case 52: return new Clang::Interface_InheritanceTrait( loader, objectInfo );
        case 54: return new Clang::Interface_ReturnTypeTrait( loader, objectInfo );
        case 56: return new Clang::Interface_ArgumentListTrait( loader, objectInfo );
        case 59: return new Clang::Interface_TransitionTypeTrait( loader, objectInfo );
        case 61: return new Clang::Interface_EventTypeTrait( loader, objectInfo );
        case 63: return new Clang::Interface_SizeTrait( loader, objectInfo );
        case 99: return new Concrete::Concrete_Graph_Vertex( loader, objectInfo );
        case 102: return new Concrete::Concrete_Dimensions_User( loader, objectInfo );
        case 105: return new Concrete::Concrete_Dimensions_Link( loader, objectInfo );
        case 109: return new Concrete::Concrete_Dimensions_UserLink( loader, objectInfo );
        case 110: return new Concrete::Concrete_Dimensions_OwnershipLink( loader, objectInfo );
        case 111: return new Concrete::Concrete_Dimensions_LinkType( loader, objectInfo );
        case 113: return new Concrete::Concrete_Dimensions_Bitset( loader, objectInfo );
        case 116: return new Concrete::Concrete_Dimensions_Configuration( loader, objectInfo );
        case 117: return new Concrete::Concrete_Dimensions_Activation( loader, objectInfo );
        case 118: return new Concrete::Concrete_Dimensions_Enablement( loader, objectInfo );
        case 119: return new Concrete::Concrete_Dimensions_History( loader, objectInfo );
        case 120: return new Concrete::Concrete_ContextGroup( loader, objectInfo );
        case 121: return new Concrete::Concrete_Context( loader, objectInfo );
        case 124: return new Concrete::Concrete_Interupt( loader, objectInfo );
        case 125: return new Concrete::Concrete_Requirement( loader, objectInfo );
        case 126: return new Concrete::Concrete_Function( loader, objectInfo );
        case 127: return new Concrete::Concrete_UserDimensionContext( loader, objectInfo );
        case 128: return new Concrete::Concrete_Namespace( loader, objectInfo );
        case 129: return new Concrete::Concrete_State( loader, objectInfo );
        case 131: return new Concrete::Concrete_Action( loader, objectInfo );
        case 132: return new Concrete::Concrete_Component( loader, objectInfo );
        case 133: return new Concrete::Concrete_Event( loader, objectInfo );
        case 134: return new Concrete::Concrete_Object( loader, objectInfo );
        case 139: return new Concrete::Concrete_Root( loader, objectInfo );
        case 185: return new Derivations::Inheritance_ObjectMapping( loader, objectInfo );
        case 186: return new Derivations::Inheritance_Mapping( loader, objectInfo );
        case 40: return new PerSourceDerivations::Interface_DimensionTrait( loader, objectInfo );
        case 46: return new PerSourceDerivations::Interface_LinkTrait( loader, objectInfo );
        case 68: return new PerSourceDerivations::Interface_IContext( loader, objectInfo );
        case 101: return new Model::Concrete_Graph_Edge( loader, objectInfo );
        case 187: return new Model::HyperGraph_Relation( loader, objectInfo );
        case 188: return new Model::HyperGraph_OwningObjectRelation( loader, objectInfo );
        case 189: return new Model::HyperGraph_NonOwningObjectRelation( loader, objectInfo );
        case 190: return new Model::HyperGraph_Graph( loader, objectInfo );
        case 47: return new PerSourceModel::Interface_LinkTrait( loader, objectInfo );
        case 100: return new PerSourceModel::Concrete_Graph_Vertex( loader, objectInfo );
        case 106: return new PerSourceModel::Concrete_Dimensions_Link( loader, objectInfo );
        case 136: return new PerSourceModel::Concrete_Object( loader, objectInfo );
        case 140: return new Model::Alias_Edge( loader, objectInfo );
        case 141: return new Model::Alias_Step( loader, objectInfo );
        case 142: return new Model::Alias_And( loader, objectInfo );
        case 143: return new Model::Alias_Or( loader, objectInfo );
        case 144: return new Model::Alias_AliasDerivation( loader, objectInfo );
        case 182: return new ConcreteTable::Symbols_ConcreteTypeID( loader, objectInfo );
        case 184: return new ConcreteTable::Symbols_SymbolTable( loader, objectInfo );
        case 104: return new PerSourceConcreteTable::Concrete_Dimensions_User( loader, objectInfo );
        case 108: return new PerSourceConcreteTable::Concrete_Dimensions_Link( loader, objectInfo );
        case 115: return new PerSourceConcreteTable::Concrete_Dimensions_Bitset( loader, objectInfo );
        case 123: return new PerSourceConcreteTable::Concrete_Context( loader, objectInfo );
        case 103: return new MemoryLayout::Concrete_Dimensions_User( loader, objectInfo );
        case 107: return new MemoryLayout::Concrete_Dimensions_Link( loader, objectInfo );
        case 112: return new MemoryLayout::Concrete_Dimensions_LinkType( loader, objectInfo );
        case 114: return new MemoryLayout::Concrete_Dimensions_Bitset( loader, objectInfo );
        case 122: return new MemoryLayout::Concrete_Context( loader, objectInfo );
        case 135: return new MemoryLayout::Concrete_Object( loader, objectInfo );
        case 191: return new MemoryLayout::MemoryLayout_Part( loader, objectInfo );
        case 192: return new MemoryLayout::MemoryLayout_Buffer( loader, objectInfo );
        case 193: return new MemoryLayout::MemoryLayout_SimpleBuffer( loader, objectInfo );
        case 194: return new GlobalMemoryLayout::MemoryLayout_MemoryMap( loader, objectInfo );
        case 138: return new GlobalMemoryRollout::Concrete_MemoryMappedObject( loader, objectInfo );
        case 130: return new AutomataAnalysis::Concrete_State( loader, objectInfo );
        case 137: return new AutomataAnalysis::Concrete_Object( loader, objectInfo );
        case 201: return new AutomataAnalysis::Automata_Vertex( loader, objectInfo );
        case 202: return new AutomataAnalysis::Automata_And( loader, objectInfo );
        case 203: return new AutomataAnalysis::Automata_Or( loader, objectInfo );
        case 204: return new AutomataAnalysis::Automata_Enum( loader, objectInfo );
        case 83: return new Operations::Invocations_Variables_Variable( loader, objectInfo );
        case 84: return new Operations::Invocations_Variables_Stack( loader, objectInfo );
        case 85: return new Operations::Invocations_Variables_LinkType( loader, objectInfo );
        case 86: return new Operations::Invocations_Variables_Reference( loader, objectInfo );
        case 87: return new Operations::Invocations_Variables_Memory( loader, objectInfo );
        case 88: return new Operations::Invocations_Variables_Parameter( loader, objectInfo );
        case 89: return new Operations::Invocations_Instructions_Instruction( loader, objectInfo );
        case 90: return new Operations::Invocations_Instructions_InstructionGroup( loader, objectInfo );
        case 91: return new Operations::Invocations_Instructions_Root( loader, objectInfo );
        case 92: return new Operations::Invocations_Instructions_ParentDerivation( loader, objectInfo );
        case 93: return new Operations::Invocations_Instructions_ChildDerivation( loader, objectInfo );
        case 94: return new Operations::Invocations_Instructions_Dereference( loader, objectInfo );
        case 95: return new Operations::Invocations_Instructions_LinkBranch( loader, objectInfo );
        case 96: return new Operations::Invocations_Instructions_PolyBranch( loader, objectInfo );
        case 97: return new Operations::Invocations_Instructions_PolyCase( loader, objectInfo );
        case 98: return new Operations::Invocations_Operations_Operation( loader, objectInfo );
        case 145: return new Operations::Derivation_Edge( loader, objectInfo );
        case 146: return new Operations::Derivation_Step( loader, objectInfo );
        case 147: return new Operations::Derivation_And( loader, objectInfo );
        case 148: return new Operations::Derivation_Or( loader, objectInfo );
        case 149: return new Operations::Derivation_Root( loader, objectInfo );
        case 150: return new Operations::Operations_ReturnTypes_ReturnType( loader, objectInfo );
        case 151: return new Operations::Operations_ReturnTypes_Void( loader, objectInfo );
        case 152: return new Operations::Operations_ReturnTypes_Dimension( loader, objectInfo );
        case 153: return new Operations::Operations_ReturnTypes_Function( loader, objectInfo );
        case 154: return new Operations::Operations_ReturnTypes_Context( loader, objectInfo );
        case 155: return new Operations::Operations_ReturnTypes_Range( loader, objectInfo );
        case 156: return new Operations::Operations_Operator( loader, objectInfo );
        case 157: return new Operations::Operations_New( loader, objectInfo );
        case 158: return new Operations::Operations_Delete( loader, objectInfo );
        case 159: return new Operations::Operations_Cast( loader, objectInfo );
        case 160: return new Operations::Operations_Active( loader, objectInfo );
        case 161: return new Operations::Operations_Enabled( loader, objectInfo );
        case 162: return new Operations::Operations_Invocation( loader, objectInfo );
        case 163: return new Operations::Operations_Start( loader, objectInfo );
        case 164: return new Operations::Operations_Call( loader, objectInfo );
        case 165: return new Operations::Operations_Signal( loader, objectInfo );
        case 166: return new Operations::Operations_Move( loader, objectInfo );
        case 167: return new Operations::Operations_GetContext( loader, objectInfo );
        case 168: return new Operations::Operations_Read( loader, objectInfo );
        case 169: return new Operations::Operations_Write( loader, objectInfo );
        case 170: return new Operations::Operations_LinkRead( loader, objectInfo );
        case 171: return new Operations::Operations_LinkAdd( loader, objectInfo );
        case 172: return new Operations::Operations_LinkRemove( loader, objectInfo );
        case 173: return new Operations::Operations_LinkClear( loader, objectInfo );
        case 174: return new Operations::Operations_Range( loader, objectInfo );
        case 175: return new Operations::Operations_Invocations( loader, objectInfo );
        case 69: return new Locations::Interface_InvocationInstance( loader, objectInfo );
        case 71: return new Locations::Interface_InvocationContext( loader, objectInfo );
        case 195: return new UnityAnalysis::UnityAnalysis_DataBinding( loader, objectInfo );
        case 196: return new UnityAnalysis::UnityAnalysis_LinkBinding( loader, objectInfo );
        case 197: return new UnityAnalysis::UnityAnalysis_ObjectBinding( loader, objectInfo );
        case 198: return new UnityAnalysis::UnityAnalysis_Prefab( loader, objectInfo );
        case 199: return new UnityAnalysis::UnityAnalysis_Manual( loader, objectInfo );
        case 200: return new UnityAnalysis::UnityAnalysis_Binding( loader, objectInfo );
        default:
            THROW_RTE( "Unrecognised object type ID" );
    }
}

}
