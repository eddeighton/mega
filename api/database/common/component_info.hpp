#ifndef IO_SOURCETREE_25_MARCH_2022
#define IO_SOURCETREE_25_MARCH_2022

#include "serialisation.hpp"

#include "database/types/component_type.hpp"

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
    ComponentInfo( ComponentType                     componentType,
                   const std::string&                strName,
                   const std::vector< std::string >& cppFlags,
                   const std::vector< std::string >& cppDefines,
                   const boost::filesystem::path&    srcDir,
                   const boost::filesystem::path&    buildDir,
                   const PathArray&                  sourceFiles,
                   const PathArray&                  dependencyFiles,
                   const PathArray&                  includeDirectories );

    ComponentType                     getComponentType() const { return m_componentType; }
    const std::string&                getName() const { return m_strName; }
    const std::vector< std::string >& getCPPFlags() const { return m_cppFlags; }
    const std::vector< std::string >& getCPPDefines() const { return m_cppDefines; }
    const boost::filesystem::path&    getSrcDir() const { return m_srcDir; }
    const boost::filesystem::path&    getBuildDir() const { return m_buildDir; }
    const PathArray&                  getSourceFiles() const { return m_sourceFiles; }
    const PathArray&                  getDependencyFiles() const { return m_dependencyFiles; }
    const PathArray&                  getIncludeDirectories() const { return m_includeDirectories; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "componentType", m_componentType );
        archive& boost::serialization::make_nvp( "strName", m_strName );
        archive& boost::serialization::make_nvp( "flags", m_cppFlags );
        archive& boost::serialization::make_nvp( "defines", m_cppDefines );
        archive& boost::serialization::make_nvp( "srcDir", m_srcDir );
        archive& boost::serialization::make_nvp( "buildDir", m_buildDir );
        archive& boost::serialization::make_nvp( "sourceFiles", m_sourceFiles );
        archive& boost::serialization::make_nvp( "dependencyFiles", m_dependencyFiles );
        archive& boost::serialization::make_nvp( "includeDirectories", m_includeDirectories );
    }

private:
    ComponentType              m_componentType;
    std::string                m_strName;
    std::vector< std::string > m_cppFlags;
    std::vector< std::string > m_cppDefines;
    boost::filesystem::path    m_srcDir;
    boost::filesystem::path    m_buildDir;
    PathArray                  m_sourceFiles;
    PathArray                  m_dependencyFiles;
    PathArray                  m_includeDirectories;
};

} // namespace io
} // namespace mega

#endif // IO_SOURCETREE_25_MARCH_2022
