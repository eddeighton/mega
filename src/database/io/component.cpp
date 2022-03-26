
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

    void Component::addSourceFileInfos( const std::vector< File::Info >& sourceFileInfos )
    {
        std::copy( sourceFileInfos.begin(), sourceFileInfos.end(),
                   std::back_inserter( m_fileInfos ) );
    }

    void Component::addComponent( Ptr pComponent )
    {
        m_components.push_back( pComponent );
    }

    void Component::addCompilationFiles( const boost::filesystem::path& sourceDirectory,
                                         const boost::filesystem::path& buildDirectory )
    {
        std::vector< File::Info > compilationFileInfos;
        for ( File::Info& fileInfo : m_fileInfos )
        {
            const boost::filesystem::path sourceFolder = fileInfo.m_filePath.parent_path();
            const boost::filesystem::path buildPath = buildDirectory / boost::filesystem::relative( sourceFolder, sourceDirectory );
            const Environment             environment( sourceDirectory, buildDirectory, sourceFolder, buildPath );

            {
                File::Info compilationFile;
                compilationFile.m_fileID = Object::NO_FILE;
                compilationFile.m_fileType = File::Info::ObjectAST;
                compilationFile.m_filePath = environment.objectAST( fileInfo.m_filePath );
                compilationFile.m_objectSourceFilePath = fileInfo.m_filePath;
                compilationFileInfos.push_back( compilationFile );
            }

            {
                File::Info compilationFile;
                compilationFile.m_fileID = Object::NO_FILE;
                compilationFile.m_fileType = File::Info::ObjectBody;
                compilationFile.m_filePath = environment.objectBody( fileInfo.m_filePath );
                compilationFile.m_objectSourceFilePath = fileInfo.m_filePath;
                compilationFileInfos.push_back( compilationFile );
            }
        }
        addSourceFileInfos( compilationFileInfos );

        for ( Component::Ptr pComponent : m_components )
        {
            pComponent->addCompilationFiles( sourceDirectory, buildDirectory );
        }
    }

    void Component::labelFiles( Object::FileID& fileID )
    {
        for ( File::Info& fileInfo : m_fileInfos )
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
                File::Info         sourceFileInfo;
                iss >> sourceFileInfo;
                m_fileInfos.push_back( sourceFileInfo );
            }
        }
    }

    void Component::save( std::ostream& os ) const
    {
        os << strComponentID << std::endl;
        for ( const File::Info& sourceFileInfo : m_fileInfos )
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