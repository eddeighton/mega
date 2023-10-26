
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

#ifndef GUARD_2023_September_14_python_type_system
#define GUARD_2023_September_14_python_type_system

#include "service/python/type_system.hpp"

namespace mega::service::python
{

TypeSystem::TypeSystem( PythonModule& module, const Project& project )
    : m_module( module )
    , m_pDatabase( std::make_unique< runtime::PythonDatabase >( project.getProjectDatabase() ) )
{
}

TypeSystem::ObjectTypesMap TypeSystem::getObjectTypes() const
{
    return m_pDatabase->getObjectTypes();
}

void TypeSystem::reload( const Project& project )
{
    THROW_TODO;
    m_pDatabase.reset();
    m_pDatabase = std::make_unique< runtime::PythonDatabase >( project.getProjectDatabase() );
}

Type::Ptr TypeSystem::getLinkType( SubType concreteObjectID, TypeID typeID )
{
    const ConcreteObjectLinkSymbol cols{ concreteObjectID, typeID };

    Type::Ptr pResult;

    auto iFind = m_links.find( cols );
    if( iFind != m_links.end() )
    {
        pResult = iFind->second;
    }
    else
    {
        // collect the set of concrete object types the link is to
        Type::ConcreteObjectTypeSet concreteObjectTypes;
        m_pDatabase->getLinkObjectTypes( concreteObjectID, typeID, concreteObjectTypes );
        VERIFY_RTE( !concreteObjectTypes.empty() );
        using SymbolTypeID = std::pair< std::string, TypeID >;
        std::map< SymbolTypeID, std::size_t > symbolTypeIDCount, linkTypeIDCount;

        // for each object type collect its symbols and record them into a
        // map that associated with a count
        for( auto concreteObjectID : concreteObjectTypes )
        {
            std::unordered_map< std::string, TypeID > symbols, links;
            m_pDatabase->getConcreteObjectSymbols( concreteObjectID, symbols, links );

            for( const auto& [ strSymbol, typeID ] : symbols )
            {
                symbolTypeIDCount[ { strSymbol, typeID } ]++;
            }
            for( const auto& [ strSymbol, typeID ] : links )
            {
                linkTypeIDCount[ { strSymbol, typeID } ]++;
            }
        }

        // now establish the symbols as those that occur for ALL concrete types
        Type::SymbolTablePtr pSymbolTable = std::make_shared< Type::SymbolTable >();
        {
            for( const auto& [ symbolTypeID, count ] : symbolTypeIDCount )
            {
                if( count == concreteObjectTypes.size() )
                {
                    pSymbolTable->insert( symbolTypeID );
                }
            }
        }
        Type::SymbolTablePtr pLinkTable = std::make_shared< Type::SymbolTable >();
        {
            for( const auto& [ symbolTypeID, count ] : linkTypeIDCount )
            {
                if( count == concreteObjectTypes.size() )
                {
                    pLinkTable->insert( symbolTypeID );
                }
            }
        }

        // create type for object
        pResult
            = std::make_shared< Type >( m_module, *this, pSymbolTable, pLinkTable, std::move( concreteObjectTypes ) );
        m_links.insert( { cols, pResult } );
    }

    return pResult;
}

PyObject* TypeSystem::cast( const mega::reference& ref )
{
    Type::Ptr pType;
    {
        auto concreteObjectID = ref.getType().getObjectID();
        auto iFind            = m_concreteObjectTypes.find( concreteObjectID );
        if( iFind != m_concreteObjectTypes.end() )
        {
            pType = iFind->second;
        }
        else
        {
            // establish the symbol table for the object type
            Type::SymbolTablePtr pSymbolTable = std::make_shared< Type::SymbolTable >();
            Type::SymbolTablePtr pLinkTable   = std::make_shared< Type::SymbolTable >();

            if( ref.valid() )
            {
                m_pDatabase->getConcreteObjectSymbols( concreteObjectID, *pSymbolTable, *pLinkTable );
            }

            // create type for object
            pType = std::make_shared< Type >(
                m_module, *this, pSymbolTable, pLinkTable, Type::ConcreteObjectTypeSet{ concreteObjectID } );
            m_concreteObjectTypes.insert( { concreteObjectID, pType } );
        }
    }

    return pType->createReference( ref );
}

} // namespace mega::service::python

#endif // GUARD_2023_September_14_python_type_system
