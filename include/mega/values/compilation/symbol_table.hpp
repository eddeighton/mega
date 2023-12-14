
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

#ifndef GUARD_2023_December_03_symbol_table
#define GUARD_2023_December_03_symbol_table

#include "mega/values/native_types.hpp"

#include "mega/values/compilation/interface/symbol_id.hpp"
#include "mega/values/compilation/interface/type_id.hpp"

#include "mega/values/compilation/concrete/type_id.hpp"
#include "mega/values/compilation/concrete/type_id_instance.hpp"

#include "mega/values/compilation/type_id_sequence.hpp"
#include "mega/values/hash.hpp"
#include "mega/values/compilation/operation_id.hpp"
#include "mega/common_strings.hpp"

#include "common/serialisation.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace mega
{

struct SymbolTraits
{
    using Symbol             = std::string;
    using SymbolVector       = std::vector< Symbol >;
    using SymbolIDVectorPair = std::pair< interface::SymbolIDSequence, interface::SymbolIDSequence >;
    using SymbolMap          = std::unordered_map< Symbol, interface::SymbolID >;
    using TypeIDSequencePair = std::pair< interface::TypeIDSequence, interface::TypeIDSequence >;

    struct SymbolVectorHash
    {
        std::size_t operator()( const interface::SymbolIDSequence& symbolVector ) const
        {
            return common::Hash( symbolVector ).get();
        }
    };
    struct TypeIDSequenceHash
    {
        std::size_t operator()( const interface::TypeIDSequence& typeIDSequence ) const
        {
            return common::Hash( typeIDSequence ).get();
        }
    };
};

class SymbolRequest
{
public:
    std::set< SymbolTraits::Symbol >             newSymbols;
    std::set< interface::SymbolIDSequence >      newInterfaceObjects;
    std::set< SymbolTraits::SymbolIDVectorPair > newInterfaceElements;
    std::set< interface::TypeIDSequence >        newConcreteObjects;
    std::set< SymbolTraits::TypeIDSequencePair > newConcreteElements;

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "newSymbols", newSymbols );
            archive& boost::serialization::make_nvp( "newInterfaceObjects", newInterfaceObjects );
            archive& boost::serialization::make_nvp( "newInterfaceElements", newInterfaceElements );
            archive& boost::serialization::make_nvp( "newConcreteObjects", newConcreteObjects );
            archive& boost::serialization::make_nvp( "newConcreteElements", newConcreteElements );
        }
        else
        {
            archive& newSymbols;
            archive& newInterfaceObjects;
            archive& newInterfaceElements;
            archive& newConcreteObjects;
            archive& newConcreteElements;
        }
    }
};

class SymbolTable
{
public:
    class InterfaceObject
    {
    public:
        using Vector = std::vector< InterfaceObject >;
        using Map
            = std::unordered_map< interface::SymbolIDSequence, interface::ObjectID, SymbolTraits::SymbolVectorHash >;
        using SubTypeMap
            = std::unordered_map< interface::SymbolIDSequence, interface::SubObjectID, SymbolTraits::SymbolVectorHash >;

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
            {
                archive& boost::serialization::make_nvp( "objectID", m_objectID );
                archive& boost::serialization::make_nvp( "objectSymbols", m_objectSymbols );
                archive& boost::serialization::make_nvp( "subTypes", m_subTypes );
            }
            else
            {
                archive& m_objectID;
                archive& m_objectSymbols;
                archive& m_subTypes;
            }
        }

        InterfaceObject() = default;
        InterfaceObject( interface::ObjectID objectID, interface::SymbolIDSequence symbolIDs )
            : m_objectID( objectID )
            , m_objectSymbols( std::move( symbolIDs ) )
        {
        }

