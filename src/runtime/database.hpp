
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
    mega::TypeID                              getInterfaceTypeID( mega::TypeID concreteTypeID ) const;
    const FinalStage::Concrete::Object*       getObject( mega::TypeID objectType ) const;
    const FinalStage::Components::Component*  getComponent( mega::TypeID objectType ) const;
    const FinalStage::Components::Component*  getOperationComponent( mega::TypeID objectType ) const;
    mega::U64                                 getTotalDomainSize( mega::TypeID concreteID ) const;
    mega::U64                                 getLocalDomainSize( mega::TypeID concreteID ) const;

private:
    mega::io::ArchiveEnvironment                          m_environment;
    mega::io::Manifest                                    m_manifest;
    FinalStage::Database                                  m_database;
    std::vector< FinalStage::Components::Component* >     m_components;
    FinalStage::Symbols::SymbolTable*                     m_pSymbolTable;
    std::map< mega::I32, FinalStage::Concrete::Context* > m_concreteIDs;
};

} // namespace runtime
} // namespace mega

#endif // DATABASE_8_AUG_2022
