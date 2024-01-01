
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

#ifndef GUARD_2023_October_03_operator_id
#define GUARD_2023_October_03_operator_id

#include "mega/values/compilation/interface/type_id.hpp"

#include "common/serialisation.hpp"

#include <ostream>
#include <array>
#include <string>
#include <utility>

namespace mega
{

class OperatorID
{
public:
    enum Type
    {
        op_new,
        op_remote_new,
        op_delete,
        op_cast,
        op_active,
        op_enabled,
        HIGHEST_OPERATOR_TYPE
    };

    Type              m_operator = HIGHEST_OPERATOR_TYPE;
    interface::TypeID m_typeID;

    inline bool operator==( const OperatorID& cmp ) const
    {
        return ( m_operator == cmp.m_operator ) && ( m_typeID == cmp.m_typeID );
    }
    inline bool operator!=( const OperatorID& cmp ) const { return !this->operator==( cmp ); }
    inline bool operator<( const OperatorID& cmp ) const
    {
        return ( m_operator != cmp.m_operator ) ? ( m_operator < cmp.m_operator ) : ( m_typeID < cmp.m_typeID );
    }

    using OperatorIDTypeName = std::pair< Type, std::string >;  
    using Array              = std::array< OperatorIDTypeName, HIGHEST_OPERATOR_TYPE >;
    static const Array names;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "operator", m_operator );
            archive& boost::serialization::make_nvp( "typeID", m_typeID );
        }
        else
        {
            archive& m_operator;
            archive& m_typeID;
        }
    }
};

} // namespace mega

std::ostream& operator<<( std::ostream& os, const mega::OperatorID& operatorID );

#endif // GUARD_2023_October_03_operator_id
