
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

#include "service/python/python_type_system.hpp"

namespace mega::service::python
{

TypeSystem::TypeSystem( PythonModule& module, const Project& project )
    : m_module( module )
    , m_pDatabase( std::make_unique< runtime::PythonDatabase >( project.getProjectDatabase() ) )
{
}

void TypeSystem::reload( const Project& project )
{
    m_pDatabase.reset();
    m_pDatabase = std::make_unique< runtime::PythonDatabase >( project.getProjectDatabase() );
}

Type::Ptr TypeSystem::getLinkType( TypeID typeID )
{
    Type::Ptr pResult;

    auto iFind = m_links.find( typeID );
    if( iFind != m_links.end() )
    {
        pResult = iFind->second;
    }
    else
    {
        Type::ObjectTypeSet objectTypes;
        m_pDatabase->getLinkObjectTypes( typeID, objectTypes );
        VERIFY_RTE( !objectTypes.empty() );

        // establish the symbol table for the object types
        Type::SymbolTablePtr pSymbolTable = std::make_shared< Type::SymbolTable >();
        Type::SymbolTablePtr pLinkTable   = std::make_shared< Type::SymbolTable >();
        for( auto objectID : objectTypes )
        {
            m_pDatabase->getObjectSymbols( objectID, *pSymbolTable, *pLinkTable );
        }

        // create type for object
        pResult = std::make_shared< Type >( m_module, *this, pSymbolTable, pLinkTable, std::move( objectTypes ) );
        m_links.insert( { typeID, pResult } );
    }

    return pResult;
}

PyObject* TypeSystem::cast( const mega::reference& ref )
{
    Type::Ptr pType;
    {
        auto objectID = ref.getType().getObjectID();
        auto iFind    = m_types.find( objectID );
        if( iFind != m_types.end() )
        {
            pType = iFind->second;
        }
        else
        {
            // establish the symbol table for the object type
            Type::SymbolTablePtr pSymbolTable = std::make_shared< Type::SymbolTable >();
            Type::SymbolTablePtr pLinkTable   = std::make_shared< Type::SymbolTable >();

            m_pDatabase->getObjectSymbols( objectID, *pSymbolTable, *pLinkTable );

            // create type for object
            pType = std::make_shared< Type >(
                m_module, *this, pSymbolTable, pLinkTable, Type::ObjectTypeSet{ objectID } );
            m_types.insert( { objectID, pType } );
        }
    }

    return pType->createReference( ref );
}

} // namespace mega::service::python

#endif // GUARD_2023_September_14_python_type_system
