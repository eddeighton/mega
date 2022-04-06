
#include "database/model/view.hxx"
#include "database/model/data.hxx"

#include "database/io/object_info.hpp"
#include "database/io/file_info.hpp"
#include "database/io/file.hpp"
#include "database/io/manifest.hpp"

#include <vector>

namespace ComponentListing
{
struct Database::Pimpl
{
    using FileMap = std::map< mega::io::ObjectInfo::FileID, mega::io::File::Ptr >;
    using FileMapCst = std::map< mega::io::ObjectInfo::FileID, mega::io::File::PtrCst >;

    Pimpl( const mega::io::Environment& environment )
        :   m_environment( environment ),
            m_manifest( m_environment.project_manifest() )
    {
        // create read-write files
        {
            std::vector< mega::io::FileInfo > fileInfos;
            m_manifest.getCompilationFileInfos( mega::io::FileInfo::FILE_ComponentListing_Components, fileInfos );
            VERIFY_RTE( fileInfos.size() == 1U );
            const mega::io::FileInfo& fileInfo = fileInfos.front();
            m_pFile_Components = std::make_shared< mega::io::File >( fileInfo ) ;
            m_readwriteFiles.insert( std::make_pair( fileInfo.getFileID(), m_pFile_Components ) ); 
        }
    }

    Components::Component* construct_Components_Component()
    {
        using PrimaryObjectPart = data::Components::Component;

        // create the primary object part
        PrimaryObjectPart* pPrimaryData = m_pFile_Components->construct< PrimaryObjectPart >();

        // create secondary object parts

        // create the view type
        Components::Component* pObject = new Components::Component( pPrimaryData );
        pPrimaryData->pView = pObject;

        return pObject;
    }

    void store()
    {
        for ( FileMap::const_iterator 
                i = m_readwriteFiles.begin(), 
                iEnd = m_readwriteFiles.end();
              i != iEnd; ++i )
        {
            i->second->store( m_manifest );
        }
    }

    const mega::io::Environment& m_environment;
    const mega::io::Manifest m_manifest;

    std::shared_ptr< mega::io::File > m_pFile_Components;

    FileMap m_readonlyFiles;
    FileMap m_readwriteFiles;
};

Database::Database( const mega::io::Environment& environment )
:   m_pimpl( new Pimpl( environment ) )
{
}

void Database::store()
{
    m_pimpl->store();
}

Components::Component* Database::construct_Components_Component()
{
    return m_pimpl->construct_Components_Component();
}


namespace Components
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Component
Component::Component( void* pData )
:   m_pData( pData )
{
}

const std::string& Component::get_name() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->name;
}
void Component::set_name( const std::string& value ) const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    pData->name = value;
}
const boost::filesystem::path& Component::get_directory() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->directory;
}
void Component::set_directory( const boost::filesystem::path& value ) const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    pData->directory = value;
}
const std::vector< boost::filesystem::path >& Component::get_includeDirectories() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->includeDirectories;
}
void Component::set_includeDirectories( const std::vector< boost::filesystem::path >& value ) const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    pData->includeDirectories = value;
}
const std::vector< boost::filesystem::path >& Component::get_sourceFiles() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->sourceFiles;
}
void Component::set_sourceFiles( const std::vector< boost::filesystem::path >& value ) const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    pData->sourceFiles = value;
}

} // Components

}
namespace InputParse
{
struct Database::Pimpl
{
    using FileMap = std::map< mega::io::ObjectInfo::FileID, mega::io::File::Ptr >;
    using FileMapCst = std::map< mega::io::ObjectInfo::FileID, mega::io::File::PtrCst >;
    Pimpl( const mega::io::Environment& environment, const boost::filesystem::path& objectFile )
        :   m_environment( environment ),
            m_objectFile( objectFile ),
            m_manifest( m_environment.project_manifest() )
    {
        // create read-write files
        {
            std::vector< mega::io::FileInfo > fileInfos;
            m_manifest.getCompilationFileInfos( mega::io::FileInfo::FILE_InputParse_AST, m_objectFile, fileInfos );
            VERIFY_RTE( fileInfos.size() == 1U );
            const mega::io::FileInfo& fileInfo = fileInfos.front();
            m_pFile_AST = std::make_shared< mega::io::File >( fileInfo ) ;
            m_readwriteFiles.insert( std::make_pair( fileInfo.getFileID(), m_pFile_AST ) ); 
        }
        {
            std::vector< mega::io::FileInfo > fileInfos;
            m_manifest.getCompilationFileInfos( mega::io::FileInfo::FILE_InputParse_Body, m_objectFile, fileInfos );
            VERIFY_RTE( fileInfos.size() == 1U );
            const mega::io::FileInfo& fileInfo = fileInfos.front();
            m_pFile_Body = std::make_shared< mega::io::File >( fileInfo ) ;
            m_readwriteFiles.insert( std::make_pair( fileInfo.getFileID(), m_pFile_Body ) ); 
        }
    }


