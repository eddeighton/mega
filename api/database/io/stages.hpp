
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
        class ObjectParse
        {
        };

        class ObjectInterface
        {
        };

        class ObjectInterfaceAnalysis
        {
        };

        class ProjectInterface
        {
        };

        class ObjectOperations
        {
        };
    } // namespace stage

    template < class TCreatingStage >
    class StagedFile : public File
    {
    public:
        using CreatingStage = TCreatingStage;
        StagedFile( const File::Info& fileInfo )
            : File( fileInfo )
        {
        }
    };

    class ObjectAST : public StagedFile< stage::ObjectParse >
    {
    public:
        ObjectAST( const File::Info& fileInfo )
            : StagedFile( fileInfo )
        {
        }
    };

    class ObjectBody : public StagedFile< stage::ObjectParse >
    {
    public:
        ObjectBody( const File::Info& fileInfo )
            : StagedFile( fileInfo )
        {
        }
    };
} // namespace io
} // namespace mega

#endif // STAGES_25_MAR_2022
