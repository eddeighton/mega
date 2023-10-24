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

#include "reporters.hpp"

#include "database_reporters/factory.hpp"

#include "environment/environment_archive.hpp"
#include "database/FinalStage.hxx"

#include "reports/renderer_html.hpp"

#include "mega/values/service/url.hpp"
#include "mega/values/service/project.hpp"
#include "mega/values/compilation/type_id.hpp"

#include "mega/mangle/traits.hpp"
#include "mega/common_strings.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace FinalStage
{
#include "compiler/interface_printer.hpp"
#include "compiler/concrete_printer.hpp"
#include "compiler/concrete.hpp"
} // namespace FinalStage

namespace mega::reporters
{

const mega::reports::ReporterID SymbolsReporter::ID = "Symbols";

SymbolsReporter::SymbolsReporter( mega::io::Environment& environment, FinalStage::Database& database )
    : m_environment( environment )
    , m_database( database )
{
}

mega::reports::Container SymbolsReporter::generate( const mega::reports::URL& url )
{
    using namespace FinalStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch{ { "(S)ymbols"s } };

    auto pSymbolTable = m_database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

    {
        Table symbols {
            {
                "Symbol ID"s, "Name"s
            }
        };
        for( auto [ symbolID, pSymbolID ] : pSymbolTable->get_symbol_type_ids() )
        {
            symbols.m_rows.push_back( ContainerVector{
                Line{ symbolID, std::nullopt, symbolID }, 
                Line{ pSymbolID->get_symbol() }

            } );
        }
        branch.m_elements.push_back( std::move( symbols ) );
    }

    return branch;
}

const mega::reports::ReporterID InterfaceTypeIDReporter::ID = "Interface";

InterfaceTypeIDReporter::InterfaceTypeIDReporter( mega::io::Environment& environment, FinalStage::Database& database )
    : m_environment( environment )
    , m_database( database )
{
}

mega::reports::Container InterfaceTypeIDReporter::generate( const mega::reports::URL& url )
{
    using namespace FinalStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch{ { "(I)nterface"s } };

    auto pSymbolTable = m_database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

    {
        Table interface {
            {
                "Interface Type ID"s, "Full Type Name"s, "Symbol IDs"s
            }
        };
        for( auto [ typeID, pInterfaceTypeID ] : pSymbolTable->get_interface_type_ids() )
        {
            if( pInterfaceTypeID->get_context().has_value() )
            {
                // clang-format off
                interface.m_rows.push_back( ContainerVector{
                    Line{ typeID, std::nullopt, typeID }, 
                    Line{ Interface::printIContextFullType( pInterfaceTypeID->get_context().value() ) },
                    Line{ pInterfaceTypeID->get_symbol_ids() }

                } );
                // clang-format on
            }
            else if( pInterfaceTypeID->get_dimension().has_value() )
            {
                Interface::DimensionTrait* pDimension = pInterfaceTypeID->get_dimension().value();
                // clang-format off
                interface.m_rows.push_back( 
                    ContainerVector{
                        Line{ typeID, std::nullopt, typeID }, 
                        Line{ Interface::printDimensionTraitFullType( pDimension ) },
                        Line{ pInterfaceTypeID->get_symbol_ids() }
                    } );
                // clang-format on
            }
            else if( pInterfaceTypeID->get_link().has_value() )
            {
                Interface::LinkTrait* pLink = pInterfaceTypeID->get_link().value();
                // clang-format off
                interface.m_rows.push_back( ContainerVector{ 
                    Line{ typeID, std::nullopt, typeID },
                    Line{ Interface::printLinkTraitFullType( pLink ) },
                    Line{ pInterfaceTypeID->get_symbol_ids() } } );
                // clang-format on
            }
            else
            {
                THROW_RTE( "Interface TypeID: " << typeID << " has no context or dimension" );
            }
        }
        branch.m_elements.push_back( std::move( interface ) );
    }

    return branch;
}

const mega::reports::ReporterID ConcreteTypeIDReporter::ID = "Concrete";

ConcreteTypeIDReporter::ConcreteTypeIDReporter( mega::io::Environment& environment, FinalStage::Database& database )
    : m_environment( environment )
    , m_database( database )
{
}

mega::reports::Container ConcreteTypeIDReporter::generate( const mega::reports::URL& url )
{
    using namespace FinalStage;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch branch{ { "(C)oncrete"s } };

    auto pSymbolTable = m_database.one< Symbols::SymbolTable >( m_environment.project_manifest() );

    {
        Table concrete{ { "Concrete Type ID"s, "Interface Type ID"s, "Full Type Name"s, "Component"s } };
        for( auto [ typeID, pConcreteID ] : pSymbolTable->get_concrete_type_ids() )
        {
            auto pVertex = pConcreteID->get_vertex();

            // clang-format off
            concrete.m_rows.push_back( ContainerVector{
                Line{ typeID, std::nullopt, typeID }, 
                Line{ Concrete::getConcreteInterfaceTypeID( pVertex ) },
                Line{ Concrete::printContextFullType( pVertex ) }, 
                Line{ pVertex->get_component()->get_name() } }

            );
            // clang-format on
        }
        branch.m_elements.push_back( std::move( concrete ) );
    }
    
    return branch;
}
} // namespace mega::reporters
