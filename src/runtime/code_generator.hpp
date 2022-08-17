#ifndef CODE_GENERATOR_16_AUG_2022
#define CODE_GENERATOR_16_AUG_2022

#include "runtime/runtime.hpp"

#include "database.hpp"

#include "service/protocol/common/megastructure_installation.hpp"
#include "service/protocol/common/project.hpp"

#include <memory>

namespace mega
{
namespace runtime
{
class CodeGenerator
{
    class Pimpl;

public:
    CodeGenerator( const mega::network::MegastructureInstallation& megastructureInstallation,
                   const mega::network::Project&                   project );

    void generate_allocate( const DatabaseInstance& database, const mega::InvocationID& invocationID, std::ostream& os );
    void generate_read( const DatabaseInstance& database, const mega::InvocationID& invocationID, std::ostream& os );

private:

    void compileToIR( const boost::filesystem::path& sourcePath, const boost::filesystem::path& irPath ) const;

private:
    boost::filesystem::path  m_clangPath;
    boost::filesystem::path  m_tempDir;
    std::shared_ptr< Pimpl > m_pPimpl;
};
} // namespace runtime
} // namespace mega

#endif // CODE_GENERATOR_16_AUG_2022
