
#ifndef SOURCES_15_APRIL_2022
#define SOURCES_15_APRIL_2022

#include "database/common/serialisation.hpp"

#include "boost/filesystem/path.hpp"

namespace mega
{
    namespace io
    {
        class Environment;
        class Manifest;

        class FilePath
        {
        protected:
            FilePath( const boost::filesystem::path& filePath )
                : m_filePath( filePath )
            {
            }

        public:
            FilePath() {}
            const boost::filesystem::path& path() const { return m_filePath; }

            bool operator==( const FilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
            bool operator<( const FilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "file_path", m_filePath );
            }

        protected:
            boost::filesystem::path m_filePath;
        };

        class SourceFilePath : public FilePath
        {
            friend class BuildEnvironment;
        protected:
            SourceFilePath( const boost::filesystem::path& filePath )
                : FilePath( filePath )
            {
            }

        public:
            SourceFilePath() {}
            bool operator==( const SourceFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
            bool operator<( const SourceFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "source_file_path", m_filePath );
            }
        };

        class manifestFilePath : public SourceFilePath
        {
            static boost::filesystem::path EXTENSION;
            
            friend class Environment;
            friend class BuildEnvironment;
            manifestFilePath( const boost::filesystem::path& filePath )
                : SourceFilePath( filePath )
            {
            }

        public:
            manifestFilePath() {}
            bool operator==( const manifestFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
            bool operator<( const manifestFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "manifest_file_path", m_filePath );
            }

            static const boost::filesystem::path& extension() { return EXTENSION; }
        };

        class megaFilePath : public SourceFilePath
        {
            static boost::filesystem::path EXTENSION;
            
            friend class BuildEnvironment;
            megaFilePath( const boost::filesystem::path& filePath )
                : SourceFilePath( filePath )
            {
            }

        public:
            megaFilePath() {}
            
            bool operator==( const megaFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
            bool operator<( const megaFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "mega_file_path", m_filePath );
            }
            static const boost::filesystem::path& extension() { return EXTENSION; }
        };

        class BuildFilePath : public FilePath
        {
            friend class Environment;
            
        protected:
            BuildFilePath( const boost::filesystem::path& filePath )
                : FilePath( filePath )
            {
            }

        public:
            BuildFilePath() {}
            bool operator==( const BuildFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
            bool operator<( const BuildFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "build_file_path", m_filePath );
            }
        };

        class ComponentListingFilePath : public BuildFilePath
        {
            static boost::filesystem::path EXTENSION;
            friend class BuildEnvironment;
            
            ComponentListingFilePath( const boost::filesystem::path& filePath )
                : BuildFilePath( filePath )
            {
            }

        public:
            ComponentListingFilePath() {}

            bool operator==( const ComponentListingFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
            bool operator<( const ComponentListingFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "component_listing_file_path", m_filePath );
            }
            static const boost::filesystem::path& extension() { return EXTENSION; }
        };

        class CompilationFilePath : public BuildFilePath
        {
            static boost::filesystem::path EXTENSION;
            friend class Environment;
            
            CompilationFilePath( const boost::filesystem::path& filePath )
                : BuildFilePath( filePath )
            {
            }

        public:
            CompilationFilePath() {}

            bool operator==( const CompilationFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
            bool operator<( const CompilationFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "compilation_file_path", m_filePath );
            }
            static const boost::filesystem::path& extension() { return EXTENSION; }
        };

        class GeneratedHPPSourceFilePath : public BuildFilePath
        {
            static boost::filesystem::path EXTENSION;
            friend class BuildEnvironment;
            
            GeneratedHPPSourceFilePath( const boost::filesystem::path& filePath )
                : BuildFilePath( filePath )
            {
            }

        public:
            GeneratedHPPSourceFilePath() {}

            bool operator==( const GeneratedHPPSourceFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
            bool operator<( const GeneratedHPPSourceFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "generated_hxx_file_path", m_filePath );
            }
            static const boost::filesystem::path& extension() { return EXTENSION; }
        };

        class GeneratedCPPSourceFilePath : public BuildFilePath
        {
            static boost::filesystem::path EXTENSION;
            friend class BuildEnvironment;
            
            GeneratedCPPSourceFilePath( const boost::filesystem::path& filePath )
                : BuildFilePath( filePath )
            {
            }

        public:
            GeneratedCPPSourceFilePath() {}

            bool operator==( const GeneratedCPPSourceFilePath& cmp ) const { return m_filePath == cmp.m_filePath; }
            bool operator<( const GeneratedCPPSourceFilePath& cmp ) const { return m_filePath < cmp.m_filePath; }

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "generated_cxx_file_path", m_filePath );
            }
            static const boost::filesystem::path& extension() { return EXTENSION; }
        };

        class PrecompiledHeaderFile : public BuildFilePath
        {
            static boost::filesystem::path EXTENSION;
            friend class BuildEnvironment;
            
            PrecompiledHeaderFile( const boost::filesystem::path& filePath )
                : BuildFilePath( filePath )
            {
            }

        public:
            PrecompiledHeaderFile() {}

            bool operator==( const PrecompiledHeaderFile& cmp ) const { return m_filePath == cmp.m_filePath; }
            bool operator<( const PrecompiledHeaderFile& cmp ) const { return m_filePath < cmp.m_filePath; }

            template < class Archive >
            inline void serialize( Archive& archive, const unsigned int version )
            {
                archive& boost::serialization::make_nvp( "precompiled_header_file", m_filePath );
            }
            static const boost::filesystem::path& extension() { return EXTENSION; }
        };


    } // namespace io
} // namespace mega

namespace mega
{
    namespace io
    {
        inline void to_json( nlohmann::json& j, const megaFilePath& p )
        {
            j = nlohmann::json{ { "megaFilePath", p.path().string() } };
        }
    }
}

#endif // SOURCES_15_APRIL_2022
