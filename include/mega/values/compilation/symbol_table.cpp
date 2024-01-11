
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

#include "mega/values/compilation/symbol_table.hpp"

#include "mega/values/compilation/reserved_symbols.hpp"

namespace mega
{

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

interface::TypeID SymbolTable::InterfaceObject::find( const interface::SymbolIDSequence& symbolVector ) const
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

interface::SubObjectID SymbolTable::InterfaceObject::add( const interface::SymbolIDSequence& symbolVector )
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
concrete::TypeID SymbolTable::ConcreteObject::find( const interface::TypeIDSequence& typeIDSequence ) const
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

concrete::SubObjectID SymbolTable::ConcreteObject::add( const interface::TypeIDSequence& typeIDSequence )
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SymbolTable::SymbolTable()
{
    // add reserved symbols
    {
        using namespace interface;

        SymbolID::ValueType id = 0;
        for( const auto& strSymbol : getReservedSymbols() )
        {
            m_symbolVector.push_back( strSymbol );
            m_symbolMap.insert( { strSymbol, SymbolID{ id } } );
            ++id;
        }
    }

    // always have empty string for id zero and root for id one
    {
        using namespace interface;

        // null interface object type
        InterfaceObject nullInterfaceObject( NULL_OBJECT_ID, { NULL_SYMBOL_ID } );
        nullInterfaceObject.add( { OWNER_SYMBOL_ID } );
        nullInterfaceObject.add( { STATE_SYMBOL_ID } );

        m_interfaceVector.push_back( nullInterfaceObject );
        m_interfaceMap.insert( { { NULL_SYMBOL_ID }, NULL_OBJECT_ID } );

        // root interface object type
        m_interfaceVector.push_back( InterfaceObject( ROOT_OBJECT_ID, { ROOT_SYMBOL_ID } ) );
        m_interfaceMap.insert( { { ROOT_SYMBOL_ID }, ROOT_OBJECT_ID } );
    }

    // null concrete object type
    ConcreteObject nullConcreteObject( concrete::NULL_OBJECT_ID, { interface::NULL_TYPE_ID } );
    nullConcreteObject.add( { interface::OWNER_TYPE_ID } );
    nullConcreteObject.add( { interface::STATE_TYPE_ID } );
    m_concreteVector.push_back( nullConcreteObject );
    m_concreteMap.insert( { { interface::NULL_TYPE_ID }, concrete::NULL_OBJECT_ID } );

    // root concrete object type
    m_concreteVector.push_back( ConcreteObject( concrete::ROOT_OBJECT_ID, { interface::ROOT_TYPE_ID } ) );
    m_concreteMap.insert( { { interface::ROOT_TYPE_ID }, concrete::ROOT_OBJECT_ID } );
}

const std::string& SymbolTable::getSymbol( const interface::SymbolID& symbolID ) const
{
    const auto symbolIndex = symbolID.getValue();
    VERIFY_RTE_MSG( symbolIndex >= 0 && symbolIndex < m_symbolVector.size(), "Invalid symbol index" );
    return m_symbolVector[ symbolIndex ];
}

std::optional< interface::SymbolID > SymbolTable::findSymbol( const SymbolTraits::Symbol& symbol ) const
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

const SymbolTable::InterfaceObject* SymbolTable::findInterfaceObject( const interface::SymbolIDSequence& symbolVector ) const
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

const SymbolTable::ConcreteObject* SymbolTable::findConcreteObject( const interface::TypeIDSequence& typeIDSequence ) const
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

void SymbolTable::add( const SymbolRequest& request )
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

} // namespace mega
