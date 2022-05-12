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

    FileHeader( std::size_t versionHashCode, std::size_t manifestHashCode )
        : m_versionHashCode( versionHashCode )
        , m_manifestHashCode( manifestHashCode )
    {
    }

    std::size_t getVersion() const { return m_versionHashCode; }
    std::size_t getManifestHashCode() const { return m_manifestHashCode; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_versionHashCode;
        archive& m_manifestHashCode;
    }

private:
    std::size_t m_versionHashCode;
    std::size_t m_manifestHashCode;
};

} // namespace io
} // namespace mega

#endif // FILE_HEADER_12_MAY_2022