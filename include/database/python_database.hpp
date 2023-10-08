
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

#include "database/common/api.hpp"
#include "database/common/environment_archive.hpp"

#include "database/model/FinalStage.hxx"
#include "database/model/manifest.hxx"

#include <map>

namespace mega::runtime
{

class EGDB_EXPORT PythonDatabase
{
    using InterfaceTypeIDMap = std::map< mega::TypeID, ::FinalStage::Symbols::InterfaceTypeID* >;
    using ConcreteTypeIDMap  = std::map< TypeID, ::FinalStage::Symbols::ConcreteTypeID* >;

public:
    PythonDatabase( const boost::filesystem::path& projectDatabasePath );

    using SymbolTable = std::unordered_map< std::string, TypeID >;
    void getObjectSymbols( TypeID::SubValueType objectConcreteID, SymbolTable& symbols, SymbolTable& links );
    void getLinkObjectTypes( TypeID linkInterfaceTypeID, std::set< TypeID::SubValueType >& objectTypes );

private:
    io::ArchiveEnvironment            m_environment;
    io::Manifest                      m_manifest;
    FinalStage::Database              m_database;
    FinalStage::Symbols::SymbolTable* m_pSymbolTable;
    InterfaceTypeIDMap                m_interfaceTypeIDs;
    ConcreteTypeIDMap                 m_concreteTypeIDs;
};
} // namespace mega::runtime

#endif // GUARD_2023_September_06_python_database
