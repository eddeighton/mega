#ifndef MODES_10_MAY_2022
#define MODES_10_MAY_2022

namespace mega
{
    enum CompilationMode
    {
        eInterface,
        eOperations,
        eImplementation,
        eCPP,
        TOTAL_COMPILATION_MODES
    };

    const char* toStr( CompilationMode mode );
    CompilationMode fromStr( const char* psz );
}

#endif //MODES_10_MAY_2022