    Parser::Opaque* construct_Parser_Opaque()
    {
        using PrimaryObjectPart = data::AST::Opaque;

        // create the primary object part
        PrimaryObjectPart* pPrimaryData = m_pFile_AST->construct< PrimaryObjectPart >();

        // create secondary object parts

        // create the view type
        Parser::Opaque* pObject = new Parser::Opaque( pPrimaryData );
        pPrimaryData->pView = pObject;

        return pObject;
    }
    Parser::Dimension* construct_Parser_Dimension()
    {
        using PrimaryObjectPart = data::AST::Dimension;

        // create the primary object part
        PrimaryObjectPart* pPrimaryData = m_pFile_AST->construct< PrimaryObjectPart >();

        // create secondary object parts

        // create the view type
        Parser::Dimension* pObject = new Parser::Dimension( pPrimaryData );
        pPrimaryData->pView = pObject;

        return pObject;
    }
    Parser::Context* construct_Parser_Context()
    {
        using PrimaryObjectPart = data::AST::Context;

        // create the primary object part
        PrimaryObjectPart* pPrimaryData = m_pFile_AST->construct< PrimaryObjectPart >();

        // create secondary object parts
        {
            using SecondaryObjectPart = data::Body::Context;
            SecondaryObjectPart* pSecondaryData = m_pFile_Body->construct< SecondaryObjectPart >();
            pSecondaryData->pAST_Context = pPrimaryData;
            pPrimaryData->pBody_Context = pSecondaryData;
        }

        // create the view type
        Parser::Context* pObject = new Parser::Context( pPrimaryData );
        pPrimaryData->pView = pObject;

        return pObject;
    }
    Parser::Root* construct_Parser_Root()
    {
        using PrimaryObjectPart = data::AST::Root;

        // create the primary object part
        PrimaryObjectPart* pPrimaryData = m_pFile_AST->construct< PrimaryObjectPart >();

        // create secondary object parts

        // create the view type
        Parser::Root* pObject = new Parser::Root( pPrimaryData );
        pPrimaryData->pView = pObject;

        return pObject;
    }

    void store()
    {
        for ( FileMap::const_iterator 
                i = m_readwriteFiles.begin(), 
                iEnd = m_readwriteFiles.end();
              i != iEnd; ++i )
        {
            i->second->store( m_manifest );
        }
    }

    const mega::io::Environment& m_environment;
    boost::filesystem::path m_objectFile;
    const mega::io::Manifest m_manifest;

    std::shared_ptr< mega::io::File > m_pFile_AST;
    std::shared_ptr< mega::io::File > m_pFile_Body;

