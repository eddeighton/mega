#ifndef ENVIRONMENT_STASH_18_JUNE_2022
#define ENVIRONMENT_STASH_18_JUNE_2022

#include "pipeline/stash.hpp"

#include "environment_build.hpp"

namespace mega
{
namespace io
{

class StashEnvironment : public BuildEnvironment
{
    mega::pipeline::Stash& m_stash;

public:
    StashEnvironment( mega::pipeline::Stash& stash, const compiler::Directories& directories );

    template < typename TFilePathType >
    task::FileHash getBuildHashCode( const TFilePathType& filePath ) const
    {
        return m_stash.getBuildHashCode( toPath( filePath ) );
    }

    template < typename TFilePathType >
    void setBuildHashCode( const TFilePathType& filePath, task::FileHash hashCode ) const
    {
        m_stash.setBuildHashCode( toPath( filePath ), hashCode );
    }

    template < typename TFilePathType >
    void setBuildHashCode( const TFilePathType& filePath ) const
    {
        m_stash.setBuildHashCode( toPath( filePath ), task::FileHash( toPath( filePath ) ) );
    }

    template < typename TFilePathType >
    void stash( const TFilePathType& filePath, task::DeterminantHash hashCode ) const
    {
        m_stash.stash( toPath( filePath ), hashCode );
    }

    template < typename TFilePathType >
    bool restore( const TFilePathType& filePath, task::DeterminantHash hashCode ) const
    {
        return m_stash.restore( toPath( filePath ), hashCode );
    }

    bool restore( const CompilationFilePath& filePath, task::DeterminantHash hashCode ) const;
};

} // namespace io
} // namespace mega

#endif // ENVIRONMENT_STASH_18_JUNE_2022
