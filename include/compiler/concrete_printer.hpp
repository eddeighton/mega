
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

static void printContextFullType( const Concrete::Context* pContext, std::ostream& os, const std::string& strDelimiter = "::" )
{
    using ContextVector = std::vector< const Concrete::Context* >;
    ContextVector path;
    while( pContext )
    {
        path.push_back( pContext );
        pContext = db_cast< const Concrete::Context >( pContext->get_parent() );
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
}

static void printContextFullType( const Concrete::Dimensions::User* pDim, std::ostream& os, const std::string& strDelimiter = "::" )
{
    auto pParent = db_cast< const Concrete::Context >( pDim->get_parent_context() );
    VERIFY_RTE( pParent );
    printContextFullType( pParent, os, strDelimiter );
    os << strDelimiter << pDim->get_interface_dimension()->get_id()->get_str();
}

static void printContextFullType( const Concrete::Dimensions::Link* pLink, std::ostream& os, const std::string& strDelimiter = "::" )
{
    auto pParent = db_cast< const Concrete::Context >( pLink->get_parent_context() );
    VERIFY_RTE( pParent );
    if( auto pUserLink = db_cast< const Concrete::Dimensions::UserLink >( pLink ) )
    {
        printContextFullType( pParent, os, strDelimiter );
        os << strDelimiter << pUserLink->get_interface_link()->get_id()->get_str();
    }
    else if( auto pOwningLink = db_cast< const Concrete::Dimensions::OwnershipLink >( pLink ) )
    {
        printContextFullType( pParent, os, strDelimiter );
        os << strDelimiter << ::mega::EG_OWNERSHIP;
    }
    else
    {
        THROW_RTE( "Unknown link type" );
    }
}

static std::string printContextFullType( const Concrete::Context* pContext, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printContextFullType( pContext, os, strDelimiter );
    return os.str();
}

static std::string printContextFullType( const Concrete::Dimensions::User* pDim, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printContextFullType( pDim, os, strDelimiter );
    return os.str();
}

static std::string printContextFullType( const Concrete::Dimensions::Link* pLink, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printContextFullType( pLink, os, strDelimiter );
    return os.str();
}

static void printContextFullType( const Concrete::Graph::Vertex* pVertex, std::ostream& os, const std::string& strDelimiter = "::" )
{
    if( auto pContext = db_cast< const Concrete::Context >( pVertex ) )
    {
        printContextFullType( pContext, os, strDelimiter );
    }
    else if( auto pLink = db_cast< const Concrete::Dimensions::Link >( pVertex ) )
    {
        printContextFullType( pLink, os, strDelimiter );
    }
    else if( auto pDim = db_cast< const Concrete::Dimensions::User >( pVertex ) )
    {
        printContextFullType( pDim, os, strDelimiter );
    }
    else
    {
        THROW_RTE( "Unknown vertex type" );
    }
}
static std::string printContextFullType( const Concrete::Graph::Vertex* pVertex, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printContextFullType( pVertex, os, strDelimiter );
    return os.str();
}

#endif // GUARD_2023_September_27_concrete_printer
