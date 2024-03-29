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

#include "report/url.hpp"

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

Report reportDataType( Parser::TypeDecl::Data* pDataType )
{
    using namespace ParserStage::Parser;
    using namespace std::string_literals;

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

            if( db_cast< Type::Absolute >( pFragment ) )
            {
                container.m_elements.push_back( Multiline{ { "Absolute: "s, osPath.str() } } );
            }
            else if( db_cast< Type::Deriving >( pFragment ) )
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

void recurse( Parser::Container* pContainer, Branch& branch )
{
    using namespace ParserStage::Parser;
    using namespace std::string_literals;

    Branch container;

    // for( pContainer->get_aliases() )
    //{
    // }
    //  pContainer->get_body_type_opt()

    for( auto p : pContainer->get_dimensions() )
    {
        // auto pType = p->get_data_type();

        Branch dim{ { "Dimension"s },
                    { Multiline{ { "Name: "s, symbolsToStr( p->get_symbols(), "." ) } },
                      Multiline{ { "Line: "s, std::to_string( p->get_line_number() ) } },
                      reportDataType( p->get_data_type() ) } };

        container.m_elements.emplace_back( std::move( dim ) );
    }
    // for( pContainer->get_links() )
    // {
    // }
    // for( pContainer->get_aliases() )
    // {
    // }
    // for( pContainer->get_usings() )
    // {
    // }
    // for( pContainer->get_includes() )
    // {
    // }
    // for( pContainer->get_parts() )
    // {
    // }

    // if( auto bodyOpt = pContainer->get_body_type_opt() )
    // {
    // }

    if( db_cast< Namespace >( pContainer ) )
    {
        container.m_label.emplace_back( "Namespace"s );
    }
    else if( db_cast< Abstract >( pContainer ) )
    {
        container.m_label.emplace_back( "Abstract"s );
    }
    else if( db_cast< Event >( pContainer ) )
    {
        container.m_label.emplace_back( "Event"s );
    }
    else if( db_cast< Interupt >( pContainer ) )
    {
        container.m_label.emplace_back( "Interupt"s );
    }
    else if( db_cast< Requirement >( pContainer ) )
    {
        container.m_label.emplace_back( "Requirement"s );
    }
    else if( db_cast< Function >( pContainer ) )
    {
        container.m_label.emplace_back( "Function"s );
    }
    else if( db_cast< Decider >( pContainer ) )
    {
        container.m_label.emplace_back( "Decider"s );
    }
    else if( db_cast< Object >( pContainer ) )
    {
        container.m_label.emplace_back( "Object"s );
    }
    else if( db_cast< Action >( pContainer ) )
    {
        container.m_label.emplace_back( "Action"s );
    }
    else if( db_cast< Component >( pContainer ) )
    {
        container.m_label.emplace_back( "Component"s );
    }
    else if( db_cast< State >( pContainer ) )
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

Report ParserReporter::generate( const URL& )
{
    using namespace std::string_literals;

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
