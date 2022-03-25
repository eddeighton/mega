#ifndef IO_FACTORY_25_MAR_2022
#define IO_FACTORY_25_MAR_2022

#include "object.hpp"

namespace mega
{
namespace io
{
    class Factory
    {
    public:
        virtual ~Factory();
        virtual Object* create( const Object& objectSpec ) = 0;
    };

} // namespace io
} // namespace mega

#endif // IO_FACTORY_25_MAR_2022
