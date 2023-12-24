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

#include "database/ParserStage.hxx"

#include "mega/values/service/url.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace mega::reporters
{
using namespace ParserStage;

namespace
{

std::string symbolsToStr( const std::vector< Parser::Symbol* >& symbols, const char* pszDelim )
{
    std::ostringstream osName;
    bool               bFirst = true;
    for( auto pSymbol : symbols )
    {
        if( bFirst )
        {
            bFirst = false;
        }
        else
        {
            osName << pszDelim;
        }
        osName << pSymbol->get_token();
    }
    return osName.str();
}

mega::reports::Container reportDataType( Parser::TypeDecl::Data* pDataType )
{
    using namespace ParserStage::Parser;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch container{ { "DataType"s } };

    for( auto* pFragment : pDataType->get_cpp_fragments()->get_elements() )
    {
        if( auto pOpaque = db_cast< Type::CPPOpaque >( pFragment ) )
        {
            container.m_elements.push_back( Multiline{ { "Opaque: "s, pOpaque->get_str() } } );
        }
        else if( auto pPath = db_cast< Type::Path >( pFragment ) )
        {
            std::ostringstream osPath;
            {
                bool bFirst = true;
                for( auto pVariant : pPath->get_variants() )
                {
                    if( bFirst )
                    {
                        bFirst = false;
                    }
                    else
                    {
                        osPath << ".";
                    }
                    osPath << symbolsToStr( pVariant->get_symbols(), "|" );
                }
            }

            if( auto pAbsolute = db_cast< Type::Absolute >( pFragment ) )
            {
                container.m_elements.push_back( Multiline{ { "Absolute: "s, osPath.str() } } );
            }
            else if( auto pDeriving = db_cast< Type::Deriving >( pFragment ) )
            {
                container.m_elements.push_back( Multiline{ { "Deriving: "s, osPath.str() } } );
            }
            else
            {
                THROW_RTE( "Unknown mega type" );
            }
        }
        else
        {
            THROW_RTE( "Unknown cpp fragment type" );
        }
    }

    return container;
}

void recurse( Parser::Container* pContainer, reports::Branch& branch )
{
    using namespace ParserStage::Parser;
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch container;

    for( auto pAlias : pContainer->get_aliases() )
    {
    }
    // pContainer->get_body_type_opt()

    for( auto p : pContainer->get_dimensions() )
    {
        auto pType = p->get_data_type();

        Branch dim{ { "Dimension"s },
                    { Multiline{ { "Name: "s, symbolsToStr( p->get_symbols(), "." ) } },
                      Multiline{ { "Line: "s, std::to_string( p->get_line_number() ) } },
                      reportDataType( p->get_data_type() ) } };

        container.m_elements.emplace_back( std::move( dim ) );
    }
    for( auto p : pContainer->get_links() )
    {
    }
    for( auto p : pContainer->get_aliases() )
    {
    }
    for( auto p : pContainer->get_usings() )
    {
    }
    for( auto p : pContainer->get_includes() )
    {
    }
    for( auto p : pContainer->get_parts() )
    {
    }

    if( auto bodyOpt = pContainer->get_body_type_opt() )
    {
        
    }

    if( auto pNamespace = db_cast< Namespace >( pContainer ) )
    {
        container.m_label.emplace_back( "Namespace"s );
    }
    else if( auto pAbstract = db_cast< Abstract >( pContainer ) )
    {
        container.m_label.emplace_back( "Abstract"s );
    }
    else if( auto pEvent = db_cast< Event >( pContainer ) )
    {
        container.m_label.emplace_back( "Event"s );
    }
    else if( auto pInterupt = db_cast< Interupt >( pContainer ) )
    {
        container.m_label.emplace_back( "Interupt"s );
    }
    else if( auto pRequirement = db_cast< Requirement >( pContainer ) )
    {
        container.m_label.emplace_back( "Requirement"s );
    }
    else if( auto pFunction = db_cast< Function >( pContainer ) )
    {
        container.m_label.emplace_back( "Function"s );
    }
    else if( auto pDecider = db_cast< Decider >( pContainer ) )
    {
        container.m_label.emplace_back( "Decider"s );
    }
    else if( auto pObject = db_cast< Object >( pContainer ) )
    {
        container.m_label.emplace_back( "Object"s );
    }
    else if( auto pAction = db_cast< Action >( pContainer ) )
    {
        container.m_label.emplace_back( "Action"s );
    }
    else if( auto pComponent = db_cast< Component >( pContainer ) )
    {
        container.m_label.emplace_back( "Component"s );
    }
    else if( auto pState = db_cast< State >( pContainer ) )
    {
        container.m_label.emplace_back( "State"s );
    }
    else
    {
        // root IS normal container
        container.m_label.emplace_back( "Container"s );
    }

    for( auto pChild : pContainer->get_children() )
    {
        recurse( pChild, container );
    }

    branch.m_elements.emplace_back( container );
}

} // namespace

mega::reports::Container ParserReporter::generate( const mega::reports::URL& url )
{
    using namespace std::string_literals;
    using namespace mega::reports;

    Branch report{ { ID } };

    for( const mega::io::megaFilePath& sourceFilePath : m_args.manifest.getMegaSourceFiles() )
    {
        Database database( m_args.environment, sourceFilePath, true );

        Branch sourceBranch{ { sourceFilePath.path() } };

        auto* pSourceRoot = database.one< Parser::ObjectSourceRoot >( sourceFilePath );
        VERIFY_RTE( pSourceRoot );
        recurse( pSourceRoot->get_ast(), sourceBranch );

        report.m_elements.push_back( sourceBranch );
    }

    return report;
}

} // namespace mega::reporters
