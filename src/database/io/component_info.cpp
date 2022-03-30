
#include "database/io/component_info.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/directory.hpp>
#include <boost/filesystem/file_status.hpp>
#include <fstream>
#include <optional>

namespace mega
{
namespace io
{
    ComponentInfo::ComponentInfo()
    {
    }

    ComponentInfo::ComponentInfo( const std::string&              strName,
                                  const boost::filesystem::path&  directory,
                                  const ComponentInfo::PathArray& sourceFiles,
                                  const PathArray&                includeDirectories )
        : m_strName( strName )
        , m_directory( directory )
        , m_sourceFiles( sourceFiles )
        , m_includeDirectories( includeDirectories )
    {
    }
/*
    ComponentInfo ComponentInfo::load( const boost::filesystem::path& manifestFilePath )
    {
        ComponentInfo::PathArray sourceFiles;
        std::optional< bool >    bIsComponentOpt;

        std::ifstream inputFileStream( manifestFilePath.native().c_str(), std::ios::in );
        if ( !inputFileStream.good() )
        {
            THROW_RTE( "Failed to open file: " << manifestFilePath.string() );
        }

        ComponentInfo ComponentInfo;
        inputFileStream >> ComponentInfo;
        return ComponentInfo;
    }

    void ComponentInfo::save( const boost::filesystem::path& manifestFilePath ) const
    {
        std::ofstream outputFileStream( manifestFilePath, std::ios_base::trunc | std::ios_base::out );
        if ( !outputFileStream.good() )
        {
            THROW_RTE( "Failed to write to file: " << manifestFilePath.string() );
        }
        outputFileStream << *this;
    }
*/
    std::ostream& operator<<( std::ostream& os, const ComponentInfo& ComponentInfo )
    {
        os << ComponentInfo.m_strName << std::endl;
        os << ComponentInfo.m_directory << std::endl;
        for ( const boost::filesystem::path& folderPath : ComponentInfo.m_includeDirectories )
        {
            os << folderPath.generic_string() << std::endl;
        }
        for ( const boost::filesystem::path& sourceFilePath : ComponentInfo.m_sourceFiles )
        {
            os << sourceFilePath.generic_string() << std::endl;
        }

        return os;
    }

    std::istream& operator>>( std::istream& is, ComponentInfo& componentInfo )
    {
        bool        bIsComponentRead = false;
        bool        bDirectoryRead = false;
        std::string str;
        while ( std::getline( is, str ) )
        {
            if ( !bIsComponentRead )
            {
                std::istringstream is( str );
                is >> componentInfo.m_strName;
                bIsComponentRead = true;
            }
            else if( !bDirectoryRead )
            {
                std::istringstream is( str );
                is >> componentInfo.m_directory;
                bDirectoryRead = true;
            }
            else
            {
                std::istringstream      is( str );
                boost::filesystem::path sourcePath;
                is >> sourcePath;
                if ( sourcePath.has_extension() )
                    componentInfo.m_sourceFiles.push_back( sourcePath );
                else
                    componentInfo.m_includeDirectories.push_back( sourcePath );
            }
        }
        return is;
    }

} // namespace io
} // namespace mega