
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

#ifndef GUARD_2023_October_28_transition_id
#define GUARD_2023_October_28_transition_id

#include "mega/values/native_types.hpp"

#ifndef MEGAJIT

#include "common/serialisation.hpp"
#include <limits>
#include <ostream>
#endif

namespace mega
{

class TransitionID
{
public:

    struct Hash
    {
        inline U64 operator()( const TransitionID& ref ) const noexcept { return 0; }
    };

#ifndef MEGAJIT
    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "transitionID", m_value );
        }
        else
        {
            archive& m_value;
        }
    }
#endif

private:
    U64 m_value;
};

#ifndef MEGAJIT
std::ostream& operator<<( std::ostream& os, const mega::TransitionID& transitionID );
std::istream& operator>>( std::istream& is, mega::TransitionID& transitionID );
#endif

}

#endif //GUARD_2023_October_28_transition_id
