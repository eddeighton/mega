
#ifndef COMPILER_CONFIGURATION_27_MAY_2022
#define COMPILER_CONFIGURATION_27_MAY_2022

#include "pipeline/configuration.hpp"
#include "pipeline/pipeline.hpp"

#include "utilities/tool_chain_hash.hpp"

#include "common/hash.hpp"
#include "common/stash.hpp"

#include "boost/filesystem/path.hpp"
#include "boost/serialization/nvp.hpp"
#include "boost/serialization/split_member.hpp"

#include <vector>
#include <string>

namespace mega
{
namespace compiler
{

struct Directories
{
    boost::filesystem::path rootSourceDir, rootBuildDir, templatesDir;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "rootSourceDir", rootSourceDir );
        archive& boost::serialization::make_nvp( "rootBuildDir", rootBuildDir );
        archive& boost::serialization::make_nvp( "templatesDir", templatesDir );
    }
};

struct Configuration
{
    pipeline::ConfigurationHeader          header;
    std::string                            projectName;
    std::vector< boost::filesystem::path > componentInfoPaths;
    Directories                            directories;
    utilities::ToolChain                   toolChain;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        //NOTE: header serialization handled seperately so can access in pipeline abstraction
        //archive& boost::serialization::make_nvp( "header", header );
        archive& boost::serialization::make_nvp( "projectName", projectName );
        archive& boost::serialization::make_nvp( "componentInfoPaths", componentInfoPaths );
        archive& boost::serialization::make_nvp( "directories", directories );
        archive& boost::serialization::make_nvp( "toolChain", toolChain );
    }
};

pipeline::Configuration makePipelineConfiguration( const Configuration& config );
Configuration           fromPipelineConfiguration( const pipeline::Configuration& pipelineConfig );

} // namespace compiler
} // namespace mega

#endif // COMPILER_CONFIGURATION_27_MAY_2022
