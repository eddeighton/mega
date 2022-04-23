#ifndef ENVIRONMENTS_22_APRIL_2022
#define ENVIRONMENTS_22_APRIL_2022

#include "database/common/archive.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/sources.hpp"

#include "database/model/environment.hxx"

#include "common/stash.hpp"
#include "common/file.hpp"

#include <boost/filesystem/operations.hpp>

namespace mega
{
    namespace io
    {
        class BuildEnvironment : public Environment
        {
            using Path = boost::filesystem::path;
            
            const Path& m_rootSourceDir;
            const Path& m_rootBuildDir;
            const Path& m_tempDir;
            mutable task::Stash m_stash;

            boost::filesystem::path toPath( const ComponentListingFilePath& key ) const { return m_rootBuildDir / key.path(); }
            boost::filesystem::path toPath( const CompilationFilePath& key ) const { return m_rootBuildDir / key.path(); }
            boost::filesystem::path toPath( const SourceFilePath& key ) const { return m_rootSourceDir / key.path(); }

            void copyToTargetPath( const boost::filesystem::path& from, const boost::filesystem::path& to ) const
            {
                VERIFY_RTE( boost::filesystem::exists( from ) );
                if ( boost::filesystem::exists( to ) )
                {
                    boost::filesystem::remove( to );
                }
                boost::filesystem::ensureFoldersExist( to );
                boost::filesystem::copy_file( from, to );
            }

            Path stashDir() const { return m_rootBuildDir / "stash"; }
        public:
            BuildEnvironment( const Path& rootSourceDir, const Path& rootBuildDir, const Path& tempDir )
                : m_rootSourceDir( rootSourceDir )
                , m_rootBuildDir( rootBuildDir )
                , m_tempDir( tempDir )
                , m_stash( stashDir() )
            {
                // m_stash.loadBuildHashCodes( );
            }

            const Path& rootSourceDir() const { return m_rootSourceDir; }
            const Path& rootBuildDir() const { return m_rootBuildDir; }

            Path dependency( const std::string& strOpaque ) const
            {
                std::ostringstream os;
                os << strOpaque << megaFilePath::extension().string();
                return Path( os.str() );
            }

            ComponentListingFilePath ComponentListingFilePath_fromPath( const Path& buildDirectory ) const
            {
                VERIFY_RTE_MSG( boost::filesystem::is_directory( buildDirectory ),
                                "Source List path is not a directory: " << buildDirectory.string() );
                return ComponentListingFilePath( boost::filesystem::relative( buildDirectory / "component.listing", m_rootBuildDir ) );
            }
            manifestFilePath manifestFilePath_fromPath( const boost::filesystem::path& filePath ) const
            {
                return manifestFilePath( boost::filesystem::relative( filePath, m_rootSourceDir ) );
            }
            megaFilePath megaFilePath_fromPath( const boost::filesystem::path& filePath ) const
            {
                return megaFilePath( boost::filesystem::relative( filePath, m_rootSourceDir ) );
            }

            // stash
            common::HashCode getBuildHashCode( const ComponentListingFilePath& key ) const
            {
                return m_stash.getBuildHashCode( toPath( key ) );
            }
            void setBuildHashCode( const ComponentListingFilePath& key, common::HashCode hashCode ) const
            {
                m_stash.setBuildHashCode( toPath( key ), hashCode );
            }
            void setBuildHashCode( const ComponentListingFilePath& key ) const
            {
                m_stash.setBuildHashCode( toPath( key ), common::hash_file( toPath( key ) ) );
            }
            void stash( const ComponentListingFilePath& file, const common::HashCode& code ) const
            {
                m_stash.stash( toPath( file ), code );
            }
            bool restore( const ComponentListingFilePath& file, const common::HashCode& code ) const
            {
                return m_stash.restore( toPath( file ), code );
            }

            common::HashCode getBuildHashCode( const CompilationFilePath& key ) const
            {
                return m_stash.getBuildHashCode( toPath( key ) );
            }
            void setBuildHashCode( const CompilationFilePath& key, common::HashCode hashCode ) const
            {
                m_stash.setBuildHashCode( toPath( key ), hashCode );
            }
            void setBuildHashCode( const CompilationFilePath& key ) const
            {
                m_stash.setBuildHashCode( toPath( key ), common::hash_file( toPath( key ) ) );
            }
            void stash( const CompilationFilePath& file, const common::HashCode& code ) const { m_stash.stash( toPath( file ), code ); }
            bool restore( const CompilationFilePath& file, const common::HashCode& code ) const
            {
                return m_stash.restore( toPath( file ), code );
            }

