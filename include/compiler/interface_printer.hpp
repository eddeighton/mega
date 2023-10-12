
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

#ifndef GUARD_2023_September_21_printer
#define GUARD_2023_September_21_printer

// ensure database stage namespace defined i.e. namespace GlobalMemoryStage
namespace Interface
{

static const std::string& getIdentifier( IContext* pContext )
{
    return pContext->get_identifier();
}
static const std::string& getIdentifier( DimensionTrait* pDim )
{
    return pDim->get_id()->get_str();
}
static const std::string& getIdentifier( LinkTrait* pLinkTrait )
{
    return pLinkTrait->get_id()->get_str();
}

static std::string printIContextFullType( IContext* pContext, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    using IContextVector = std::vector< IContext* >;
    IContextVector path;
    while( pContext )
    {
        path.push_back( pContext );
        pContext = db_cast< IContext >( pContext->get_parent() );
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

static void printDimensionTraitFullType( DimensionTrait* pDim, std::ostream& os,
                                         const std::string& strDelimiter = "::" )
{
    auto pParent = db_cast< IContext >( pDim->get_parent() );
    VERIFY_RTE( pParent );
    os << printIContextFullType( pParent, strDelimiter ) << strDelimiter << pDim->get_id()->get_str();
}
static std::string printDimensionTraitFullType( DimensionTrait* pDim, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printDimensionTraitFullType( pDim, os, strDelimiter );
    return os.str();
}

static void printLinkTraitFullType( LinkTrait* pLink, std::ostream& os, const std::string& strDelimiter = "::" )
{
    auto pParent = db_cast< IContext >( pLink->get_parent() );
    VERIFY_RTE( pParent );
    os << printIContextFullType( pParent, strDelimiter ) << strDelimiter << pLink->get_id()->get_str();
}
static std::string printLinkTraitFullType( LinkTrait* pLink, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    printLinkTraitFullType( pLink, os, strDelimiter );
    return os.str();
}

static void printContextType( std::vector< IContext* >& contexts, std::ostream& os )
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
        for( IContext* pContext : contexts )
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

static std::string printLinkTraitTypePath( const LinkTrait* pLinkTrait )
{
    std::ostringstream os;

    os << pLinkTrait->get_interface_id() << " ";

    for( auto pTypePathVariant : pLinkTrait->get_tuple() )
    {
        os << "(";
        bool bFirstTypePath = true;
        for( auto pTypePath : pTypePathVariant->get_sequence() )
        {
            if( bFirstTypePath )
                bFirstTypePath = false;
            else
                os << ",";

            bool bFirstSymbol = true;
            for( auto pSymbol : pTypePath->get_types() )
            {
                if( bFirstSymbol )
                    bFirstSymbol = false;
                else
                    os << ".";
                os << pSymbol->get_symbol();
            }
        }
        os << ")";
    }

    return os.str();
}

} // namespace Interface

#endif // GUARD_2023_September_21_printer