        inline const interface::SymbolIDSequence& getSymbols() const { return m_objectSymbols; }
        inline interface::ObjectID                getObjectID() const { return m_objectID; }
        inline interface::TypeID                  find( const interface::SymbolIDSequence& symbolVector ) const
        {
            auto iFind = m_subTypes.find( symbolVector );
            if( iFind != m_subTypes.end() )
            {
                return interface::TypeID {
                    m_objectID, iFind->second
                };
            }
            else
            {
                return {};
            }
        }

        inline interface::SubObjectID add( const interface::SymbolIDSequence& symbolVector )
        {
            auto iFind = m_subTypes.find( symbolVector );
            if( iFind != m_subTypes.end() )
            {
                return iFind->second;
            }
            else
            {
                const auto newSubType = static_cast< interface::SubObjectID >( m_subTypes.size() + 1 );
                m_subTypes.insert( { symbolVector, newSubType } );
                return newSubType;
            }
        }

    private:
        interface::ObjectID         m_objectID; // index into Vector
        interface::SymbolIDSequence m_objectSymbols;
        SubTypeMap                  m_subTypes;
    };

    class ConcreteObject
    {
    public:
        using Vector = std::vector< ConcreteObject >;
        using Map
            = std::unordered_map< interface::TypeIDSequence, concrete::ObjectID, SymbolTraits::TypeIDSequenceHash >;
        using SubTypeMap
            = std::unordered_map< interface::TypeIDSequence, concrete::SubObjectID, SymbolTraits::TypeIDSequenceHash >;

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
            {
                archive& boost::serialization::make_nvp( "objectID", m_objectID );
                archive& boost::serialization::make_nvp( "objectTypeIDs", m_objectTypeIDs );
                archive& boost::serialization::make_nvp( "subTypes", m_subTypes );
            }
            else
            {
                archive& m_objectID;
                archive& m_objectTypeIDs;
                archive& m_subTypes;
            }
        }

        ConcreteObject() = default;
        ConcreteObject( concrete::ObjectID objectID, interface::TypeIDSequence symbolIDs )
            : m_objectID( objectID )
            , m_objectTypeIDs( std::move( symbolIDs ) )
        {
        }

        inline const interface::TypeIDSequence& getInterfaceTypeIDs() const { return m_objectTypeIDs; }
        inline concrete::ObjectID               getObjectID() const { return m_objectID; }
        inline concrete::TypeID                 find( const interface::TypeIDSequence& typeIDSequence ) const
        {
            auto iFind = m_subTypes.find( typeIDSequence );
            if( iFind != m_subTypes.end() )
            {
                return concrete::TypeID( m_objectID, iFind->second );
            }
            else
            {
                return {};
            }
        }

        inline concrete::SubObjectID add( const interface::TypeIDSequence& typeIDSequence )
        {
            auto iFind = m_subTypes.find( typeIDSequence );
            if( iFind != m_subTypes.end() )
            {
                return iFind->second;
            }
            else
            {
                const auto newSubType = static_cast< concrete::SubObjectID >( m_subTypes.size() + 1 );
                auto [ iFind, _ ]     = m_subTypes.insert( { typeIDSequence, newSubType } );
                return iFind->second;
            }
        }

    private:
        concrete::ObjectID        m_objectID; // index into Vector
        interface::TypeIDSequence m_objectTypeIDs;
        SubTypeMap                m_subTypes;
    };

    SymbolTable()
    {
        // always have empty string for id zero and root for id one
        {
            using namespace interface;

            m_symbolVector.push_back( "" );
            m_symbolMap.insert( { "", NULL_SYMBOL_ID } );

            m_symbolVector.push_back( ROOT_SYMBOL );
            m_symbolMap.insert( { ROOT_SYMBOL, ROOT_SYMBOL_ID } );

            // null interface object type
            m_interfaceVector.push_back( InterfaceObject( NULL_OBJECT_ID, { NULL_SYMBOL_ID } ) );
            m_interfaceMap.insert( { { NULL_SYMBOL_ID }, NULL_OBJECT_ID } );

            // root interface object type
            m_interfaceVector.push_back( InterfaceObject( ROOT_OBJECT_ID, { ROOT_SYMBOL_ID } ) );
            m_interfaceMap.insert( { { ROOT_SYMBOL_ID }, ROOT_OBJECT_ID } );
        }

        // null concrete object type
        m_concreteVector.push_back( ConcreteObject( concrete::NULL_OBJECT_ID, { interface::NULL_TYPE_ID } ) );
        m_concreteMap.insert( { { interface::NULL_TYPE_ID }, concrete::NULL_OBJECT_ID } );

        // root concrete object type
        m_concreteVector.push_back( ConcreteObject( concrete::ROOT_OBJECT_ID, { interface::ROOT_TYPE_ID } ) );
        m_concreteMap.insert( { { interface::ROOT_TYPE_ID }, concrete::ROOT_OBJECT_ID } );

        // add operations symbols
        {
            const auto opStrings = getOperationStrings();
            for( interface::SymbolID::ValueType i = 0; i != HIGHEST_OPERATION_TYPE; ++i )
            {
                m_symbolVector.push_back( opStrings[ i ] );
                m_symbolMap.insert( { opStrings[ i ], interface::SymbolID( i ) } );
            }

            m_symbolVector.push_back( EG_TYPE_PATH );
            m_symbolMap.insert( { EG_TYPE_PATH, interface::SymbolID( id_TypePath ) } );
            m_symbolVector.push_back( EG_VARIANT_TYPE );
            m_symbolMap.insert( { EG_VARIANT_TYPE, interface::SymbolID( id_Variant ) } );
        }
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int version )
    {
        if constexpr( boost::serialization::IsXMLArchive< Archive >::value )
        {
            archive& boost::serialization::make_nvp( "symbolVector", m_symbolVector );
            archive& boost::serialization::make_nvp( "symbolMap", m_symbolMap );
            archive& boost::serialization::make_nvp( "interfaceVector", m_interfaceVector );
            archive& boost::serialization::make_nvp( "interfaceMap", m_interfaceMap );
            archive& boost::serialization::make_nvp( "concreteVector", m_concreteVector );
            archive& boost::serialization::make_nvp( "concreteMap", m_concreteMap );
        }
        else
        {
            archive& m_symbolVector;
            archive& m_symbolMap;
            archive& m_interfaceVector;
            archive& m_interfaceMap;
            archive& m_concreteVector;
            archive& m_concreteMap;
        }
    }

    inline const std::string& getSymbol( const interface::SymbolID& symbolID ) const
    {
        const auto symbolIndex = symbolID.getValue();
        VERIFY_RTE_MSG( symbolIndex >= 0 && symbolIndex < m_symbolVector.size(), "Invalid symbol index" );
        return m_symbolVector[ symbolIndex ];
    }

    inline const InterfaceObject::Vector& getInterfaceObjects() const { return m_interfaceVector; }
    inline const ConcreteObject::Vector&  getConcreteObjects() const { return m_concreteVector; }

    inline std::optional< interface::SymbolID > findSymbol( const SymbolTraits::Symbol& symbol ) const
    {
        auto iFind = m_symbolMap.find( symbol );
        if( iFind != m_symbolMap.end() )
        {
            return iFind->second;
        }
        else
        {
            return std::nullopt;
        }
    }

    inline const InterfaceObject* findInterfaceObject( const interface::SymbolIDSequence& symbolVector ) const
    {
        auto iFind = m_interfaceMap.find( symbolVector );
        if( iFind != m_interfaceMap.end() )
        {
            return &m_interfaceVector[ iFind->second.getValue() ];
        }
        else
        {
            return nullptr;
        }
    }

    inline const ConcreteObject* findConcreteObject( const interface::TypeIDSequence& typeIDSequence ) const
    {
        auto iFind = m_concreteMap.find( typeIDSequence );
        if( iFind != m_concreteMap.end() )
        {
            return &m_concreteVector[ iFind->second.getValue() ];
        }
        else
        {
            return nullptr;
        }
    }

    inline void add( const SymbolRequest& request )
    {
        for( const auto& str : request.newSymbols )
        {
            auto iFind = m_symbolMap.find( str );
            if( iFind == m_symbolMap.end() )
            {
                const auto symbolIndex = static_cast< interface::SymbolID::ValueType >( m_symbolVector.size() );
                m_symbolMap.insert( { str, interface::SymbolID{ symbolIndex } } );
                m_symbolVector.push_back( str );
            }
        }

        for( const interface::SymbolIDSequence& symbolIDVector : request.newInterfaceObjects )
        {
            auto iFind = m_interfaceMap.find( symbolIDVector );
            if( iFind == m_interfaceMap.end() )
            {
                const auto newInterfaceID = static_cast< interface::ObjectID >( m_interfaceVector.size() );
                m_interfaceMap.insert( { symbolIDVector, newInterfaceID } );
                m_interfaceVector.push_back( InterfaceObject{ newInterfaceID, symbolIDVector } );
            }
        }

        for( const SymbolTraits::SymbolIDVectorPair& symbolIDVectorPair : request.newInterfaceElements )
        {
            interface::ObjectID interfaceObjectType;
            {
                auto iFind = m_interfaceMap.find( symbolIDVectorPair.first );
                if( iFind == m_interfaceMap.end() )
                {
                    interfaceObjectType = static_cast< interface::ObjectID >( m_interfaceVector.size() );
                    m_interfaceMap.insert( { symbolIDVectorPair.first, interfaceObjectType } );
                    m_interfaceVector.push_back( InterfaceObject{ interfaceObjectType, symbolIDVectorPair.first } );
                }
                else
                {
                    interfaceObjectType = iFind->second;
                }
            }

            InterfaceObject& interfaceObject = m_interfaceVector[ interfaceObjectType.getValue() ];
            interfaceObject.add( symbolIDVectorPair.second );
        }

        for( const interface::TypeIDSequence& typeIDSequence : request.newConcreteObjects )
        {
            auto iFind = m_concreteMap.find( typeIDSequence );
            if( iFind == m_concreteMap.end() )
            {
                const auto newConcreteID = static_cast< concrete::ObjectID >( m_concreteVector.size() );
                m_concreteMap.insert( { typeIDSequence, newConcreteID } );
                m_concreteVector.push_back( ConcreteObject{ newConcreteID, typeIDSequence } );
            }
        }

        for( const SymbolTraits::TypeIDSequencePair& typeIDSequencePair : request.newConcreteElements )
        {
            concrete::ObjectID concreteObjectType;
            {
                auto iFind = m_concreteMap.find( typeIDSequencePair.first );
                if( iFind == m_concreteMap.end() )
                {
                    concreteObjectType = static_cast< concrete::ObjectID >( m_concreteVector.size() );
                    m_concreteMap.insert( { typeIDSequencePair.first, concreteObjectType } );
                    m_concreteVector.push_back( ConcreteObject{ concreteObjectType, typeIDSequencePair.first } );
                }
                else
                {
                    concreteObjectType = iFind->second;
                }
            }

            ConcreteObject& concreteObject = m_concreteVector[ concreteObjectType.getValue() ];
            concreteObject.add( typeIDSequencePair.second );
        }
    }

private:
    SymbolTraits::SymbolVector m_symbolVector;
    SymbolTraits::SymbolMap    m_symbolMap;
    InterfaceObject::Vector    m_interfaceVector;
    InterfaceObject::Map       m_interfaceMap;
    ConcreteObject::Vector     m_concreteVector;
    ConcreteObject::Map        m_concreteMap;
};

} // namespace mega

#endif // GUARD_2023_December_03_symbol_table
