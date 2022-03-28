
#include "database/io/source_listing.hpp"

#include "common/assert_verify.hpp"

#include <fstream>
#include <optional>

namespace mega
{
namespace io
{
    SourceListing::SourceListing()
        :   m_bIsComponent( false )
    {
    }

    SourceListing::SourceListing( const SourceListing::PathArray& sourceFiles, bool bIsComponent )
        : m_sourceFiles( sourceFiles )
        , m_bIsComponent( bIsComponent )
    {
    }

    SourceListing SourceListing::load( const boost::filesystem::path& manifestFilePath )
    {
        SourceListing::PathArray sourceFiles;
        std::optional< bool >  bIsComponentOpt;

        std::ifstream inputFileStream( manifestFilePath.native().c_str(), std::ios::in );
        if ( !inputFileStream.good() )
        {
            THROW_RTE( "Failed to open file: " << manifestFilePath.string() );
        }

        SourceListing sourceListing;
        inputFileStream >> sourceListing;
        return sourceListing;
    }

    void SourceListing::save( const boost::filesystem::path& manifestFilePath ) const
    {
        std::ofstream outputFileStream( manifestFilePath, std::ios_base::trunc | std::ios_base::out );
        if ( !outputFileStream.good() )
        {
            THROW_RTE( "Failed to write to file: " << manifestFilePath.string() );
        }
        outputFileStream << *this;
    }
    
    std::ostream& operator<<( std::ostream& os, const SourceListing& sourceListing )
    {
        os << sourceListing.m_bIsComponent << std::endl;
        for ( const boost::filesystem::path& sourceFilePath : sourceListing.m_sourceFiles )
        {
            os << sourceFilePath.generic_string() << std::endl;
        }
        return os;
    }

    std::istream& operator>>( std::istream& is, SourceListing& sourceListing )
    {
        bool bIsComponentRead = false;
        std::string str;
        while ( std::getline( is, str ) )
        {
            if ( !bIsComponentRead )
            {
                std::istringstream is( str );
                is >> sourceListing.m_bIsComponent;
                bIsComponentRead = true;
            }
            else
            {
                std::istringstream is( str );
                boost::filesystem::path sourcePath;
                is >> sourcePath;
                sourceListing.m_sourceFiles.push_back( sourcePath );
            }
        }
        return is;
    }

} // namespace io
} // namespace mega