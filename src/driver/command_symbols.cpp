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

namespace driver
{
namespace symbols
{
namespace
{

const std::string& getIdentifier( FinalStage::Interface::IContext* pContext )
{
    return pContext->get_identifier();
}
const std::string& getIdentifier( FinalStage::Concrete::Context* pContext )
{
    return pContext->get_interface()->get_identifier();
}
const std::string& getIdentifier( FinalStage::Concrete::Dimensions::User* pDim )
{
    return pDim->get_interface_dimension()->get_id()->get_str();
}
/*
const std::string& getIdentifier( FinalStage::Concrete::Dimensions::Link* pLink )
{
    return pLink->get_link()->get_link()->get_identifier();
}*/

template < typename TContextType >
std::string getContextFullTypeName( TContextType* pContext, std::string strDelim = "_" )
{
    using namespace FinalStage;

    std::vector< TContextType* > path;
    {
        while( pContext )
        {
            path.push_back( pContext );
            pContext = db_cast< TContextType >( pContext->get_parent() );
        }
        std::reverse( path.begin(), path.end() );
    }

    std::ostringstream os;
    {
        bool bFirst = true;
        for( TContextType* pIter : path )
        {
            if( !bFirst )
            {
                os << strDelim;
            }
            else
            {
                bFirst = false;
            }
            os << getIdentifier( pIter );
        }
    }

    return os.str();
}
} // namespace

void command( bool bHelp, const std::vector< std::string >& args )
{
    std::string             strGraphType;
    boost::filesystem::path databaseArchivePath, outputFilePath;
    bool                    bConcrete = false;

    namespace po = boost::program_options;
    po::options_description commandOptions( " Generate graph json data" );
    {
        // clang-format off
        commandOptions.add_options()
            ( "database",   po::value< boost::filesystem::path >( &databaseArchivePath ),               "Path to database archive" )
            ( "concrete",   po::bool_switch( &bConcrete ),                                              "Concrete symbols" )
            ;
        // clang-format on
    }

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

            if( bConcrete )
            {
                for( const auto& [ concreteTypeID, pConcreteTypeID ] : pSymbolTable->get_concrete_type_ids() )
                {
                    if( pConcreteTypeID->get_context().has_value() )
                    {
                        std::cout << concreteTypeID << " "
                                  << getContextFullTypeName( pConcreteTypeID->get_context().value() ) << "\n";
                    }
                    else if( pConcreteTypeID->get_dim_user().has_value() )
                    {
                        auto pDimension = pConcreteTypeID->get_dim_user().value();
                        std::cout << concreteTypeID << " " << getContextFullTypeName( pDimension->get_parent() )
                                  << "::" << getIdentifier( pDimension )
                                  << " type:" << pDimension->get_interface_dimension()->get_canonical_type() << "\n";
                    }
                    else if( pConcreteTypeID->get_dim_link().has_value() )
                    {
                        THROW_TODO;
                        /*auto pLink = pConcreteTypeID->get_dim_link().value();
                        std::cout << concreteTypeID << " " << getContextFullTypeName( pLink->get_parent() )
                                  << "::" << getIdentifier( pLink ) << "\n";*/
                    }
                    else if( pConcreteTypeID->get_dim_allocation().has_value() )
                    {
                        auto pAllocation = pConcreteTypeID->get_dim_allocation().value();
                        std::cout << concreteTypeID << " " << getContextFullTypeName( pAllocation->get_parent() )
                                  << "::_allocation_\n";
                    }
                    else
                    {
                        THROW_RTE( "Concrete TypeID: " << concreteTypeID << " has no context or dimension" );
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
                                  << getContextFullTypeName( pInterfaceTypeID->get_context().value() ) << "\n";
                    }
                    else if( pInterfaceTypeID->get_dimension().has_value() )
                    {
                        Interface::DimensionTrait* pDimension = pInterfaceTypeID->get_dimension().value();

                        std::cout << interfaceTypeID << " " << getContextFullTypeName( pDimension->get_parent() )
                                  << "::" << pDimension->get_id()->get_str()
                                  << " type:" << pDimension->get_canonical_type() << "\n";
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
