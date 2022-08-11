
#include "database/common/environment_stash.hpp"

#include "database/common/file_header.hpp"

namespace mega
{
namespace io
{

StashEnvironment::StashEnvironment( mega::pipeline::Stash& stash, const compiler::Directories& directories )
    : BuildEnvironment( directories )
    , m_stash( stash )
{
}

bool StashEnvironment::restore( const CompilationFilePath& filePath, task::DeterminantHash hashCode ) const
{
    const Path actualPath = toPath( filePath );
    if ( m_stash.restore( actualPath, hashCode ) )
    {
        // check the file header for the correct version
        data::NullObjectPartLoader nullObjectPartLoader;

        std::unique_ptr< std::istream >   pFileStream = read( filePath );
        std::set< mega::io::ObjectInfo* > objectInfos;
        boost::archive::MegaIArchive      archive( *pFileStream, objectInfos, nullObjectPartLoader );

        mega::io::FileHeader fileHeader;
        archive >> fileHeader;
        if ( fileHeader.getVersion() == getVersion() )
        {
            return true;
        }
    }
    return false;
}

} // namespace io
} // namespace mega
