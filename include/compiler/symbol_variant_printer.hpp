
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

namespace Interface
{

static void printSymbolVariantSequence( const SymbolVariantSequence* pSymbolVariantSequence, std::ostream& os )
{
    bool bFirstVariant = true;
    for( auto pSymbolVariant : pSymbolVariantSequence->get_variants() )
    {
        if( bFirstVariant )
            bFirstVariant = false;
        else
            os << ".";

        const auto symbols = pSymbolVariant->get_symbols();
        if( symbols.size() == 1 )
        {
            os << symbols.front()->get_symbol();
        }
        else
        {
            os << "<";
            bool bFirstSymbol = true;
            for( auto pSymbol : symbols )
            {
                if( bFirstSymbol )
                    bFirstSymbol = false;
                else
                    os << ".";
                os << pSymbol->get_symbol();
            }
            os << ">";
        }
    }
}

static std::string printSymbolVariantSequence( const SymbolVariantSequence* pSymbolVariantSequence )
{
    std::ostringstream os;
    printSymbolVariantSequence( pSymbolVariantSequence, os );
    return os.str();
}

static void printSymbolVariantSequences( const std::vector< SymbolVariantSequence* >& symbolVariantSequences, std::ostream& os )
{
    os << "(";
    bool bFirstSequence = true;
    for( auto pSymbolVariantSequence : symbolVariantSequences )
    {
        if( bFirstSequence )
            bFirstSequence = false;
        else
            os << ",";
        printSymbolVariantSequence( pSymbolVariantSequence, os );
    }
    os << ")";
}

static std::string printSymbolVariantSequences( const std::vector< SymbolVariantSequence* >& symbolVariantSequences )
{
    std::ostringstream os;
    printSymbolVariantSequences( symbolVariantSequences, os );
    return os.str();
}

static std::string printLinkTraitTypePath( const LinkTrait* pLinkTrait )
{
    std::ostringstream os;

    os << pLinkTrait->get_interface_id() << " ";

    if( auto pUserLinkTrait = db_cast< Interface::UserLinkTrait >( pLinkTrait ) )
    {
        Interface::printSymbolVariantSequences( pUserLinkTrait->get_symbol_variant_sequences(), os );
    }
    else if( auto pOwnershipLinkTrait = db_cast< Interface::OwnershipLinkTrait >( pLinkTrait ) )
    {
        os << "(Ownership)";
    }

    return os.str();
}

} // namespace Interface