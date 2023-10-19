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

#include "database/mpo_database.hpp"

#include "mega/common_strings.hpp"

namespace FinalStage
{
#include "compiler/concrete_printer.hpp"
}

namespace mega::runtime
{

MPODatabase::MPODatabase( const boost::filesystem::path& projectDatabasePath )
    : m_environment( projectDatabasePath )
    , m_manifest( m_environment, m_environment.project_manifest() )
    , m_database( m_environment, m_manifest.getManifestFilePath() )
    , m_pSymbolTable( m_database.one< FinalStage::Symbols::SymbolTable >( m_manifest.getManifestFilePath() ) )
    , m_concreteTypeIDs( m_pSymbolTable->get_concrete_type_ids() )
    , m_interfaceTypeIDs( m_pSymbolTable->get_interface_type_ids() )
{
}

MPODatabase::MemoryMapping MPODatabase::getMemoryMappings()
{
    MPODatabase::MemoryMapping result;

    using namespace FinalStage;
    for( MemoryLayout::MemoryMap* pMemoryMap :
         m_database.many< MemoryLayout::MemoryMap >( m_environment.project_manifest() ) )
    {
        result.insert( { pMemoryMap->get_interface()->get_interface_id(), pMemoryMap } );
    }
    return result;
}
std::string MPODatabase::getConcreteFullType( TypeID typeID ) const
{
    if( typeID.valid() )
    {
        auto iFind = m_concreteTypeIDs.find( typeID );
        VERIFY_RTE_MSG( iFind != m_concreteTypeIDs.end(), "Failed to locate concrete type id: " << typeID );
        return FinalStage::Concrete::printContextFullType( iFind->second->get_vertex() );
    }
    else
    {
        return {};
    }
}
} // namespace mega::runtime