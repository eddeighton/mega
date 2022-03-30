#ifndef IO_FACTORY_25_MAR_2022
#define IO_FACTORY_25_MAR_2022

#include "object_info.hpp"
#include "object.hpp"

namespace mega
{
namespace io
{
    class Factory
    {
    public:
        static Object* create( const ObjectInfo& objectSpec );
    };

} // namespace io
} // namespace mega

#endif // IO_FACTORY_25_MAR_2022
