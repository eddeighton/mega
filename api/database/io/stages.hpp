
#ifndef STAGES_25_MAR_2022
#define STAGES_25_MAR_2022

#include "file.hpp"

#include "boost/filesystem/path.hpp"

#include <memory>
#include <optional>

namespace mega
{
namespace io
{
    namespace stage
    {
        
// clang-format off
#define STAGE_TYPE( stage )     \
        class stage            \
        {                       \
        };
#include "stage_types.hxx"
// clang-format on
#undef STAGE_TYPE
    } // namespace stage

    template < class TCreatingStage >
    class StagedFile : public File
    {
    public:
        using CreatingStage = TCreatingStage;
        StagedFile( const FileInfo& fileInfo )
            : File( fileInfo )
        {
        }
    };

    namespace file
    {

// clang-format off
#define FILE_TYPE( filetype, stage_name )                              \
    class filetype : public StagedFile< stage::stage_name >            \
    {                                                                  \
    public:                                                            \
        filetype( const FileInfo& fileInfo ) : StagedFile( fileInfo )  \
        {                                                              \
        }                                                              \
    };
#include "file_types.hxx"
// clang-format on
#undef FILE_TYPE

    }
} // namespace io
} // namespace mega

#endif // STAGES_25_MAR_2022
