
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

#ifndef GUARD_2023_November_28_program
#define GUARD_2023_November_28_program

#include "mega/values/native_types.hpp"
#include "mega/values/service/project.hpp"

#include "common/serialisation.hpp"
#include "common/assert_verify.hpp"

#include <optional>
#include <limits>

namespace mega::service
{

class Program
{
public:
    class Version
    {
    public:
        using Value = U32;

        inline static constexpr Value MAX = std::numeric_limits< Value >::max();

        inline Version()                 = default;
        inline Version( const Version& ) = default;
        inline Version( Value value )
            : m_value( value )
        {
        }

        inline Value   value() const { return m_value; }
        inline Version next() const { return { m_value + 1 }; }

        inline bool operator==( const Version& ) const = default;
        inline bool operator!=( const Version& ) const = default;
        inline bool operator<( const Version& cmp ) const { return m_value < cmp.m_value; }

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
            {
                archive& boost::serialization::make_nvp( "value", m_value );
            }
            else
            {
                archive& m_value;
            }
        }

    private:
        Value m_value = 0;
    };

    static std::optional< Program > parse( const std::string& str );
    static std::optional< Program > latest( const Project& project, const boost::filesystem::path& workBin );
    static Program                  next( const Project& project, const boost::filesystem::path& workBin );

    inline Program() = default;
    inline Program( Project project )
        : m_project( std::move( project ) )
    {
    }
    inline Program( Project project, Version version )
        : m_project( std::move( project ) )
        , m_version( version )
    {
    }

    inline bool operator==( const Program& ) const = default;
    inline bool operator<( const Program& cmp ) const
    {
        return ( m_project != cmp.project() ) ? ( m_project < cmp.project() ) : ( m_version < cmp.version() );
    }

    const Project& project() const { return m_project; }
    const Version& version() const { return m_version; }

    inline Program next() const { return { m_project, m_version.next() }; }

    inline bool empty() const { return m_project.empty(); }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "project", m_project );
            archive& boost::serialization::make_nvp( "version", m_version );
        }
        else
        {
            archive& m_project;
            archive& m_version;
        }
    }

private:
    Project m_project;
    Version m_version;
};

std::ostream& operator<<( std::ostream& os, const Program& program );

} // namespace mega::service

#endif // GUARD_2023_November_28_program
