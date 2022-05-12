#ifndef OBJECT_LOADER_9_APRIL_2022
#define OBJECT_LOADER_9_APRIL_2022

#include "object_info.hpp"
#include "object.hpp"

namespace data
{
    class ObjectPartLoader
    {
    public:
        virtual ~ObjectPartLoader() {}
        virtual mega::io::Object* load( const mega::io::ObjectInfo& objectInfo ) = 0;
    };

    class NullObjectPartLoader : public ObjectPartLoader
    {
        virtual mega::io::Object* load( const mega::io::ObjectInfo& objectInfo )
        {
            THROW_RTE( "load called on NullObjectPartLoader" );
        }
    };
}

#endif //OBJECT_LOADER_9_APRIL_2022
