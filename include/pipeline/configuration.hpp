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

#ifndef CONFIGURATION_30_MAY_2022
#define CONFIGURATION_30_MAY_2022

#include "version.hpp"

#include <vector>
#include <string>

namespace mega::pipeline
{

using PipelineID = std::string;

struct ConfigurationHeader
{
    PipelineID pipelineID;
    Version    version;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        archive& boost::serialization::make_nvp( "id", pipelineID );
        archive& boost::serialization::make_nvp( "version", version );
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

    [[nodiscard]] PipelineID getPipelineID() const;
    [[nodiscard]] Version    getVersion() const;

    [[nodiscard]] const Buffer& get() const { return m_buffer; }
    Buffer&                     data() { return m_buffer; }

    // serialisation used by network - NOT when loading from xml file
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        archive& m_buffer;
    }

private:
    Buffer m_buffer;
};

} // namespace mega::pipeline

#endif // CONFIGURATION_30_MAY_2022
