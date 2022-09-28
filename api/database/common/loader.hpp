

#ifndef LOADER_18_04_2019
#define LOADER_18_04_2019

#include "serialisation.hpp"
#include "object_info.hpp"
#include "object.hpp"
#include "file_system.hpp"

#include "database/types/sources.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>

#include <istream>
#include <memory>
#include <optional>
#include <set>
#include <map>
#include <vector>

namespace data
{
class ObjectPartLoader;
}
namespace mega
{
namespace io
{
class Manifest;

class Loader
{
public:
    Loader( const FileSystem& fileSystem, const CompilationFilePath& filePath, ::data::ObjectPartLoader& loader );

    template < typename T >
    void load( T& value )
    {
        m_archive& value;
    }

    void postLoad( const Manifest& runtimeManifest );

private:
    std::set< mega::io::ObjectInfo* > m_objectInfos;
    std::unique_ptr< std::istream >   m_pFileStream;
    boost::archive::MegaIArchive      m_archive;
};

} // namespace io
} // namespace mega

#endif // LOADER_18_04_2019
