#ifndef SYMBOL_UTILS_SEPT_19_2022
#define SYMBOL_UTILS_SEPT_19_2022

#include "mega/reference.hpp"
#include "database/types/invocation_id.hpp"

#include <ostream>

namespace mega
{
    namespace runtime
    {
        static inline void symbolPrefix( const char* prefix, mega::TypeID objectTypeID, std::ostream& os )
        {
            std::ostringstream osTypeID;
            osTypeID << prefix << objectTypeID;
            os << "_Z" << osTypeID.str().size() << osTypeID.str();
        }

        static inline void symbolPrefix( const mega::InvocationID& invocationID, std::ostream& os )
        {
            std::ostringstream osTypeID;
            osTypeID << invocationID;
            os << "_Z" << osTypeID.str().size() << osTypeID.str();
        }
    }
}

#endif //SYMBOL_UTILS_SEPT_19_2022
