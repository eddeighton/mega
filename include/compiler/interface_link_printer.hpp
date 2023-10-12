
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

static std::string printLinkTraitTypePath( const LinkTrait* pLinkTrait )
{
    std::ostringstream os;

    os << pLinkTrait->get_interface_id() << " ";

    if( auto pUserLinkTrait = db_cast< Interface::UserLinkTrait >( pLinkTrait ) )
    {
        for( auto pTypePathVariant : pUserLinkTrait->get_tuple() )
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
    }
    else if( auto pOwnershipLinkTrait = db_cast< Interface::OwnershipLinkTrait >( pLinkTrait ) )
    {
        os << "(Ownership)";
    }

    return os.str();
}
} // namespace Interface