    FileMap m_readonlyFiles;
    FileMap m_readwriteFiles;
};

Database::Database( const mega::io::Environment& environment, const boost::filesystem::path& objectFile )
:   m_pimpl( new Pimpl( environment, objectFile ) )
{
}

void Database::store()
{
    m_pimpl->store();
}

Parser::Opaque* Database::construct_Parser_Opaque()
{
    return m_pimpl->construct_Parser_Opaque();
}
Parser::Dimension* Database::construct_Parser_Dimension()
{
    return m_pimpl->construct_Parser_Dimension();
}
Parser::Context* Database::construct_Parser_Context()
{
    return m_pimpl->construct_Parser_Context();
}
Parser::Root* Database::construct_Parser_Root()
{
    return m_pimpl->construct_Parser_Root();
}

namespace Components
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Component
Component::Component( void* pData )
:   m_pData( pData )
{
}
const std::string& Component::get_name() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->name;
}
const boost::filesystem::path& Component::get_directory() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->directory;
}
const std::vector< boost::filesystem::path >& Component::get_includeDirectories() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->includeDirectories;
}
const std::vector< boost::filesystem::path >& Component::get_sourceFiles() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->sourceFiles;
}

} // Components

namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Opaque
Opaque::Opaque( void* pData )
:   m_pData( pData )
{
}

const std::string& Opaque::get_str() const
{
    using PrimaryObjectPart = data::AST::Opaque;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->str;
}
void Opaque::set_str( const std::string& value ) const
{
    using PrimaryObjectPart = data::AST::Opaque;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    pData->str = value;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Dimension
Dimension::Dimension( void* pData )
:   m_pData( pData )
{
}

const bool& Dimension::get_isConst() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->isConst;
}
void Dimension::set_isConst( const bool& value ) const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    pData->isConst = value;
}
const std::string& Dimension::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
void Dimension::set_identifier( const std::string& value ) const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    pData->identifier = value;
}
const std::string& Dimension::get_type() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->type;
}
void Dimension::set_type( const std::string& value ) const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    pData->type = value;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Context
Context::Context( void* pData )
:   m_pData( pData )
{
}

const std::string& Context::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
void Context::set_identifier( const std::string& value ) const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    pData->identifier = value;
}
const std::string& Context::get_body() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->pBody_Context->body;
}
void Context::set_body( const std::string& value ) const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    pData->pBody_Context->body = value;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}


} // Parser

}
namespace Interface
{
struct Database::Pimpl
{
    using FileMap = std::map< mega::io::ObjectInfo::FileID, mega::io::File::Ptr >;
    using FileMapCst = std::map< mega::io::ObjectInfo::FileID, mega::io::File::PtrCst >;
    Pimpl( const mega::io::Environment& environment, const boost::filesystem::path& objectFile )
        :   m_environment( environment ),
            m_objectFile( objectFile ),
            m_manifest( m_environment.project_manifest() )
    {
        // create read-write files
        {
            std::vector< mega::io::FileInfo > fileInfos;
            m_manifest.getCompilationFileInfos( mega::io::FileInfo::FILE_Interface_Tree, m_objectFile, fileInfos );
            VERIFY_RTE( fileInfos.size() == 1U );
            const mega::io::FileInfo& fileInfo = fileInfos.front();
            m_pFile_Tree = std::make_shared< mega::io::File >( fileInfo ) ;
            m_readwriteFiles.insert( std::make_pair( fileInfo.getFileID(), m_pFile_Tree ) ); 
        }
    }


    Interface::Root* construct_Interface_Root()
    {
        using PrimaryObjectPart = data::Tree::Root;

        // create the primary object part
        PrimaryObjectPart* pPrimaryData = m_pFile_Tree->construct< PrimaryObjectPart >();

        // create secondary object parts

        // create the view type
        Interface::Root* pObject = new Interface::Root( pPrimaryData );
        pPrimaryData->pView = pObject;

        return pObject;
    }

    void store()
    {
        for ( FileMap::const_iterator 
                i = m_readwriteFiles.begin(), 
                iEnd = m_readwriteFiles.end();
              i != iEnd; ++i )
        {
            i->second->store( m_manifest );
        }
    }

