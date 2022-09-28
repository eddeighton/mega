



#ifndef STASH_20_MAY_2022
#define STASH_20_MAY_2022

#include "common/stash.hpp"

namespace mega
{
namespace pipeline
{
class Stash
{
public:
    virtual ~Stash();

    virtual task::FileHash getBuildHashCode( const boost::filesystem::path& filePath )                          = 0;
    virtual void           setBuildHashCode( const boost::filesystem::path& filePath, task::FileHash hashCode ) = 0;
    virtual void           stash( const boost::filesystem::path& file, task::DeterminantHash code )             = 0;
    virtual bool           restore( const boost::filesystem::path& file, task::DeterminantHash code )           = 0;
};
} // namespace pipeline
} // namespace mega

#endif // STASH_20_MAY_2022
