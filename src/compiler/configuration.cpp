


#include "compiler/configuration.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

namespace mega
{
namespace compiler
{

pipeline::Configuration makePipelineConfiguration( const Configuration& configuration )
{
    std::ostringstream os;
    {
        boost::archive::xml_oarchive oa( os );

        pipeline::ConfigurationHeader header = configuration.header;
        oa&                           boost::serialization::make_nvp( "pipeline_header", header );
        Configuration temp = configuration;
        oa&                           boost::serialization::make_nvp( "pipeline_config", temp );
    }
    return pipeline::Configuration( os.str() );
}

Configuration fromPipelineConfiguration( const pipeline::Configuration& pipelineConfig )
{
    Configuration configuration;
    {
        std::istringstream           is( pipelineConfig.get() );
        boost::archive::xml_iarchive ia( is );

        pipeline::ConfigurationHeader header;
        ia&                           boost::serialization::make_nvp( "pipeline_header", header );
        ia&                           boost::serialization::make_nvp( "pipeline_config", configuration );
        configuration.header = header;
    }
    return configuration;
}

} // namespace compiler
} // namespace mega
