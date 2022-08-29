#ifndef OWNERSHIP_13_MAY_2022
#define OWNERSHIP_13_MAY_2022

#include "database/common/serialisation.hpp"

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
    Ownership( Value value )
        : m_value( value )
    {
    }

    const char*      str() const;
    static Ownership fromStr( const char* psz );

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

std::ostream& operator<<( std::ostream& os, mega::Ownership ownership );

namespace mega
{
inline void to_json( nlohmann::json& j, mega::Ownership ownership )
{
    j = nlohmann::json{ { "ownership", ownership.str() } };
}
} // namespace mega


#endif // MODES_10_MAY_2022
