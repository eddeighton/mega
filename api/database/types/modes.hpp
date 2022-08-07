#ifndef MODES_10_MAY_2022
#define MODES_10_MAY_2022

#include "database/common/serialisation.hpp"

#include <ostream>

namespace mega
{
class CompilationMode
{
public:
    enum Value
    {
        eNormal,
        eInterface,
        eLibrary,
        eOperations,
        eCPP,
        TOTAL_COMPILATION_MODES
    };

    const char*            str() const;
    static CompilationMode fromStr( const char* psz );

    CompilationMode()
        : m_value( TOTAL_COMPILATION_MODES )
    {
    }
    CompilationMode( Value value )
        : m_value( value )
    {
    }

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

std::ostream& operator<<( std::ostream& os, mega::CompilationMode compilationMode );

namespace mega
{
inline void to_json( nlohmann::json& j, const mega::CompilationMode& compilationMode )
{
    j = nlohmann::json{ { "compilation_mode", compilationMode.str() } };
}
} // namespace mega

#endif // MODES_10_MAY_2022
