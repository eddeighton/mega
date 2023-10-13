
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

// ensure database stage namespace defined i.e. namespace GlobalMemoryStage
namespace Interface
{

static const std::string& getIdentifier( const IContext* pContext )
{
    return pContext->get_identifier();
}

static const std::string& getIdentifier( const DimensionTrait* pDim )
{
    if( auto pUserDimensionTrait = db_cast< Interface::UserDimensionTrait >( pDim ) )
    {
        return pUserDimensionTrait->get_parser_dimension()->get_id()->get_str();
    }
    else if( auto pCompilerDimensionTrait = db_cast< Interface::CompilerDimensionTrait >( pDim ) )
    {
        return pCompilerDimensionTrait->get_identifier();
    }
    else
    {
        THROW_RTE( "Unknown dimension trait type" );
    }
}

static const std::string& getIdentifier( const LinkTrait* pLinkTrait )
{
    if( auto pUserLink = db_cast< const UserLinkTrait >( pLinkTrait ) )
    {
        return pUserLink->get_parser_link()->get_id()->get_str();
    }
    else if( auto pOwnerLink = db_cast< const OwnershipLinkTrait >( pLinkTrait ) )
    {
        static const std::string strOwner = mega::EG_OWNER;
        return strOwner;
    }
    else
    {
        THROW_RTE( "Unknown link type" );
    }
}

static std::string printIContextFullType( const IContext* pContext, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    using IContextVector = std::vector< const IContext* >;
    IContextVector path;
    while( pContext )
    {
        path.push_back( pContext );
        pContext = db_cast< const IContext >( pContext->get_parent() );
    }
    std::reverse( path.begin(), path.end() );
    for( auto i = path.begin(), iNext = path.begin(), iEnd = path.end(); i != iEnd; ++i )
    {
        ++iNext;
        if( iNext == iEnd )
        {
            os << ( *i )->get_identifier();
        }
        else
        {
            os << ( *i )->get_identifier() << strDelimiter;
        }
    }
    return os.str();
}

static void printDimensionTraitFullType( const DimensionTrait* pDim, std::ostream& os,
                                         const std::string& strDelimiter = "::" )
{
    auto pParent = db_cast< const IContext >( pDim->get_parent() );
    VERIFY_RTE( pParent );
    os << printIContextFullType( pParent, strDelimiter ) << strDelimiter << getIdentifier( pDim );
}
static std::string printDimensionTraitFullType( const DimensionTrait* pDim, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printDimensionTraitFullType( pDim, os, strDelimiter );
    return os.str();
}

static void printLinkTraitFullType( const LinkTrait* pLink, std::ostream& os, const std::string& strDelimiter = "::" )
{
    auto pParent = db_cast< const IContext >( pLink->get_parent() );
    VERIFY_RTE( pParent );
    os << printIContextFullType( pParent, strDelimiter ) << strDelimiter << getIdentifier( pLink );
}
static std::string printLinkTraitFullType( const LinkTrait* pLink, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printLinkTraitFullType( pLink, os, strDelimiter );
    return os.str();
}

static void printContextType( std::vector< const IContext* >& contexts, std::ostream& os )
{
    VERIFY_RTE( !contexts.empty() );
    if( contexts.size() == 1 )
    {
        os << printIContextFullType( contexts.front(), "::" );
    }
    else
    {
        // from mega/common_strings.hpp
        // cannot include header here as file is intended to be included in stage namespace
        static const char* EG_VARIANT_TYPE = "__eg_variant";
        os << EG_VARIANT_TYPE << "< ";
        bool bFirst = true;
        for( auto pContext : contexts )
        {
            if( bFirst )
                bFirst = false;
            else
                os << ", ";
            os << printIContextFullType( pContext );
        }
        os << " >";
    }
}

} // namespace Interface
