


#ifndef DERIVATION_29_AUG_2022
#define DERIVATION_29_AUG_2022

#include "database/common/serialisation.hpp"

#include <ostream>

namespace mega
{
class DerivationDirection
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

std::ostream& operator<<( std::ostream& os, mega::DerivationDirection derivation );

namespace mega
{
inline void to_json( nlohmann::json& j, mega::DerivationDirection derivation )
{
    j = nlohmann::json{ { "derivation", derivation.str() } };
}
} // namespace mega

#endif // DERIVATION_29_AUG_2022
