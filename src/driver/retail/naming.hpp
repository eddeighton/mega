
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

#ifndef GUARD_2023_July_31_naming
#define GUARD_2023_July_31_naming

#include "database/model/FinalStage.hxx"

#include <map>
#include <string>

namespace driver::retail
{
    using namespace FinalStage;

    class DataNaming
    {
        using PartNames      = std::map< const MemoryLayout::Part*, std::string >;
        using ObjectNames    = std::map< const Concrete::Object*, std::string >;
        using AllocatorNames = std::map< const Concrete::Dimensions::Allocation*, std::string >;
        using LinkNames      = std::map< const Concrete::Dimensions::LinkReference*, std::string >;
        using UserNames      = std::map< const Concrete::Dimensions::User*, std::string >;

        template < typename T >
        inline const std::string& generate( const char prefix, std::map< const T*, std::string >& names, const T* p )
        {
            auto iFind = names.find( p );
            if( iFind == names.end() )
            {
                std::ostringstream os;
                os << prefix << '_' << names.size();
                auto ib = names.insert( { p, os.str() } );
                VERIFY_RTE( ib.second );
                return ib.first->second;
            }
            else
            {
                return iFind->second;
            }
        }

    public:
        const std::string& partName( const MemoryLayout::Part* p ) { return generate( 'p', m_partNames, p ); }

        const std::string& objectName( Concrete::Object* p ) { return generate( 'o', m_objectNames, p ); }

        const std::string& allocatorName( Concrete::Dimensions::Allocation* p )
        {
            return generate( 'a', m_allocatorNames, p );
        }
        const std::string& linkName( Concrete::Dimensions::LinkReference* p )
        {
            return generate( 'l', m_linkNames, p );
        }
        const std::string& userName( Concrete::Dimensions::User* p ) { return generate( 'u', m_userNames, p ); }

    private:
        PartNames      m_partNames;
        ObjectNames    m_objectNames;
        AllocatorNames m_allocatorNames;
        LinkNames      m_linkNames;
        UserNames      m_userNames;
    };
}

#endif //GUARD_2023_July_31_naming
