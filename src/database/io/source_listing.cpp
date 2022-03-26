
#include "database/io/source_listing.hpp"

#include "common/assert_verify.hpp"

#include <fstream>
#include <optional>

namespace mega
{
namespace io
{

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

        std::string str;
        while ( std::getline( inputFileStream, str ) )
        {
            if ( !bIsComponentOpt.has_value() )
            {
                std::istringstream is( str );
                bool               bIsComponent = false;
                is >> bIsComponent;
                bIsComponentOpt = bIsComponent;
            }
            else
            {
                std::istringstream is( str );

                boost::filesystem::path sourcePath;
                is >> sourcePath;

                sourceFiles.push_back( sourcePath );
            }
        }

        return SourceListing( sourceFiles, bIsComponentOpt.value_or( false ) );
    }

    void SourceListing::save( const boost::filesystem::path& manifestFilePath ) const
    {
        std::ofstream outputFileStream( manifestFilePath, std::ios_base::trunc | std::ios_base::out );
        if ( !outputFileStream.good() )
        {
            THROW_RTE( "Failed to write to file: " << manifestFilePath.string() );
        }

        outputFileStream << m_bIsComponent << std::endl;

        for ( const boost::filesystem::path& sourceFilePath : m_sourceFiles )
        {
            outputFileStream << sourceFilePath.generic_string() << std::endl;
        }
    }

    boost::filesystem::path SourceListing::filepath( const boost::filesystem::path& buildDir )
    {
        return buildDir / "source_list.txt";
    }
    
} // namespace io
} // namespace mega