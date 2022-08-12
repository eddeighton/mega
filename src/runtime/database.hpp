
#ifndef DATABASE_8_AUG_2022
#define DATABASE_8_AUG_2022

#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"
#include "database/common/manifest_data.hpp"

#include "database/model/FinalStage.hxx"
#include "database/model/manifest.hxx"

#include "database/types/invocation_id.hpp"

namespace mega
{
namespace runtime
{
class DatabaseInstance
{
public:
    DatabaseInstance( const boost::filesystem::path& projectDatabasePath );

    const FinalStage::Operations::Invocation* getInvocation( const mega::InvocationID& invocation ) const;

private:
    mega::io::ArchiveEnvironment                      m_environment;
    mega::io::Manifest                                m_manifest;
    FinalStage::Database                              m_database;
    std::vector< FinalStage::Components::Component* > m_components;
    FinalStage::Symbols::SymbolTable*                 m_pSymbolTable;
};

} // namespace runtime
} // namespace mega

#endif // DATABASE_8_AUG_2022