    const mega::io::Environment& m_environment;
    boost::filesystem::path m_objectFile;
    const mega::io::Manifest m_manifest;

    std::shared_ptr< mega::io::File > m_pFile_Tree;

    FileMap m_readonlyFiles;
    FileMap m_readwriteFiles;
};

Database::Database( const mega::io::Environment& environment, const boost::filesystem::path& objectFile )
:   m_pimpl( new Pimpl( environment, objectFile ) )
{
}

void Database::store()
{
    m_pimpl->store();
}

Interface::Root* Database::construct_Interface_Root()
{
    return m_pimpl->construct_Interface_Root();
}

namespace Components
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Component
Component::Component( void* pData )
:   m_pData( pData )
{
}
const std::string& Component::get_name() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->name;
}
const boost::filesystem::path& Component::get_directory() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->directory;
}
const std::vector< boost::filesystem::path >& Component::get_includeDirectories() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->includeDirectories;
}
const std::vector< boost::filesystem::path >& Component::get_sourceFiles() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->sourceFiles;
}

} // Components
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Opaque
Opaque::Opaque( void* pData )
:   m_pData( pData )
{
}
const std::string& Opaque::get_str() const
{
    using PrimaryObjectPart = data::AST::Opaque;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->str;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Dimension
Dimension::Dimension( void* pData )
:   m_pData( pData )
{
}
const bool& Dimension::get_isConst() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->isConst;
}
const std::string& Dimension::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Dimension::get_type() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->type;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Context
Context::Context( void* pData )
:   m_pData( pData )
{
}
const std::string& Context::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Context::get_body() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->pBody_Context->body;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Parser

namespace Interface
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}


} // Interface

}
namespace Dependencies
{
struct Database::Pimpl
{
    using FileMap = std::map< mega::io::ObjectInfo::FileID, mega::io::File::Ptr >;
    using FileMapCst = std::map< mega::io::ObjectInfo::FileID, mega::io::File::PtrCst >;

    Pimpl( const mega::io::Environment& environment )
        :   m_environment( environment ),
            m_manifest( m_environment.project_manifest() )
    {
        // create read-write files
        {
            std::vector< mega::io::FileInfo > fileInfos;
            m_manifest.getCompilationFileInfos( mega::io::FileInfo::FILE_Dependencies_DependencyAnalysis, fileInfos );
            VERIFY_RTE( fileInfos.size() == 1U );
            const mega::io::FileInfo& fileInfo = fileInfos.front();
            m_pFile_DependencyAnalysis = std::make_shared< mega::io::File >( fileInfo ) ;
            m_readwriteFiles.insert( std::make_pair( fileInfo.getFileID(), m_pFile_DependencyAnalysis ) ); 
        }
    }


    void store()
    {
        for ( FileMap::const_iterator 
                i = m_readwriteFiles.begin(), 
                iEnd = m_readwriteFiles.end();
              i != iEnd; ++i )
        {
            i->second->store( m_manifest );
        }
    }

    const mega::io::Environment& m_environment;
    const mega::io::Manifest m_manifest;

    std::shared_ptr< mega::io::File > m_pFile_DependencyAnalysis;

