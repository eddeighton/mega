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

#ifndef OWNERSHIP_13_MAY_2022
#define OWNERSHIP_13_MAY_2022

#include <ostream>

namespace mega
{
class Ownership
{
public:
    enum Value
    {
        eOwnNothing,
        eOwnSource,
        eOwnTarget,
        TOTAL_OWNERSHIP_MODES
    };

    Ownership()
        : m_value( TOTAL_OWNERSHIP_MODES )
    {
    }
    explicit Ownership( Value value )
        : m_value( value )
    {
    }

    const char*      str() const;
    static Ownership fromStr( const char* psz );

    Value get() const { return m_value; }
    void  set( Value value ) { m_value = value; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        archive& m_value;
    }

private:
    Value m_value;
};

std::ostream& operator<<( std::ostream& os, Ownership ownership );

} // namespace mega

#endif // MODES_10_MAY_2022
