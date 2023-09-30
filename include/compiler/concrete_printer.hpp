
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

#ifndef GUARD_2023_September_27_concrete_printer
#define GUARD_2023_September_27_concrete_printer

static std::string printContextFullType( Concrete::Context* pContext, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    using ContextVector = std::vector< Concrete::Context* >;
    ContextVector path;
    while( pContext )
    {
        path.push_back( pContext );
        pContext = db_cast< Concrete::Context >( pContext->get_parent() );
    }
    std::reverse( path.begin(), path.end() );
    for( auto i = path.begin(), iNext = path.begin(), iEnd = path.end(); i != iEnd; ++i )
    {
        ++iNext;
        if( iNext == iEnd )
        {
            os << ( *i )->get_interface()->get_identifier();
        }
        else
        {
            os << ( *i )->get_interface()->get_identifier() << strDelimiter;
        }
    }
    return os.str();
}

static void printConcreteFullType( Concrete::Dimensions::User* pDim, std::ostream& os, const std::string& strDelimiter = "::" )
{
    auto pParent = db_cast< Concrete::Context >( pDim->get_parent_context() );
    VERIFY_RTE( pParent );
    os << printContextFullType( pParent ) << strDelimiter << pDim->get_interface_dimension()->get_id()->get_str();
}

static void printConcreteFullType( Concrete::Dimensions::Link* pLink, std::ostream& os, const std::string& strDelimiter = "::" )
{
    auto pParent = db_cast< Concrete::Context >( pLink->get_parent_context() );
    VERIFY_RTE( pParent );
    if( auto pUserLink = db_cast< Concrete::Dimensions::UserLink >( pLink ) )
    {
        os << printContextFullType( pParent ) << strDelimiter << pUserLink->get_interface_link()->get_id()->get_str();
    }
    else if( auto pOwningLink = db_cast< Concrete::Dimensions::OwnershipLink >( pLink ) )
    {
        os << printContextFullType( pParent ) << strDelimiter << ::mega::EG_OWNERSHIP;
    }
    else
    {
        THROW_RTE( "Unknown link type" );
    }
}

static void printConcreteFullType( Concrete::Graph::Vertex* pVertex, std::ostream& os, const std::string& strDelimiter = "::" )
{
    if( auto pContext = db_cast< Concrete::Context >( pVertex ) )
    {
        os << printContextFullType( pContext, strDelimiter );
    }
    else if( auto pLink = db_cast< Concrete::Dimensions::Link >( pVertex ) )
    {
        printConcreteFullType( pLink, os, strDelimiter );
    }
    else if( auto pDim = db_cast< Concrete::Dimensions::User >( pVertex ) )
    {
        printConcreteFullType( pDim, os, strDelimiter );
    }
    else
    {
        THROW_RTE( "Unknown vertex type" );
    }
}

#endif // GUARD_2023_September_27_concrete_printer
