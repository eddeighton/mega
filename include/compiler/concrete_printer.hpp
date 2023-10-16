
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

namespace Concrete
{

static const std::string& getIdentifier( const Context* pContext )
{
    return pContext->get_interface()->get_identifier();
}
static const std::string& getIdentifier( const Dimensions::User* pDim )
{
    if( auto pUserDimensionTrait = db_cast< Interface::UserDimensionTrait >( pDim->get_interface_dimension() ) )
    {
        return pUserDimensionTrait->get_parser_dimension()->get_id()->get_str();
    }
    else if( auto pCompilerDimensionTrait
             = db_cast< Interface::CompilerDimensionTrait >( pDim->get_interface_dimension() ) )
    {
        return pCompilerDimensionTrait->get_identifier();
    }
    else
    {
        THROW_RTE( "Unknown dimension trait type" );
    }
}
static const std::string& getIdentifier( const Dimensions::Link* pDim )
{
    if( auto pUserLink = db_cast< const Dimensions::UserLink >( pDim ) )
    {
        return pUserLink->get_interface_user_link()->get_parser_link()->get_id()->get_str();
    }
    else
    {
        static const std::string str = ::mega::EG_OWNER;
        return str;
    }
}
static const std::string& getIdentifier( const Dimensions::Bitset* pBitset )
{
    if( auto pCompilerDimensionTrait
             = db_cast< Interface::CompilerDimensionTrait >( pBitset->get_interface_compiler_dimension() ) )
    {
        return pCompilerDimensionTrait->get_identifier();
    }
    else
    {
        THROW_RTE( "Unknown bitset trait type" );
    }
}

static void printContextFullType( const Context* pContext, std::ostream& os, const std::string& strDelimiter = "::" )
{
    using ContextVector = std::vector< const Context* >;
    ContextVector path;
    while( pContext )
    {
        path.push_back( pContext );
        pContext = db_cast< const Context >( pContext->get_parent() );
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

static void printContextFullType( const Dimensions::User* pDim, std::ostream& os,
                                  const std::string& strDelimiter = "::" )
{
    auto pParent = db_cast< const Context >( pDim->get_parent_context() );
    VERIFY_RTE( pParent );
    printContextFullType( pParent, os, strDelimiter );
    os << strDelimiter << getIdentifier( pDim );
}

static void printContextFullType( const Dimensions::Link* pLink, std::ostream& os,
                                  const std::string& strDelimiter = "::" )
{
    auto pParent = db_cast< const Context >( pLink->get_parent_context() );
    VERIFY_RTE( pParent );
    printContextFullType( pParent, os, strDelimiter );
    os << strDelimiter << getIdentifier( pLink );
}

static void printContextFullType( const Dimensions::Bitset* pBitset, std::ostream& os,
                                  const std::string& strDelimiter = "::" )
{
    auto pParent = db_cast< const Context >( pBitset->get_parent_object() );
    VERIFY_RTE( pParent );
    printContextFullType( pParent, os, strDelimiter );
    os << strDelimiter << getIdentifier( pBitset );
}

static std::string printContextFullType( const Context* pContext, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printContextFullType( pContext, os, strDelimiter );
    return os.str();
}

static std::string printContextFullType( const Dimensions::User* pDim, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printContextFullType( pDim, os, strDelimiter );
    return os.str();
}

static std::string printContextFullType( const Dimensions::Link* pLink, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printContextFullType( pLink, os, strDelimiter );
    return os.str();
}

static std::string printContextFullType( const Dimensions::Bitset* pBitset, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printContextFullType( pBitset, os, strDelimiter );
    return os.str();
}

static void printContextFullType( const Graph::Vertex* pVertex, std::ostream& os,
                                  const std::string& strDelimiter = "::" )
{
    if( auto pContext = db_cast< const Context >( pVertex ) )
    {
        printContextFullType( pContext, os, strDelimiter );
    }
    else if( auto pContextGroup = db_cast< const ContextGroup >( pVertex ) )
    {
        THROW_RTE( "Cannot print context group that is NOT a context" );
    }
    else if( auto pLink = db_cast< const Dimensions::Link >( pVertex ) )
    {
        printContextFullType( pLink, os, strDelimiter );
    }
    else if( auto pDim = db_cast< const Dimensions::User >( pVertex ) )
    {
        printContextFullType( pDim, os, strDelimiter );
    }
    else if( auto pBitset = db_cast< const Dimensions::Bitset >( pVertex ) )
    {
        printContextFullType( pBitset, os, strDelimiter );
    }
    else
    {
        THROW_RTE( "Unknown vertex type" );
    }
}
static std::string printContextFullType( const Graph::Vertex* pVertex, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printContextFullType( pVertex, os, strDelimiter );
    return os.str();
}

} // namespace Concrete