    FileMap m_readonlyFiles;
    FileMap m_readwriteFiles;
};

Database::Database( const mega::io::Environment& environment )
:   m_pimpl( new Pimpl( environment ) )
{
}

void Database::store()
{
    m_pimpl->store();
}


namespace Components
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Component
Component::Component( void* pData )
:   m_pData( pData )
{
}
const std::string& Component::get_name() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->name;
}
const boost::filesystem::path& Component::get_directory() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->directory;
}
const std::vector< boost::filesystem::path >& Component::get_includeDirectories() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->includeDirectories;
}
const std::vector< boost::filesystem::path >& Component::get_sourceFiles() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->sourceFiles;
}

} // Components
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Opaque
Opaque::Opaque( void* pData )
:   m_pData( pData )
{
}
const std::string& Opaque::get_str() const
{
    using PrimaryObjectPart = data::AST::Opaque;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->str;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Dimension
Dimension::Dimension( void* pData )
:   m_pData( pData )
{
}
const bool& Dimension::get_isConst() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->isConst;
}
const std::string& Dimension::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Dimension::get_type() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->type;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Context
Context::Context( void* pData )
:   m_pData( pData )
{
}
const std::string& Context::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Context::get_body() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->pBody_Context->body;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Parser
namespace Interface
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Interface


}
namespace Analysis
{
struct Database::Pimpl
{
    using FileMap = std::map< mega::io::ObjectInfo::FileID, mega::io::File::Ptr >;
    using FileMapCst = std::map< mega::io::ObjectInfo::FileID, mega::io::File::PtrCst >;
    Pimpl( const mega::io::Environment& environment, const boost::filesystem::path& objectFile )
        :   m_environment( environment ),
            m_objectFile( objectFile ),
            m_manifest( m_environment.project_manifest() )
    {
        // create read-write files
        {
            std::vector< mega::io::FileInfo > fileInfos;
            m_manifest.getCompilationFileInfos( mega::io::FileInfo::FILE_Analysis_ObjectCompile, m_objectFile, fileInfos );
            VERIFY_RTE( fileInfos.size() == 1U );
            const mega::io::FileInfo& fileInfo = fileInfos.front();
            m_pFile_ObjectCompile = std::make_shared< mega::io::File >( fileInfo ) ;
            m_readwriteFiles.insert( std::make_pair( fileInfo.getFileID(), m_pFile_ObjectCompile ) ); 
        }
        {
            std::vector< mega::io::FileInfo > fileInfos;
            m_manifest.getCompilationFileInfos( mega::io::FileInfo::FILE_Analysis_ObjectAnalysis, m_objectFile, fileInfos );
            VERIFY_RTE( fileInfos.size() == 1U );
            const mega::io::FileInfo& fileInfo = fileInfos.front();
            m_pFile_ObjectAnalysis = std::make_shared< mega::io::File >( fileInfo ) ;
            m_readwriteFiles.insert( std::make_pair( fileInfo.getFileID(), m_pFile_ObjectAnalysis ) ); 
        }
    }



    void store()
    {
        for ( FileMap::const_iterator 
                i = m_readwriteFiles.begin(), 
                iEnd = m_readwriteFiles.end();
              i != iEnd; ++i )
        {
            i->second->store( m_manifest );
        }
    }

    const mega::io::Environment& m_environment;
    boost::filesystem::path m_objectFile;
    const mega::io::Manifest m_manifest;

    std::shared_ptr< mega::io::File > m_pFile_ObjectCompile;
    std::shared_ptr< mega::io::File > m_pFile_ObjectAnalysis;

    FileMap m_readonlyFiles;
    FileMap m_readwriteFiles;
};

Database::Database( const mega::io::Environment& environment, const boost::filesystem::path& objectFile )
:   m_pimpl( new Pimpl( environment, objectFile ) )
{
}

void Database::store()
{
    m_pimpl->store();
}


namespace Components
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Component
Component::Component( void* pData )
:   m_pData( pData )
{
}
const std::string& Component::get_name() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->name;
}
const boost::filesystem::path& Component::get_directory() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->directory;
}
const std::vector< boost::filesystem::path >& Component::get_includeDirectories() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->includeDirectories;
}
const std::vector< boost::filesystem::path >& Component::get_sourceFiles() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->sourceFiles;
}

} // Components
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Opaque
Opaque::Opaque( void* pData )
:   m_pData( pData )
{
}
const std::string& Opaque::get_str() const
{
    using PrimaryObjectPart = data::AST::Opaque;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->str;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Dimension
Dimension::Dimension( void* pData )
:   m_pData( pData )
{
}
const bool& Dimension::get_isConst() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->isConst;
}
const std::string& Dimension::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Dimension::get_type() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->type;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Context
Context::Context( void* pData )
:   m_pData( pData )
{
}
const std::string& Context::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Context::get_body() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->pBody_Context->body;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Parser
namespace Interface
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Interface


}
namespace Concrete
{
struct Database::Pimpl
{
    using FileMap = std::map< mega::io::ObjectInfo::FileID, mega::io::File::Ptr >;
    using FileMapCst = std::map< mega::io::ObjectInfo::FileID, mega::io::File::PtrCst >;

