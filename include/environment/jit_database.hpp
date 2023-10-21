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

#include "mega/values/compilation/type_id.hpp"
#include "mega/values/compilation/relation_id.hpp"
#include "mega/values/compilation/invocation_id.hpp"
#include "mega/values/compilation/size_alignment.hpp"

#include "database/api.hpp"
#include "environment/environment_archive.hpp"
#include "database/exception.hpp"
#include "database/manifest_data.hpp"

#include "database/FinalStage.hxx"
#include "database/manifest.hxx"


namespace mega::runtime
{

class EGDB_EXPORT JITDatabase
{
    using InterfaceTypeIDMap    = std::map< mega::TypeID, ::FinalStage::Symbols::InterfaceTypeID* >;
    using ConcreteTypeIDMap     = std::map< TypeID, ::FinalStage::Symbols::ConcreteTypeID* >;
    using DynamicInvocationsMap = std::map< InvocationID, const ::FinalStage::Operations::Invocation* >;

    const FinalStage::Operations::Invocation* getExistingInvocation( const InvocationID& invocation ) const;

public:
    JITDatabase( const boost::filesystem::path& projectDatabasePath );

    const io::ArchiveEnvironment& getEnvironment() const { return m_environment; }
    const io::Manifest&           getManifest() const { return m_manifest; }

    FinalStage::HyperGraph::Relation* getRelation( const RelationID& relationID ) const;
    using RelationMap = std::unordered_map< RelationID, FinalStage::HyperGraph::Relation*, RelationID::Hash >;
    const RelationMap& getRelations() const { return m_relations; }

    SizeAlignment getObjectSize( TypeID objectType ) const;

    const FinalStage::Operations::Invocation* getInvocation( const InvocationID& invocation ) const;
    const FinalStage::Operations::Invocation* tryGetInvocation( const InvocationID& invocation ) const;
    void addDynamicInvocation( const InvocationID& invocation, const FinalStage::Operations::Invocation* pInvocation );

    TypeID                                       getInterfaceTypeID( TypeID concreteTypeID ) const;
    TypeID                                       getSingularConcreteTypeID( TypeID interfaceTypeID ) const;
    std::vector< TypeID >                        getCompatibleConcreteTypes( TypeID interfaceTypeID ) const;
    FinalStage::Concrete::Object*                getObject( TypeID objectType ) const;
    FinalStage::Interface::Action*               getAction( TypeID interfaceTypeID ) const;
    const FinalStage::Components::Component*     getComponent( TypeID objectType ) const;
    const FinalStage::Components::Component*     getOperationComponent( TypeID interfaceTypeID ) const;
    std::vector< FinalStage::Concrete::Object* > getObjects() const;

    std::vector< FinalStage::Concrete::Dimensions::User* >       getUserDimensions() const;
    std::vector< FinalStage::Concrete::Dimensions::Link* >       getLinkDimensions() const;

    using ConcreteToInterface = std::vector< std::pair< TypeID, TypeID > >;
    void getConcreteToInterface( ConcreteToInterface& concreteToInterface ) const;

    using PrefabBindings = std::vector< FinalStage::UnityAnalysis::Binding* >;
    PrefabBindings getPrefabBindings() const;

private:
    io::ArchiveEnvironment                            m_environment;
    io::Manifest                                      m_manifest;
    FinalStage::Database                              m_database;
    std::vector< FinalStage::Components::Component* > m_components;
    
    FinalStage::Symbols::SymbolTable*                 m_pSymbolTable;
    ConcreteTypeIDMap                                 m_concreteTypeIDs;
    InterfaceTypeIDMap                                m_interfaceTypeIDs;

    FinalStage::Concrete::Object*                     m_pConcreteRoot;
    DynamicInvocationsMap                             m_dynamicInvocations;
    RelationMap                                       m_relations;
};

} // namespace mega::runtime

#endif // DATABASE_8_AUG_2022
