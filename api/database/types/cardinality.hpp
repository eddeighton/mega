


#ifndef CARDINALITY_13_MAY_2022
#define CARDINALITY_13_MAY_2022

#include "database/common/serialisation.hpp"

#include <optional>
#include <ostream>

namespace mega
{

struct Cardinality
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
        , m_allow_null( false )
    {
    }

    Cardinality( NumberType number, bool bAllowNull = false )
        : m_number( number )
        , m_allow_null( bAllowNull )
    {
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& m_number;
        archive& m_allow_null;
    }

    bool       isNullAllowed() const { return m_allow_null; }
    bool       isZero() const { return m_number == ZERO; }
    bool       isOne() const { return m_number == ONE; }
    bool       isMany() const { return m_number == MANY; }
    bool       isNumber() const { return !isZero() && !isOne() && !isMany(); }
    NumberType getNumber() const { return m_number; }

    void setZero() { m_number = ZERO; }
    void setOne() { m_number = ONE; }
    void setMany() { m_number = MANY; }
    void setNumber( NumberType number ) { m_number = number; }
    void setIsNullAllowed( bool bIsAllowed ) { m_allow_null = bIsAllowed; }

private:
    NumberType m_number;
    bool       m_allow_null;
};

struct CardinalityRange
{
    CardinalityRange() {}

    CardinalityRange( const Cardinality& minimum, const Cardinality& maximum )
        : m_minimum( minimum )
        , m_maximum( maximum )
    {
    }

    const Cardinality& minimum() const { return m_minimum; }
    const Cardinality& maximum() const { return m_maximum; }

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

std::ostream& operator<<( std::ostream& os, const mega::CardinalityRange& cardinalityRange );

namespace mega
{
inline void to_json( nlohmann::json& j, const mega::Cardinality& cardinality )
{
    j = nlohmann::json{ { "isNullAllowed", cardinality.isNullAllowed() }, { "number", cardinality.getNumber() } };
}
inline void to_json( nlohmann::json& j, const mega::CardinalityRange& cardinalityRange )
{
    j = nlohmann::json{ { "minimum", cardinalityRange.minimum() }, { "maximum", cardinalityRange.maximum() } };
}
} // namespace mega

#endif // CARDINALITY_13_MAY_2022
