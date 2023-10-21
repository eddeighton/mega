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

#ifndef ENVIRONMENT_STASH_18_JUNE_2022
#define ENVIRONMENT_STASH_18_JUNE_2022

#include "database/api.hpp"
#include "pipeline/stash.hpp"

#include "environment_build.hpp"

namespace mega::io
{
class EGDB_EXPORT StashEnvironment : public BuildEnvironment
{
    mega::pipeline::Stash& m_stash;

public:
    StashEnvironment( mega::pipeline::Stash& stash, const Directories& directories );

    template < typename TFilePathType >
    task::FileHash getBuildHashCode( const TFilePathType& filePath ) const
    {
        return m_stash.getBuildHashCode( toPath( filePath ) );
    }

    task::FileHash getBuildHashCodePath( const boost::filesystem::path& filePath ) const
    {
        return m_stash.getBuildHashCode( filePath );
    }

    template < typename TFilePathType >
    void setBuildHashCode( const TFilePathType& filePath, task::FileHash hashCode ) const
    {
        m_stash.setBuildHashCode( toPath( filePath ), hashCode );
    }

    inline void setBuildHashCodePath( const boost::filesystem::path& filePath ) const
    {
        m_stash.setBuildHashCode( filePath, task::FileHash( filePath ) );
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

    inline void stashPath( const boost::filesystem::path& filePath, task::DeterminantHash hashCode ) const
    {
        m_stash.stash( filePath, hashCode );
    }

    template < typename TFilePathType >
    bool restore( const TFilePathType& filePath, task::DeterminantHash hashCode ) const
    {
        return m_stash.restore( toPath( filePath ), hashCode );
    }

    inline bool restorePath( const boost::filesystem::path& filePath, task::DeterminantHash hashCode ) const
    {
        return m_stash.restore( filePath, hashCode );
    }

    bool restore( const CompilationFilePath& filePath, task::DeterminantHash hashCode ) const;

};

} // namespace mega::io

#endif // ENVIRONMENT_STASH_18_JUNE_2022
