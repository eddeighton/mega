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

#ifndef MEMORY_CONFIG_29_SEPT_2022
#define MEMORY_CONFIG_29_SEPT_2022

#include "project.hpp"

#include "boost/serialization/nvp.hpp"

namespace mega
{
namespace network
{

class MemoryConfig
{
public:
    const std::string& getMemory() const { return m_addressSpaceMemory; }
    void               setMemory( const std::string& memory ) { m_addressSpaceMemory = memory; }
    const std::string& getMutex() const { return m_addressSpaceMutex; }
    void               setMutex( const std::string& memory ) { m_addressSpaceMutex = memory; }
    const std::string& getMap() const { return m_addressSpaceMap; }
    void               setMap( const std::string& memory ) { m_addressSpaceMap = memory; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "memory", m_addressSpaceMemory );
        archive& boost::serialization::make_nvp( "mutex", m_addressSpaceMutex );
        archive& boost::serialization::make_nvp( "map", m_addressSpaceMap );
    }

private:
    std::string m_addressSpaceMemory;
    std::string m_addressSpaceMutex;
    std::string m_addressSpaceMap;
};

} // namespace network
} // namespace mega

#endif // MEMORY_CONFIG_29_SEPT_2022
