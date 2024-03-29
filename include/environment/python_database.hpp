
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

#ifndef GUARD_2023_September_06_python_database
#define GUARD_2023_September_06_python_database

#include "environment/environment_archive.hpp"

//#include "database/FinalStage.hxx"
//#include "database/manifest.hxx"

#include <map>

namespace mega::runtime
{
/*
class PythonDatabase
{
    using SymbolTypeIDMap    = std::map< mega::TypeID, ::FinalStage::Symbols::SymbolTypeID* >;
    using InterfaceTypeIDMap = std::map< mega::TypeID, ::FinalStage::Symbols::interface::TypeID* >;
    using ConcreteTypeIDMap  = std::map< TypeID, ::FinalStage::Symbols::concrete::TypeID* >;

public:
    PythonDatabase( boost::filesystem::path databaseArchivePath );

    using SymbolTable = std::unordered_map< std::string, TypeID >;
    void getConcreteObjectSymbols( SubType objectConcreteID, SymbolTable& symbols, SymbolTable& links );
    void getLinkObjectTypes( SubType concreteObjectID, TypeID linkInterfaceTypeID,
                             std::set< SubType >& objectTypes );

    using ObjectTypesMap = std::map< std::string, SubType >;
    ObjectTypesMap getObjectTypes();

private:
    io::ArchiveEnvironment            m_environment;
    io::Manifest                      m_manifest;
    FinalStage::Database              m_database;
    FinalStage::Symbols::SymbolTable* m_pSymbolTable;
    SymbolTypeIDMap                   m_symbolTypeIDs;
    InterfaceTypeIDMap                m_interfaceTypeIDs;
    ConcreteTypeIDMap                 m_concreteTypeIDs;
};*/
} // namespace mega::runtime

#endif // GUARD_2023_September_06_python_database
