

#ifndef STORER_18_04_2019
#define STORER_18_04_2019

#include "serialisation.hpp"
#include "object.hpp"
#include "data_pointer.hpp"
#include "file_system.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>

#include <ostream>
#include <memory>
#include <optional>
#include <set>
#include <map>
#include <vector>

namespace mega
{
namespace io
{
class Storer
{
public:
    Storer( const FileSystem& fileSystem, const CompilationFilePath& filePath, boost::filesystem::path& tempFile );

    template < class T >
    inline void store( const T& value )
    {
        m_archive& value;
    }

    std::set< const mega::io::ObjectInfo* >& getObjectInfos() { return m_objectInfos; }

private:
    std::set< const mega::io::ObjectInfo* > m_objectInfos;
    std::unique_ptr< std::ostream >         m_pFileStream;
    boost::archive::MegaOArchive            m_archive;
};

} // namespace io
} // namespace mega

#endif // STORER_18_04_2019