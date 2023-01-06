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

#include "database/common/archive.hpp"
#include "database/common/file_header.hpp"
#include "database/common/exception.hpp"

#include "database/model/file_info.hxx"

#include "common/assert_verify.hpp"

#include <boost/iostreams/stream.hpp>
#include "database/types/sources.hpp"

#include <boost/filesystem.hpp>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/copy.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <vector>

namespace boost
{
namespace iostreams
{

namespace detail
{
template < typename Source, typename Sink >
std::streamsize eds_copy_impl( Source src, Sink snk, std::streamsize buffer_size )
{
    using namespace std;
    typedef typename char_type_of< Source >::type src_char;
    typedef typename char_type_of< Sink >::type   snk_char;
    BOOST_STATIC_ASSERT( ( is_same< src_char, snk_char >::value ) );
    return copy_operation< Source, Sink >( src, snk, buffer_size )();
}
} // namespace detail

template < typename Source, typename Sink >
std::streamsize eds_copy( Source& src, Sink& snk,
                          std::streamsize              buffer_size
                          = default_device_buffer_size BOOST_IOSTREAMS_ENABLE_IF_STREAM( Source )
                              BOOST_IOSTREAMS_ENABLE_IF_STREAM( Sink ) )
{
    return detail::eds_copy_impl( detail::wrap( src ), detail::wrap( snk ), buffer_size );
}

} // namespace iostreams
} // namespace boost

namespace mega::io
{

struct TableOfContents
{
    struct File
    {
        using PositionType = mega::U64;

        File::PositionType m_start, m_length;
        std::string        m_id;

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& m_start;
            archive& m_length;
            archive& m_id;
        }
    };

    using FileVector = std::vector< File >;
    FileVector m_sourceFiles;
    FileVector m_buildFiles;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_sourceFiles;
        archive& m_buildFiles;
    }

    const File& getSourceFile( const std::string& strID ) const
    {
        for ( FileVector::const_iterator i = m_sourceFiles.begin(), iEnd = m_sourceFiles.end(); i != iEnd; ++i )
        {
            if ( i->m_id == strID )
                return *i;
        }
        THROW_RTE( "Failed to locate file: " << strID );
    }

    const File& getBuildFile( const std::string& strID ) const
    {
        for ( FileVector::const_iterator i = m_buildFiles.begin(), iEnd = m_buildFiles.end(); i != iEnd; ++i )
        {
            if ( i->m_id == strID )
                return *i;
        }
        THROW_RTE( "Failed to locate file: " << strID );
    }
};

struct ReadArchive::Pimpl
{
    using MappedSourceType = boost::iostreams::mapped_file_source;
    // using MappedStreamType = boost::iostreams::stream< MappedSourceType >;

    struct StreamImpl : public boost::iostreams::stream< MappedSourceType >
    {
        std::unique_ptr< MappedSourceType > m_pSource;

        StreamImpl( std::unique_ptr< MappedSourceType > pSource, mega::U64 szOffset )
            : boost::iostreams::stream< MappedSourceType >( *pSource )
            , m_pSource( std::move( pSource ) )
        {
            seekg( szOffset );
        }

        static std::unique_ptr< StreamImpl > make_stream_impl( const boost::filesystem::path& filePath,
                                                               const TableOfContents::File&   file )
        {
            std::unique_ptr< MappedSourceType > pMappedFile;
            mega::U64                           szOffset = 0U;
            try
            {
                const mega::U64 szAlignment = MappedSourceType::alignment();

                // ensure the mapping starts at alignment boundary
                const mega::U64 szStart = file.m_start - file.m_start % szAlignment;

                // record the addition offset from szStart where the embeded file starts
                szOffset = file.m_start - szStart;

                // also ensure the length accounts for the offset
                const mega::U64 szLength = file.m_length + szOffset;

                pMappedFile = std::make_unique< MappedSourceType >( filePath, szLength, szStart );
            }
            catch ( std::exception& ex )
            {
                THROW_RTE( "Failed loading from archive: " << filePath.string() << " archive file: " << file.m_id
                                                           << " at: " << file.m_start << " with length: "
                                                           << file.m_length << " Error:" << ex.what() );
            }
            return std::make_unique< StreamImpl >( std::move( pMappedFile ), szOffset );
        }
    };

    using SourceType = boost::iostreams::basic_file_source< char >;
    using StreamType = boost::iostreams::stream< SourceType >;

    Pimpl( const boost::filesystem::path& filePath )
        : m_filePath( filePath )
    {
        VERIFY_RTE_MSG(
            boost::filesystem::exists( m_filePath ), "Failed to locate archive file: " << m_filePath.string() );
        // read the table of contents
        SourceType                      fileSource( m_filePath.string() );
        StreamType                      istream( fileSource );
        boost::archive::binary_iarchive archive( istream );
        archive&                        m_toc;
    }