    Pimpl( const mega::io::Environment& environment )
        :   m_environment( environment ),
            m_manifest( m_environment.project_manifest() )
    {
        // create read-write files
        {
            std::vector< mega::io::FileInfo > fileInfos;
            m_manifest.getCompilationFileInfos( mega::io::FileInfo::FILE_Concrete_Tree, fileInfos );
            VERIFY_RTE( fileInfos.size() == 1U );
            const mega::io::FileInfo& fileInfo = fileInfos.front();
            m_pFile_Tree = std::make_shared< mega::io::File >( fileInfo ) ;
            m_readwriteFiles.insert( std::make_pair( fileInfo.getFileID(), m_pFile_Tree ) ); 
        }
    }


    void store()
    {
        for ( FileMap::const_iterator 
                i = m_readwriteFiles.begin(), 
                iEnd = m_readwriteFiles.end();
              i != iEnd; ++i )
        {
            i->second->store( m_manifest );
        }
    }

    const mega::io::Environment& m_environment;
    const mega::io::Manifest m_manifest;

    std::shared_ptr< mega::io::File > m_pFile_Tree;

    FileMap m_readonlyFiles;
    FileMap m_readwriteFiles;
};

Database::Database( const mega::io::Environment& environment )
:   m_pimpl( new Pimpl( environment ) )
{
}

void Database::store()
{
    m_pimpl->store();
}


namespace Components
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Component
Component::Component( void* pData )
:   m_pData( pData )
{
}
const std::string& Component::get_name() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->name;
}
const boost::filesystem::path& Component::get_directory() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->directory;
}
const std::vector< boost::filesystem::path >& Component::get_includeDirectories() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->includeDirectories;
}
const std::vector< boost::filesystem::path >& Component::get_sourceFiles() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->sourceFiles;
}

} // Components
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Opaque
Opaque::Opaque( void* pData )
:   m_pData( pData )
{
}
const std::string& Opaque::get_str() const
{
    using PrimaryObjectPart = data::AST::Opaque;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->str;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Dimension
Dimension::Dimension( void* pData )
:   m_pData( pData )
{
}
const bool& Dimension::get_isConst() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->isConst;
}
const std::string& Dimension::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Dimension::get_type() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->type;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Context
Context::Context( void* pData )
:   m_pData( pData )
{
}
const std::string& Context::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Context::get_body() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->pBody_Context->body;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Parser
namespace Interface
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Interface


}
namespace Layout
{
struct Database::Pimpl
{
    using FileMap = std::map< mega::io::ObjectInfo::FileID, mega::io::File::Ptr >;
    using FileMapCst = std::map< mega::io::ObjectInfo::FileID, mega::io::File::PtrCst >;

    Pimpl( const mega::io::Environment& environment )
        :   m_environment( environment ),
            m_manifest( m_environment.project_manifest() )
    {
        // create read-write files
        {
            std::vector< mega::io::FileInfo > fileInfos;
            m_manifest.getCompilationFileInfos( mega::io::FileInfo::FILE_Layout_Buffers, fileInfos );
            VERIFY_RTE( fileInfos.size() == 1U );
            const mega::io::FileInfo& fileInfo = fileInfos.front();
            m_pFile_Buffers = std::make_shared< mega::io::File >( fileInfo ) ;
            m_readwriteFiles.insert( std::make_pair( fileInfo.getFileID(), m_pFile_Buffers ) ); 
        }
    }


