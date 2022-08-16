#ifndef CODE_GENERATOR_16_AUG_2022
#define CODE_GENERATOR_16_AUG_2022

#include "runtime/runtime.hpp"

#include "database.hpp"

namespace mega
{
    namespace runtime
    {
        class CodeGenerator
        {
        public:


            void generate_read( DatabaseInstance& database, const mega::InvocationID& invocationID );

        };
    }
}

#endif //CODE_GENERATOR_16_AUG_2022
