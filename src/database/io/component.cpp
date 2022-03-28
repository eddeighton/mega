
#include "database/io/component.hpp"
#include "database/io/object.hpp"
#include "database/io/environment.hpp"

#include <boost/filesystem/operations.hpp>
#include <iostream>
#include <string>

namespace mega
{
namespace io
{
    static const std::string strComponentID = "component:";

    Component::Component()
    {
    }

    void Component::addSourceFileInfos( const std::vector< FileInfo >& sourceFileInfos )
    {
        std::copy( sourceFileInfos.begin(), sourceFileInfos.end(),
                   std::back_inserter( m_fileInfos ) );
    }

    void Component::collectFileInfos( std::vector< FileInfo >& fileInfos ) const
    {
        std::copy( m_fileInfos.begin(), m_fileInfos.end(),
                   std::back_inserter( fileInfos ) );

        for ( Component::Ptr pComponent : m_components )
        {
            pComponent->collectFileInfos( fileInfos );
        }
    }

    void Component::addComponent( Ptr pComponent )
    {
        m_components.push_back( pComponent );
    }

    void Component::addCompilationFiles( const Environment& environment )
    {
        std::vector< FileInfo > compilationFileInfos;
        for ( FileInfo& fileInfo : m_fileInfos )
        {
            {
                FileInfo compilationFile;
                compilationFile.m_fileID = Object::NO_FILE;
                compilationFile.m_fileType = FileInfo::ObjectAST;
                compilationFile.m_filePath = environment.objectAST( fileInfo.m_filePath );
                compilationFile.m_objectSourceFilePath = fileInfo.m_filePath;
                compilationFileInfos.push_back( compilationFile );
            }

            {
                FileInfo compilationFile;
                compilationFile.m_fileID = Object::NO_FILE;
                compilationFile.m_fileType = FileInfo::ObjectBody;
                compilationFile.m_filePath = environment.objectBody( fileInfo.m_filePath );
                compilationFile.m_objectSourceFilePath = fileInfo.m_filePath;
                compilationFileInfos.push_back( compilationFile );
            }
        }
        addSourceFileInfos( compilationFileInfos );

        for ( Component::Ptr pComponent : m_components )
        {
            pComponent->addCompilationFiles( environment );
        }
    }

    void Component::labelFiles( Object::FileID& fileID )
    {
        for ( FileInfo& fileInfo : m_fileInfos )
        {
            fileInfo.m_fileID = fileID;
            ++fileID;
        }

        for ( Component::Ptr pComponent : m_components )
        {
            pComponent->labelFiles( fileID );
        }
    }

    void Component::load( std::istream& is )
    {
        std::string str;
        while ( std::getline( is, str ) )
        {
            if ( str == strComponentID )
            {
                Component::Ptr pComponent = std::make_shared< Component >();
                m_components.push_back( pComponent );
                pComponent->load( is );
            }
            else
            {
                std::istringstream iss( str );
                FileInfo           sourceFileInfo;
                iss >> sourceFileInfo;
                m_fileInfos.push_back( sourceFileInfo );
            }
        }
    }

    void Component::save( std::ostream& os ) const
    {
        os << strComponentID << std::endl;
        for ( const FileInfo& sourceFileInfo : m_fileInfos )
        {
            os << sourceFileInfo << std::endl;
        }
        for ( Component::Ptr pComponent : m_components )
        {
            pComponent->save( os );
        }
    }

} // namespace io
} // namespace mega