            common::HashCode getBuildHashCode( const SourceFilePath& key ) const
            {
                return m_stash.getBuildHashCode( toPath( key ) );
            }
            void setBuildHashCode( const SourceFilePath& key, common::HashCode hashCode ) const
            {
                m_stash.setBuildHashCode( toPath( key ), hashCode );
            }
            void setBuildHashCode( const SourceFilePath& key ) const
            {
                m_stash.setBuildHashCode( toPath( key ), common::hash_file( toPath( key ) ) );
            }
            void stash( const SourceFilePath& file, const common::HashCode& code ) const { m_stash.stash( toPath( file ), code ); }
            bool restore( const SourceFilePath& file, const common::HashCode& code ) const
            {
                return m_stash.restore( toPath( file ), code );
            }

            // FileSystem
            virtual std::unique_ptr< std::istream > read( const ComponentListingFilePath& filePath ) const
            {
                return boost::filesystem::createBinaryInputFileStream( toPath( filePath ) );
            }
            virtual std::unique_ptr< std::ostream > write_temp( const ComponentListingFilePath& filePath,
                                                                               boost::filesystem::path&        tempFilePath ) const
            {
                tempFilePath = m_tempDir / filePath.path();
                return boost::filesystem::createBinaryOutputFileStream( tempFilePath );
            }
            virtual void temp_to_real( const ComponentListingFilePath& filePath ) const
            {
                copyToTargetPath( m_tempDir / filePath.path(), toPath( filePath ) );
            }

            virtual std::unique_ptr< std::istream > read( const CompilationFilePath& filePath ) const
            {
                return boost::filesystem::createBinaryInputFileStream( toPath( filePath ) );
            }
            virtual std::unique_ptr< std::ostream > write_temp( const CompilationFilePath& filePath,
                                                                               boost::filesystem::path&   tempFilePath ) const
            {
                tempFilePath = m_tempDir / filePath.path();
                return boost::filesystem::createBinaryOutputFileStream( tempFilePath );
            }
            virtual void temp_to_real( const CompilationFilePath& filePath ) const
            {
                copyToTargetPath( m_tempDir / filePath.path(), toPath( filePath ) );
            }

            virtual std::unique_ptr< std::istream > read( const SourceFilePath& filePath ) const
            {
                return boost::filesystem::createBinaryInputFileStream( toPath( filePath ) );
            }
            virtual std::unique_ptr< std::ostream > write_temp( const SourceFilePath&  filePath,
                                                                               boost::filesystem::path& tempFilePath ) const
            {
                tempFilePath = m_tempDir / filePath.path();
                return boost::filesystem::createBinaryOutputFileStream( tempFilePath );
            }
            virtual void temp_to_real( const SourceFilePath& filePath ) const
            {
                copyToTargetPath( m_tempDir / filePath.path(), toPath( filePath ) );
            }
        };

        class RetailEnvironment : public Environment
        {
            ReadArchive m_fileArchive;
        public:
            RetailEnvironment( const boost::filesystem::path& archiveFilePath )
                :   m_fileArchive( archiveFilePath )
            {
            }


            // FileSystem
            virtual std::unique_ptr< std::istream > read( const ComponentListingFilePath& filePath ) const
            {
                return m_fileArchive.read( filePath );
            }
            virtual std::unique_ptr< std::ostream > write_temp( const ComponentListingFilePath& filePath,
                                                                               boost::filesystem::path&        tempFilePath ) const
            {
                THROW_RTE( "Invalid use of retail environment" );
            }
            virtual void temp_to_real( const ComponentListingFilePath& filePath ) const
            {
                THROW_RTE( "Invalid use of retail environment" );
            }

            virtual std::unique_ptr< std::istream > read( const CompilationFilePath& filePath ) const
            {
                return m_fileArchive.read( filePath );
            }
            virtual std::unique_ptr< std::ostream > write_temp( const CompilationFilePath& filePath,
                                                                               boost::filesystem::path&   tempFilePath ) const
            {
                THROW_RTE( "Invalid use of retail environment" );
            }
            virtual void temp_to_real( const CompilationFilePath& filePath ) const
            {
                THROW_RTE( "Invalid use of retail environment" );
            }

            virtual std::unique_ptr< std::istream > read( const SourceFilePath& filePath ) const
            {
                return m_fileArchive.read( filePath );
            }
            virtual std::unique_ptr< std::ostream > write_temp( const SourceFilePath&  filePath,
                                                                               boost::filesystem::path& tempFilePath ) const
            {
                THROW_RTE( "Invalid use of retail environment" );
            }
            virtual void temp_to_real( const SourceFilePath& filePath ) const
            {
                THROW_RTE( "Invalid use of retail environment" );
            }
        };
        
    } // namespace io
} // namespace mega

#endif // ENVIRONMENTS_22_APRIL_2022