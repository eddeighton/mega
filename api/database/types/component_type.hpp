#ifndef COMPONENT_TYPE_22_JUNE_2022
#define COMPONENT_TYPE_22_JUNE_2022

#include "database/common/serialisation.hpp"

#include <ostream>

namespace mega
{

class ComponentType
{
public:
    enum Value
    {
        eInterface,
        eLibrary,
        TOTAL_COMPONENT_TYPES
    };

    const char*            str() const;
    static ComponentType fromStr( const char* psz );

    ComponentType()
        : m_value( TOTAL_COMPONENT_TYPES )
    {
    }
    ComponentType( Value value )
        : m_value( value )
    {
    }

    Value get() const { return m_value; }
    void  set( Value value ) { m_value = value; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        archive& boost::serialization::make_nvp( "componentType", m_value );
    }
private:
    Value m_value;
};
}

std::ostream& operator<<( std::ostream& os, mega::ComponentType componentType );

namespace mega
{
inline void to_json( nlohmann::json& j, const mega::ComponentType& componentType )
{
    j = nlohmann::json{ { "component_type", componentType.str() } };
}
} // namespace mega


#endif //COMPONENT_TYPE_22_JUNE_2022