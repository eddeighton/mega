

#ifndef CONFIGURATION_30_MAY_2022
#define CONFIGURATION_30_MAY_2022

#include "boost/serialization/nvp.hpp"

#include <vector>
#include <string>

namespace mega
{
namespace pipeline
{

using PipelineID = std::string;

struct ConfigurationHeader
{
    PipelineID pipelineID;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "id", pipelineID );
    }
};

class Configuration
{
public:
    using Buffer = std::string;

    Configuration();
    Configuration( const Buffer& buffer );

    inline bool operator<( const Configuration& cmp ) const { return m_buffer < cmp.m_buffer; }
    inline bool operator==( const Configuration& cmp ) const { return m_buffer == cmp.m_buffer; }
    inline bool operator!=( const Configuration& cmp ) const { return m_buffer != cmp.m_buffer; }

    PipelineID    getPipelineID() const;
    const Buffer& get() const { return m_buffer; }
    Buffer&       data() { return m_buffer; }

    // serialisation used by network - NOT when loading from xml file
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_buffer;
    }
private:
    Buffer m_buffer;
};

} // namespace pipeline
} // namespace mega

#endif // CONFIGURATION_30_MAY_2022
