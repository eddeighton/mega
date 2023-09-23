
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

std::string printIContextFullType( Interface::IContext* pContext )
{
    std::ostringstream os;
    using IContextVector = std::vector< Interface::IContext* >;
    IContextVector path;
    while( pContext )
    {
        path.push_back( pContext );
        pContext = db_cast< Interface::IContext >( pContext->get_parent() );
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
            os << ( *i )->get_identifier() << "::";
        }
    }
    return os.str();
}

void printDimensionTraitFullType( Interface::DimensionTrait* pDim, std::ostream& os )
{
    auto pParent = db_cast< Interface::IContext >( pDim->get_parent() );
    VERIFY_RTE( pParent );
    os << printIContextFullType( pParent ) << "::" << pDim->get_id()->get_str();
}

void printLinkTraitFullType( Interface::LinkTrait* pLink, std::ostream& os )
{
    auto pParent = db_cast< Interface::IContext >( pLink->get_parent() );
    VERIFY_RTE( pParent );
    os << printIContextFullType( pParent ) << "::" << pLink->get_id()->get_str();
}

void printContextType( std::vector< Interface::IContext* >& contexts, std::ostream& os )
{
    VERIFY_RTE( !contexts.empty() );
    if( contexts.size() == 1 )
    {
        os << printIContextFullType( contexts.front() );
    }
    else
    {
        // from mega/common_strings.hpp
        // cannot include header here as file is intended to be included in stage namespace
        static const char* EG_VARIANT_TYPE = "__eg_variant";
        os << EG_VARIANT_TYPE << "< ";
        bool bFirst = true;
        for( Interface::IContext* pContext : contexts )
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

std::string printLinkTraitTypePath( const Interface::TypedLinkTrait* pLinkTrait )
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
#endif //GUARD_2023_September_21_printer
