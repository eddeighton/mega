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




#ifndef DERIVATION_29_AUG_2022
#define DERIVATION_29_AUG_2022

#include "database/common/api.hpp"
#include "database/common/serialisation.hpp"

#include <ostream>

namespace mega
{
class EGDB_EXPORT DerivationDirection
{
public:
    enum Value
    {
        eDeriveNone,
        eDeriveSource,
        eDeriveTarget,
        TOTAL_DERIVATION_MODES
    };

    DerivationDirection()
        : m_value( TOTAL_DERIVATION_MODES )
    {
    }
    DerivationDirection( Value value )
        : m_value( value )
    {
    }

    const char*                str() const;
    static DerivationDirection fromStr( const char* psz );

    Value get() const { return m_value; }
    void  set( Value value ) { m_value = value; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_value;
    }

private:
    Value m_value;
};

} // namespace mega

EGDB_EXPORT std::ostream& operator<<( std::ostream& os, mega::DerivationDirection derivation );

#endif // DERIVATION_29_AUG_2022
