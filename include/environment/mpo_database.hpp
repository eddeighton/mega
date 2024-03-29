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

#ifndef GUARD_2023_April_04_mpo_database
#define GUARD_2023_April_04_mpo_database


#include "environment/environment_archive.hpp"

#include "database/manifest.hxx"

#include <unordered_map>

namespace mega::runtime
{

class MPODatabase
{
    // using InterfaceTypeIDMap = std::map< interface::TypeID, ::FinalStage::Symbols::interface::TypeID* >;
    // using ConcreteTypeIDMap  = std::map< concrete::TypeID, ::FinalStage::Symbols::concrete::TypeID* >;

public:
    MPODatabase( const boost::filesystem::path& projectDatabasePath );

    //using MemoryMapping = std::unordered_map< concrete::TypeID, FinalStage::MemoryLayout::MemoryMap*, concrete::TypeID::Hash >;
    //MemoryMapping getMemoryMappings();

    std::string getConcreteFullType( concrete::TypeID typeID ) const;

private:
    io::ArchiveEnvironment            m_environment;
    io::Manifest                      m_manifest;

    // FinalStage::Database              m_database;
    // FinalStage::Symbols::SymbolTable* m_pSymbolTable;
    // ConcreteTypeIDMap                 m_concreteTypeIDs;
    // InterfaceTypeIDMap                m_interfaceTypeIDs;
};

} // namespace mega::runtime

#endif // GUARD_2023_April_04_mpo_database
