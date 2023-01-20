//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef DATABASE_8_AUG_2022
#define DATABASE_8_AUG_2022

#include "mega/memory.hpp"
#include "mega/relation_id.hpp"

#include "database/common/api.hpp"
#include "database/common/environment_archive.hpp"
#include "database/common/exception.hpp"
#include "database/common/manifest_data.hpp"

#include "database/model/FinalStage.hxx"
#include "database/model/manifest.hxx"

#include "mega/invocation_id.hpp"

namespace mega::runtime
{

class EGDB_EXPORT DatabaseInstance
{
public:
    using ConcreteTypeIDMap = std::map< mega::TypeID, ::FinalStage::Symbols::ConcreteTypeID* >;
    DatabaseInstance( const boost::filesystem::path& projectDatabasePath );

    FinalStage::HyperGraph::Relation* getRelation( const RelationID& relationID ) const;

    mega::SizeAlignment                          getObjectSize( mega::TypeID objectType ) const;
    const FinalStage::Operations::Invocation*    getInvocation( const mega::InvocationID& invocation ) const;
    mega::TypeID                                 getInterfaceTypeID( mega::TypeID concreteTypeID ) const;
    FinalStage::Concrete::Object*                getObject( mega::TypeID objectType ) const;
    const FinalStage::Components::Component*     getComponent( mega::TypeID objectType ) const;
    const FinalStage::Components::Component*     getOperationComponent( mega::TypeID objectType ) const;
    mega::U64                                    getLocalDomainSize( mega::TypeID concreteID ) const;
    std::vector< FinalStage::Concrete::Object* > getObjects() const;

    std::vector< FinalStage::Concrete::Dimensions::User* > getUserDimensions() const;
    std::vector< FinalStage::Concrete::Dimensions::LinkReference* > getLinkDimensions() const;
    std::vector< FinalStage::Concrete::Dimensions::Allocation* > getAllocationDimensions() const;
private:
    mega::io::ArchiveEnvironment                      m_environment;
    mega::io::Manifest                                m_manifest;
    FinalStage::Database                              m_database;
    std::vector< FinalStage::Components::Component* > m_components;
    FinalStage::Symbols::SymbolTable*                 m_pSymbolTable;
    ConcreteTypeIDMap                                 m_concreteTypeIDs;
    FinalStage::Concrete::Object*                     m_pConcreteRoot;

    using RelationMap = std::unordered_map< RelationID, FinalStage::HyperGraph::Relation*, RelationID::Hash >;
    RelationMap m_relations;
};

} // namespace mega::runtime

#endif // DATABASE_8_AUG_2022