    std::unique_ptr< std::istream > read( const TableOfContents::File& file )
    {
        return StreamImpl::make_stream_impl( m_filePath, file );
    }

    void verify()
    {
        // ensure ALL compilation files have correct environment database version
        for ( const TableOfContents::File& file : m_toc.m_buildFiles )
        {
            std::unique_ptr< std::istream > pIS = StreamImpl::make_stream_impl( m_filePath, file );
            boost::archive::binary_iarchive fileArchive( *pIS );
            mega::io::FileHeader            fileHeader;
            fileArchive&                    fileHeader;

            if ( fileHeader.getVersion() != mega::io::Environment::getVersion() )
            {
                std::ostringstream os;
                os << "Archive compilation file: " << file.m_id << " has incorrect database version";
                throw mega::io::DatabaseVersionException( os.str() );
            }
        }
    }

    const boost::filesystem::path m_filePath;
    TableOfContents               m_toc;
};

ReadArchive::ReadArchive( const boost::filesystem::path& filePath )
    : m_pImpl( new Pimpl( filePath ) )
{
}

std::unique_ptr< std::istream > ReadArchive::read( const SourceFilePath& filePath ) const
{
    const TableOfContents::File& file = m_pImpl->m_toc.getSourceFile( filePath.path().string() );
    return m_pImpl->read( file );
}

std::unique_ptr< std::istream > ReadArchive::read( const BuildFilePath& filePath ) const
{
    const TableOfContents::File& file = m_pImpl->m_toc.getBuildFile( filePath.path().string() );
    return m_pImpl->read( file );
}

void ReadArchive::verify() { m_pImpl->verify(); }

void ReadArchive::compile_archive( const boost::filesystem::path& filePath, const Manifest& manifest,
                                   const boost::filesystem::path& srcDir, const boost::filesystem::path& buildDir )
{
    TableOfContents toc;

    struct StreamPos
    {
        TableOfContents::File::PositionType operator()( std::ofstream& os ) const
        {
            const std::basic_ostream< char >::pos_type position = os.tellp();
            VERIFY_RTE( position != -1 );
            return static_cast< TableOfContents::File::PositionType >( position );
        }
    };

    std::ofstream ofStream( filePath.c_str(), std::ios::binary | std::ios::out | std::ios::trunc );

    VERIFY_RTE( StreamPos()( ofStream ) == 0U );

    // add files to the table of contents
    {
        const manifestFilePath& manifestFilePath = manifest.getManifestFilePath();

        TableOfContents::File file;
        file.m_id = manifestFilePath.path().string();
        toc.m_sourceFiles.push_back( file );
    }
    for ( const FileInfo& compilationFile : manifest.getCompilationFileInfos() )
    {
        TableOfContents::File file;
        file.m_id = compilationFile.getFilePath().path().string();
        toc.m_buildFiles.push_back( file );
    }

    // write initial version of the toc
    {
        boost::archive::binary_oarchive archive( ofStream );
        archive&                        toc;
    }

    VERIFY_RTE( ofStream.is_open() );

    // now write the actual files
    for ( TableOfContents::File& file : toc.m_sourceFiles )
    {
        file.m_start = StreamPos()( ofStream );
        {
            const boost::filesystem::path  filePath   = srcDir / file.m_id;
            const mega::U64                szFileSize = boost::filesystem::file_size( filePath );
            ReadArchive::Pimpl::SourceType fileSource( filePath.string() );
            ReadArchive::Pimpl::StreamType istream( fileSource );
            boost::iostreams::eds_copy( istream, ofStream, szFileSize );
        }
        file.m_length = StreamPos()( ofStream ) - file.m_start;
    }

    for ( TableOfContents::File& file : toc.m_buildFiles )
    {
        file.m_start = StreamPos()( ofStream );
        {
            const boost::filesystem::path  filePath   = buildDir / file.m_id;
            const mega::U64                szFileSize = boost::filesystem::file_size( filePath );
            ReadArchive::Pimpl::SourceType fileSource( filePath.string() );
            ReadArchive::Pimpl::StreamType istream( fileSource );
            boost::iostreams::eds_copy( istream, ofStream, szFileSize );
        }
        file.m_length = StreamPos()( ofStream ) - file.m_start;
    }

    // re-write the toc with actual offsets
    {
        ofStream.seekp( 0U );
        boost::archive::binary_oarchive archive( ofStream );
        archive&                        toc;
    }
}

} // namespace mega::io