    void store()
    {
        for ( FileMap::const_iterator 
                i = m_readwriteFiles.begin(), 
                iEnd = m_readwriteFiles.end();
              i != iEnd; ++i )
        {
            i->second->store( m_manifest );
        }
    }

    const mega::io::Environment& m_environment;
    const mega::io::Manifest m_manifest;

    std::shared_ptr< mega::io::File > m_pFile_Buffers;

    FileMap m_readonlyFiles;
    FileMap m_readwriteFiles;
};

Database::Database( const mega::io::Environment& environment )
:   m_pimpl( new Pimpl( environment ) )
{
}

void Database::store()
{
    m_pimpl->store();
}


namespace Components
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Component
Component::Component( void* pData )
:   m_pData( pData )
{
}
const std::string& Component::get_name() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->name;
}
const boost::filesystem::path& Component::get_directory() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->directory;
}
const std::vector< boost::filesystem::path >& Component::get_includeDirectories() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->includeDirectories;
}
const std::vector< boost::filesystem::path >& Component::get_sourceFiles() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->sourceFiles;
}

} // Components
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Opaque
Opaque::Opaque( void* pData )
:   m_pData( pData )
{
}
const std::string& Opaque::get_str() const
{
    using PrimaryObjectPart = data::AST::Opaque;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->str;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Dimension
Dimension::Dimension( void* pData )
:   m_pData( pData )
{
}
const bool& Dimension::get_isConst() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->isConst;
}
const std::string& Dimension::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Dimension::get_type() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->type;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Context
Context::Context( void* pData )
:   m_pData( pData )
{
}
const std::string& Context::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Context::get_body() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->pBody_Context->body;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Parser
namespace Interface
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Interface


}
namespace HyperGraph
{
struct Database::Pimpl
{
    using FileMap = std::map< mega::io::ObjectInfo::FileID, mega::io::File::Ptr >;
    using FileMapCst = std::map< mega::io::ObjectInfo::FileID, mega::io::File::PtrCst >;

    Pimpl( const mega::io::Environment& environment )
        :   m_environment( environment ),
            m_manifest( m_environment.project_manifest() )
    {
        // create read-write files
        {
            std::vector< mega::io::FileInfo > fileInfos;
            m_manifest.getCompilationFileInfos( mega::io::FileInfo::FILE_HyperGraph_Graph, fileInfos );
            VERIFY_RTE( fileInfos.size() == 1U );
            const mega::io::FileInfo& fileInfo = fileInfos.front();
            m_pFile_Graph = std::make_shared< mega::io::File >( fileInfo ) ;
            m_readwriteFiles.insert( std::make_pair( fileInfo.getFileID(), m_pFile_Graph ) ); 
        }
    }


    void store()
    {
        for ( FileMap::const_iterator 
                i = m_readwriteFiles.begin(), 
                iEnd = m_readwriteFiles.end();
              i != iEnd; ++i )
        {
            i->second->store( m_manifest );
        }
    }

    const mega::io::Environment& m_environment;
    const mega::io::Manifest m_manifest;

    std::shared_ptr< mega::io::File > m_pFile_Graph;

