
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
#include "mega/values/compilation/reserved_symbols.hpp"

#include "mega/eg_common_strings.hpp"
#include "mega/common_strings.hpp"

#include "common/serialisation.hpp"
#include "common/hash.hpp"

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
    inline void serialize( Archive& archive, const unsigned int )
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
        inline void serialize( Archive& archive, const unsigned int )
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
        interface::TypeID                         find( const interface::SymbolIDSequence& symbolVector ) const;
        interface::SubObjectID                    add( const interface::SymbolIDSequence& symbolVector );

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
        inline void serialize( Archive& archive, const unsigned int )
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
        concrete::TypeID                        find( const interface::TypeIDSequence& typeIDSequence ) const;
        concrete::SubObjectID                   add( const interface::TypeIDSequence& typeIDSequence );

    private:
        concrete::ObjectID        m_objectID; // index into Vector
        interface::TypeIDSequence m_objectTypeIDs;
        SubTypeMap                m_subTypes;
    };

    SymbolTable();

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
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

    inline const InterfaceObject::Vector& getInterfaceObjects() const { return m_interfaceVector; }
    inline const ConcreteObject::Vector&  getConcreteObjects() const { return m_concreteVector; }

    const std::string&                   getSymbol( const interface::SymbolID& symbolID ) const;
    std::optional< interface::SymbolID > findSymbol( const SymbolTraits::Symbol& symbol ) const;
    const InterfaceObject*               findInterfaceObject( const interface::SymbolIDSequence& symbolVector ) const;
    const ConcreteObject*                findConcreteObject( const interface::TypeIDSequence& typeIDSequence ) const;
    void                                 add( const SymbolRequest& request );

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
