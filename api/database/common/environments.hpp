#ifndef ENVIRONMENTS_22_APRIL_2022
#define ENVIRONMENTS_22_APRIL_2022

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

        public:
            BuildEnvironment( const Path& rootSourceDir, const Path& rootBuildDir, const Path& tempDir )
                : Environment( rootSourceDir, rootBuildDir, tempDir )
                , m_stash( stashDir() )
            {
                // m_stash.loadBuildHashCodes( );
            }

            // stash
            virtual common::HashCode getBuildHashCode( const ComponentListingFilePath& key ) const
            {
                return m_stash.getBuildHashCode( toPath( key ) );
            }
            virtual void setBuildHashCode( const ComponentListingFilePath& key, common::HashCode hashCode ) const
            {
                m_stash.setBuildHashCode( toPath( key ), hashCode );
            }
            virtual void setBuildHashCode( const ComponentListingFilePath& key ) const
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

            virtual common::HashCode getBuildHashCode( const CompilationFilePath& key ) const
            {
                return m_stash.getBuildHashCode( toPath( key ) );
            }
            virtual void setBuildHashCode( const CompilationFilePath& key, common::HashCode hashCode ) const
            {
                m_stash.setBuildHashCode( toPath( key ), hashCode );
            }
            virtual void setBuildHashCode( const CompilationFilePath& key ) const
            {
                m_stash.setBuildHashCode( toPath( key ), common::hash_file( toPath( key ) ) );
            }
            void stash( const CompilationFilePath& file, const common::HashCode& code ) const { m_stash.stash( toPath( file ), code ); }
            bool restore( const CompilationFilePath& file, const common::HashCode& code ) const
            {
                return m_stash.restore( toPath( file ), code );
            }

            virtual common::HashCode getBuildHashCode( const SourceFilePath& key ) const
            {
                return m_stash.getBuildHashCode( toPath( key ) );
            }
            virtual void setBuildHashCode( const SourceFilePath& key, common::HashCode hashCode ) const
            {
                m_stash.setBuildHashCode( toPath( key ), hashCode );
            }
            virtual void setBuildHashCode( const SourceFilePath& key ) const
            {
                m_stash.setBuildHashCode( toPath( key ), common::hash_file( toPath( key ) ) );
            }
            void stash( const SourceFilePath& file, const common::HashCode& code ) const { m_stash.stash( toPath( file ), code ); }
            bool restore( const SourceFilePath& file, const common::HashCode& code ) const
            {
                return m_stash.restore( toPath( file ), code );
            }

            // file io
            virtual std::unique_ptr< boost::filesystem::ifstream > read( const ComponentListingFilePath& filePath ) const
            {
                return boost::filesystem::createBinaryInputFileStream( toPath( filePath ) );
            }
            virtual std::unique_ptr< boost::filesystem::ofstream > write_temp( const ComponentListingFilePath& filePath,
                                                                               boost::filesystem::path&        tempFilePath ) const
            {
                tempFilePath = m_tempDir / filePath.path();
                return boost::filesystem::createBinaryOutputFileStream( tempFilePath );
            }
            virtual void temp_to_real( const ComponentListingFilePath& filePath ) const
            {
                copyToTargetPath( m_tempDir / filePath.path(), toPath( filePath ) );
            }

            virtual std::unique_ptr< boost::filesystem::ifstream > read( const CompilationFilePath& filePath ) const
            {
                return boost::filesystem::createBinaryInputFileStream( toPath( filePath ) );
            }
            virtual std::unique_ptr< boost::filesystem::ofstream > write_temp( const CompilationFilePath& filePath,
                                                                               boost::filesystem::path&   tempFilePath ) const
            {
                tempFilePath = m_tempDir / filePath.path();
                return boost::filesystem::createBinaryOutputFileStream( tempFilePath );
            }
            virtual void temp_to_real( const CompilationFilePath& filePath ) const
            {
                copyToTargetPath( m_tempDir / filePath.path(), toPath( filePath ) );
            }

            virtual std::unique_ptr< boost::filesystem::ifstream > read( const SourceFilePath& filePath ) const
            {
                return boost::filesystem::createBinaryInputFileStream( toPath( filePath ) );
            }
            virtual std::unique_ptr< boost::filesystem::ofstream > write_temp( const SourceFilePath&  filePath,
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
        /*
                class RetailEnvironment : public Environment
                {
                    task::Stash& m_stash;
                public:
                    RetailEnvironment( const Path& rootSourceDir, const Path& rootBuildDir, const Path& tempDir, task::Stash& stash )
                        :   Environment( rootSourceDir, rootBuildDir, tempDir ),
                            m_stash( stash )
                    {
                    }

                    // stash
                    virtual common::HashCode getBuildHashCode( const boost::filesystem::path& key ) const
                    {
                        return m_stash.getBuildHashCode( key );
                    }
                    virtual void setBuildHashCode( const boost::filesystem::path& key, common::HashCode hashCode )
                    {
                        m_stash.setBuildHashCode(key, hashCode);
                    }
                    virtual void loadBuildHashCodes( const boost::filesystem::path& file )
                    {
                        m_stash.loadBuildHashCodes( file );
                    }
                    virtual void saveBuildHashCodes( const boost::filesystem::path& file ) const
                    {
                        m_stash.saveBuildHashCodes( file );
                    }

                    // file io
                    virtual std::unique_ptr< boost::filesystem::ifstream > read( const CompilationFilePath& filePath ) const
                    {
                        return boost::filesystem::createBinaryInputFileStream( m_rootBuildDir / filePath.path() );
                    }
                    virtual std::unique_ptr< boost::filesystem::ofstream > write_temp( const CompilationFilePath& filePath,
           boost::filesystem::path& tempFilePath ) const
                    {
                        tempFilePath = m_tempDir / filePath.path();
                        return boost::filesystem::createBinaryOutputFileStream( tempFilePath );
                    }
                    virtual void temp_to_real( const CompilationFilePath& filePath ) const
                    {
                        boost::filesystem::copy_file( m_tempDir / filePath.path(), m_rootBuildDir / filePath.path() );
                    }

                    virtual std::unique_ptr< boost::filesystem::ifstream > read( const SourceFilePath& filePath ) const
                    {
                        return boost::filesystem::createBinaryInputFileStream( m_rootSourceDir / filePath.path() );
                    }
                    virtual std::unique_ptr< boost::filesystem::ofstream > write_temp( const SourceFilePath& filePath,
           boost::filesystem::path& tempFilePath ) const
                    {
                        tempFilePath = m_tempDir / filePath.path();
                        return boost::filesystem::createBinaryOutputFileStream( tempFilePath );
                    }
                    virtual void temp_to_real( const SourceFilePath& filePath ) const
                    {
                        boost::filesystem::copy_file( m_tempDir / filePath.path(), m_rootSourceDir / filePath.path() );
                    }
                };*/
    } // namespace io
} // namespace mega

#endif // ENVIRONMENTS_22_APRIL_2022