    FileMap m_readonlyFiles;
    FileMap m_readwriteFiles;
};

Database::Database( const mega::io::Environment& environment )
:   m_pimpl( new Pimpl( environment ) )
{
}

void Database::store()
{
    m_pimpl->store();
}


namespace Components
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Component
Component::Component( void* pData )
:   m_pData( pData )
{
}
const std::string& Component::get_name() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->name;
}
const boost::filesystem::path& Component::get_directory() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->directory;
}
const std::vector< boost::filesystem::path >& Component::get_includeDirectories() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->includeDirectories;
}
const std::vector< boost::filesystem::path >& Component::get_sourceFiles() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->sourceFiles;
}

} // Components
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Opaque
Opaque::Opaque( void* pData )
:   m_pData( pData )
{
}
const std::string& Opaque::get_str() const
{
    using PrimaryObjectPart = data::AST::Opaque;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->str;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Dimension
Dimension::Dimension( void* pData )
:   m_pData( pData )
{
}
const bool& Dimension::get_isConst() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->isConst;
}
const std::string& Dimension::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Dimension::get_type() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->type;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Context
Context::Context( void* pData )
:   m_pData( pData )
{
}
const std::string& Context::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Context::get_body() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->pBody_Context->body;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Parser
namespace Interface
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Interface


}
namespace Derivations
{
struct Database::Pimpl
{
    using FileMap = std::map< mega::io::ObjectInfo::FileID, mega::io::File::Ptr >;
    using FileMapCst = std::map< mega::io::ObjectInfo::FileID, mega::io::File::PtrCst >;

    Pimpl( const mega::io::Environment& environment )
        :   m_environment( environment ),
            m_manifest( m_environment.project_manifest() )
    {
        // create read-write files
        {
            std::vector< mega::io::FileInfo > fileInfos;
            m_manifest.getCompilationFileInfos( mega::io::FileInfo::FILE_Derivations_Invocations, fileInfos );
            VERIFY_RTE( fileInfos.size() == 1U );
            const mega::io::FileInfo& fileInfo = fileInfos.front();
            m_pFile_Invocations = std::make_shared< mega::io::File >( fileInfo ) ;
            m_readwriteFiles.insert( std::make_pair( fileInfo.getFileID(), m_pFile_Invocations ) ); 
        }
    }


    void store()
    {
        for ( FileMap::const_iterator 
                i = m_readwriteFiles.begin(), 
                iEnd = m_readwriteFiles.end();
              i != iEnd; ++i )
        {
            i->second->store( m_manifest );
        }
    }

    const mega::io::Environment& m_environment;
    const mega::io::Manifest m_manifest;

    std::shared_ptr< mega::io::File > m_pFile_Invocations;

    FileMap m_readonlyFiles;
    FileMap m_readwriteFiles;
};

Database::Database( const mega::io::Environment& environment )
:   m_pimpl( new Pimpl( environment ) )
{
}

void Database::store()
{
    m_pimpl->store();
}


namespace Components
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Component
Component::Component( void* pData )
:   m_pData( pData )
{
}
const std::string& Component::get_name() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->name;
}
const boost::filesystem::path& Component::get_directory() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->directory;
}
const std::vector< boost::filesystem::path >& Component::get_includeDirectories() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->includeDirectories;
}
const std::vector< boost::filesystem::path >& Component::get_sourceFiles() const
{
    using PrimaryObjectPart = data::Components::Component;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->sourceFiles;
}

} // Components
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Opaque
Opaque::Opaque( void* pData )
:   m_pData( pData )
{
}
const std::string& Opaque::get_str() const
{
    using PrimaryObjectPart = data::AST::Opaque;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->str;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Dimension
Dimension::Dimension( void* pData )
:   m_pData( pData )
{
}
const bool& Dimension::get_isConst() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->isConst;
}
const std::string& Dimension::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Dimension::get_type() const
{
    using PrimaryObjectPart = data::AST::Dimension;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->type;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Context
Context::Context( void* pData )
:   m_pData( pData )
{
}
const std::string& Context::get_identifier() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->identifier;
}
const std::string& Context::get_body() const
{
    using PrimaryObjectPart = data::AST::Context;
    PrimaryObjectPart* pData = reinterpret_cast< PrimaryObjectPart* >( m_pData );
    return pData->pBody_Context->body;
}

} // Parser
namespace Parser
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Parser
namespace Interface
{
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// Root
Root::Root( void* pData )
:   m_pData( pData )
{
}

} // Interface


}
