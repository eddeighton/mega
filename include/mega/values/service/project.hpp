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

#ifndef PROJECT_20_JUNE_2022
#define PROJECT_20_JUNE_2022

#include "common/serialisation.hpp"

#include <string>
#include <ostream>

namespace mega::service
{

class Project
{
public:
    inline Project() = default;
    inline Project( std::string strName )
        : m_name( std::move( strName ) )
    {
    }

    inline const bool         empty() const { return m_name.empty(); }
    inline const std::string& str() const { return m_name; }

    inline bool operator<( const Project& cmp ) const { return m_name < cmp.m_name; }
    inline bool operator!=( const Project& cmp ) const { return !this->operator==( cmp ); }
    inline bool operator==( const Project& cmp ) const { return m_name == cmp.m_name; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "name", m_name );
        }
        else
        {
            archive& m_name;
        }
    }

private:
    std::string m_name;
};

std::ostream& operator<<( std::ostream& os, const Project& project );

} // namespace mega::service

#endif // PROJECT_20_JUNE_2022
