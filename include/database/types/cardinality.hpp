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




#ifndef CARDINALITY_13_MAY_2022
#define CARDINALITY_13_MAY_2022

#include "database/common/api.hpp"
#include "database/common/serialisation.hpp"

#include <optional>
#include <ostream>

namespace mega
{

struct EGDB_EXPORT Cardinality
{
    using NumberType = int;

    enum : NumberType
    {
        MANY = -1,
        ZERO = 0,
        ONE  = 1
    };

    Cardinality()
        : m_number( ONE )
    {
    }

    Cardinality( NumberType number )
        : m_number( number )
    {
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_number;
    }

    bool       isZero() const { return m_number == ZERO; }
    bool       isOne() const { return m_number == ONE; }
    bool       isMany() const { return m_number == MANY; }
    bool       isNumber() const { return !isZero() && !isOne() && !isMany(); }
    NumberType getNumber() const { return m_number; }

    void setZero() { m_number = ZERO; }
    void setOne() { m_number = ONE; }
    void setMany() { m_number = MANY; }
    void setNumber( NumberType number ) { m_number = number; }

private:
    NumberType m_number;
};

struct EGDB_EXPORT CardinalityRange
{
    CardinalityRange() {}

    CardinalityRange( const Cardinality& minimum, const Cardinality& maximum )
        : m_minimum( minimum )
        , m_maximum( maximum )
    {
    }

    const Cardinality& minimum() const { return m_minimum; }
    const Cardinality& maximum() const { return m_maximum; }

    inline bool isOptional() const
    {
        return m_minimum.isZero();
    }
    inline bool isMandatory() const
    {
        return !isOptional();
    }
    inline bool isSingular() const
    {
        return m_maximum.isOne();
    }
    inline bool isNonSingular() const
    {
        return !isSingular();
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_minimum;
        archive& m_maximum;
    }

private:
    Cardinality m_minimum, m_maximum;
};

} // namespace mega

EGDB_EXPORT std::ostream& operator<<( std::ostream& os, const mega::CardinalityRange& cardinalityRange );

namespace mega
{
inline void to_json( nlohmann::json& j, const mega::Cardinality& cardinality )
{
    j = nlohmann::json{ { "number", cardinality.getNumber() } };
}
inline void to_json( nlohmann::json& j, const mega::CardinalityRange& cardinalityRange )
{
    j = nlohmann::json{ { "minimum", cardinalityRange.minimum() }, { "maximum", cardinalityRange.maximum() } };
}
} // namespace mega

#endif // CARDINALITY_13_MAY_2022
