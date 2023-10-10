
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

#ifndef GUARD_2023_September_02_python_type_system
#define GUARD_2023_September_02_python_type_system

#include "service/python/python_type.hpp"

#include "database/python_database.hpp"

#include "utilities/project.hpp"

#include "mega/type_id.hpp"
#include "mega/reference.hpp"

#include <pybind11/pybind11.h>

#include <unordered_map>

namespace mega::service::python
{
class PythonModule;

class TypeSystem
{
    using ConcreteObjectTypes = std::unordered_map< TypeID::SubValueType, Type::Ptr >;
    struct ConcreteObjectLinkSymbol
    {
        TypeID::SubValueType concreteObjectID;
        TypeID               linkSymbol;

        inline bool operator==( const ConcreteObjectLinkSymbol& value ) const
        {
            return ( concreteObjectID == value.concreteObjectID ) && ( linkSymbol == value.linkSymbol );
        }

        struct Hash
        {
            inline U64 operator()( const ConcreteObjectLinkSymbol& value ) const noexcept
            {
                return value.linkSymbol.getSymbolID() + ( static_cast< U64 >( value.concreteObjectID ) << 32 );
            }
        };
    };
    using LinkTypeMap    = std::unordered_map< ConcreteObjectLinkSymbol, Type::Ptr, ConcreteObjectLinkSymbol::Hash >;
    using SymbolTablePtr = std::unique_ptr< Type::SymbolTable >;
    using SymbolTableMap = std::unordered_map< TypeID::SubValueType, SymbolTablePtr >;
    using DatabasePtr    = std::unique_ptr< runtime::PythonDatabase >;

public:
    using Ptr = std::unique_ptr< TypeSystem >;

    TypeSystem( PythonModule& module, const Project& project );

    using ObjectTypesMap = std::map< std::string, TypeID::SubValueType >;
    ObjectTypesMap getObjectTypes() const;

    void reload( const Project& project );

    Type::Ptr getLinkType( TypeID::SubValueType concreteObjectID, TypeID typeID );
    PyObject* cast( const mega::reference& ref );

private:
    PythonModule&       m_module;
    DatabasePtr         m_pDatabase;
    ConcreteObjectTypes m_concreteObjectTypes;
    LinkTypeMap         m_links;
    SymbolTableMap      m_symbolTables;
};

} // namespace mega::service::python

#endif // GUARD_2023_September_02_python_type_system
