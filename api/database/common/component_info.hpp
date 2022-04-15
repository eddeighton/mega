#ifndef IO_SOURCETREE_25_MARCH_2022
#define IO_SOURCETREE_25_MARCH_2022

#include "archive.hpp"

#include <boost/filesystem/path.hpp>

#include <vector>

namespace mega
{
namespace io
{
    class ComponentInfo
    {
    public:
        using PathArray = std::vector< boost::filesystem::path >;
        ComponentInfo();
        ComponentInfo( const std::string&             strName,
                       const boost::filesystem::path& directory,
                       const PathArray&               sourceFiles,
                       const PathArray&               includeDirectories );

        const std::string&             getName() const { return m_strName; }
        const boost::filesystem::path& getDirectory() const { return m_directory; }
        const PathArray&               getSourceFiles() const { return m_sourceFiles; }
        const PathArray&               getIncludeDirectories() const { return m_includeDirectories; }

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive & boost::serialization::make_nvp( "strName", m_strName );
            archive & boost::serialization::make_nvp( "directory", m_directory );
            archive & boost::serialization::make_nvp( "sourceFiles", m_sourceFiles );
            archive & boost::serialization::make_nvp( "includeDirectories", m_includeDirectories );
        }

    private:
        std::string             m_strName;
        boost::filesystem::path m_directory;
        PathArray               m_sourceFiles;
        PathArray               m_includeDirectories;
    };

} // namespace io
} // namespace mega

#endif // IO_SOURCETREE_25_MARCH_2022
