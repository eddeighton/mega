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