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

#include "database/common/archive.hpp"
#include "database/model/FinalStage.hxx"

#include "database/model/manifest.hxx"

#include "database/common/component_info.hpp"
#include "database/common/serialisation.hpp"
#include "database/common/environment_build.hpp"
#include "database/common/environment_archive.hpp"

#include "database/types/sources.hpp"
#include "utilities/cmake.hpp"

#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"
#include "common/stash.hpp"
#include "common/requireSemicolon.hpp"

#include "nlohmann/json.hpp"

#include <boost/process/environment.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>

namespace FinalStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
} // namespace FinalStage

namespace driver
{
namespace symbols
{

void command( bool bHelp, const std::vector< std::string >& args )
{
    std::string             strGraphType;
    boost::filesystem::path databaseArchivePath, outputFilePath;
    bool                    bConcrete = false;
    bool                    bSymbols  = false;

    namespace po = boost::program_options;
    po::options_description commandOptions(
        " Print TypeIDs.  Defaults to interface types but can show concrete or symbols" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "database",   po::value< boost::filesystem::path >( &databaseArchivePath ),               "Path to database archive" )
            ( "concrete",   po::bool_switch( &bConcrete ),                                              "Concrete TypeIDs" )
            ( "tokens",     po::bool_switch( &bSymbols ),                                               "Token Symbol TypeIDs" )
            ;
        // clang-format on
    }

    VERIFY_RTE_MSG( !bSymbols || !bConcrete, "Error - choose concrete OR symbols " );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
    po::notify( vm );

    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        std::ostringstream osOutput;
        {
            mega::io::ArchiveEnvironment environment( databaseArchivePath );
            using namespace FinalStage;
            Database database( environment );

            Symbols::SymbolTable* pSymbolTable = database.one< Symbols::SymbolTable >( environment.project_manifest() );

            if( bSymbols )
            {
                for( const auto& [ typeID, pSymbol ] : pSymbolTable->get_symbol_type_ids() )
                {
                    std::cout << std::setw( 8 ) << typeID << " " << pSymbol->get_symbol() << "\n";
                }
            }
            else if( bConcrete )
            {
                for( const auto& [ concreteTypeID, pConcreteTypeID ] : pSymbolTable->get_concrete_type_ids() )
                {
                    auto pVertex = pConcreteTypeID->get_vertex();
                    std::cout << concreteTypeID << " " <<  Concrete::printContextFullType( pVertex );
                    if( auto pContext = db_cast< Concrete::Context >( pVertex ) )
                    {
                        std::cout << "\n";
                    }
                    else if( auto pUser = db_cast< Concrete::Dimensions::User >( pVertex ) )
                    {
                        std::cout << " type:" << pUser->get_interface_dimension()->get_canonical_type() << "\n";
                    }
                    else if( auto pLink = db_cast< Concrete::Dimensions::Link >( pVertex ) )
                    {
                        std::cout << "\n";
                    }
                    else if( auto pBitset = db_cast< Concrete::Dimensions::Bitset >( pVertex ) )
                    {
                        std::cout << "\n";
                    }
                    else
                    {
                        THROW_RTE( "Unknown vertex type" );
                    }
                }
            }
            else
            {
                for( const auto& [ interfaceTypeID, pInterfaceTypeID ] : pSymbolTable->get_interface_type_ids() )
                {
                    if( pInterfaceTypeID->get_context().has_value() )
                    {
                        std::cout << interfaceTypeID << " "
                                  << Interface::printIContextFullType( pInterfaceTypeID->get_context().value() )
                                  << "\n";
                    }
                    else if( pInterfaceTypeID->get_dimension().has_value() )
                    {
                        Interface::DimensionTrait* pDimension = pInterfaceTypeID->get_dimension().value();
                        std::cout << interfaceTypeID << " " << Interface::printDimensionTraitFullType( pDimension )
                                  << " type:" << pDimension->get_canonical_type() << "\n";
                    }
                    else if( pInterfaceTypeID->get_link().has_value() )
                    {
                        Interface::LinkTrait* pLink = pInterfaceTypeID->get_link().value();
                        std::cout << interfaceTypeID << " " << Interface::printLinkTraitFullType( pLink ) << "\n";
                    }
                    else
                    {
                        THROW_RTE( "Interface TypeID: " << interfaceTypeID << " has no context or dimension" );
                    }
                }
            }
        }
    }
}
} // namespace symbols
} // namespace driver
