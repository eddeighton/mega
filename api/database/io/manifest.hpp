#ifndef IO_MANIFEST_26_MAR_2022
#define IO_MANIFEST_26_MAR_2022

#include "object.hpp"
#include "file_info.hpp"
#include "environment.hpp"

#include "database/io/archive.hpp"

#include <boost/filesystem/path.hpp>

#include <memory>

namespace mega
{
namespace io
{
    class Component;
    class Manifest
    {
        using FileVector = std::vector< boost::filesystem::path >;
        using FileInfoVector = std::vector< FileInfo >;

    public:
        using PtrCst = std::shared_ptr< const Manifest >;

        Manifest();

        // Load an existing manifest from the specified file.
        Manifest( const boost::filesystem::path& filepath );

        // Construct a manifest from the source and build directory by
        // recursively analysing existing source listing files.
        Manifest( const Environment& environment, const std::vector< boost::filesystem::path >& sourceListings );

        const FileVector& getSourceFiles() const { return m_sourceFiles; }
        const FileInfoVector& getCompilationFileInfos() const { return m_compilationFileInfos; }
        const FileInfo& getCompilationFilePath( ObjectInfo::FileID fileID ) const;

        void getCompilationFileInfos( FileInfo::Type fileType, FileInfoVector& fileInfos ) const;
        void getCompilationFileInfos( FileInfo::Type fileType, const boost::filesystem::path& objectFile, FileInfoVector& fileInfos ) const;

        void load( const boost::filesystem::path& filepath );
        void save( const boost::filesystem::path& filepath ) const;

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& boost::serialization::make_nvp( "sourceFiles", m_sourceFiles );
            archive& boost::serialization::make_nvp( "compilationFileInfos", m_compilationFileInfos );
        }

    private: 
        FileVector m_sourceFiles;
        FileInfoVector m_compilationFileInfos;
    };

} // namespace io
} // namespace mega

#endif // IO_MANIFEST_26_MAR_2022