
#ifndef STAGES_25_MAR_2022
#define STAGES_25_MAR_2022

#include "file_info.hpp"
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
#define STAGE_TYPE( stage )                                         \
        struct stage                                                \
        {                                                           \
            static const FileInfo::Stage Stage = FileInfo::stage;   \
        };
#include "database/model/stage_types.hxx"
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
#define FILE_TYPE( filetype, stagetype )                                \
    class filetype : public StagedFile< stage::stagetype >              \
    {                                                                   \
    public:                                                             \
        filetype( const FileInfo& fileInfo ) : StagedFile( fileInfo )   \
        {                                                               \
        }                                                               \
        static const typename FileInfo::Type Type = FileInfo::filetype; \
    };
#include "database/model/file_types_global.hxx"
#include "database/model/file_types_object.hxx"
// clang-format on
#undef FILE_TYPE

    } // namespace file
} // namespace io
} // namespace mega

#endif // STAGES_25_MAR_2022
