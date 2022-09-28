
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

#ifndef FILE_HEADER_12_MAY_2022
#define FILE_HEADER_12_MAY_2022

#include <cstddef>

namespace mega
{
namespace io
{

class FileHeader
{
public:
    FileHeader()
        : m_versionHashCode( 0U )
        , m_manifestHashCode( 0U )
    {
    }

    FileHeader( mega::U64 versionHashCode, mega::U64 manifestHashCode )
        : m_versionHashCode( versionHashCode )
        , m_manifestHashCode( manifestHashCode )
    {
    }

    mega::U64 getVersion() const { return m_versionHashCode; }
    mega::U64 getManifestHashCode() const { return m_manifestHashCode; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_versionHashCode;
        archive& m_manifestHashCode;
    }

private:
    mega::U64 m_versionHashCode;
    mega::U64 m_manifestHashCode;
};

} // namespace io
} // namespace mega

#endif // FILE_HEADER_12_MAY_2022