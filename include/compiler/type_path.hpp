
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

#ifndef GUARD_2023_December_19_type_path
#define GUARD_2023_December_19_type_path

namespace Interface
{

using SymbolIDSequenceMap = std::map< ::mega::interface::SymbolIDSequence, ::ConcreteStage::Symbols::InterfaceTypeID* >;

static Interface::Node* resolve( const SymbolIDSequenceMap& idMap, Parser::Type::Absolute* pAbsolutePath )
{
    mega::interface::SymbolIDSequence symbolIDSeq;

    for( auto pVar : pAbsolutePath->get_variants() )
    {
        auto symbols = pVar->get_symbols();
        VERIFY_RTE_MSG( symbols.size() == 1, "Unsupported use of variant in absolute type" );
        auto pSymbol   = symbols.front();
        auto pSymbolID = pSymbol->get_id();
        symbolIDSeq.push_back( pSymbolID->get_id() );
    }

    auto iFind = idMap.find( symbolIDSeq );
    if( iFind != idMap.end() )
    {
        return iFind->second->get_node();
    }
    else
    {
        std::ostringstream os;

        bool bFirst = true;
        for( auto pVar : pAbsolutePath->get_variants() )
        {
            if( bFirst )
            {
                bFirst = false;
            }
            else
            {
                os << '.';
            }
            os << pVar->get_symbols().front()->get_token();
        }
        THROW_RTE( "Failed to locate absolute type: " << os.str() );
    }
    return {};
}

} // namespace Interface

#endif // GUARD_2023_December_19_type_path
