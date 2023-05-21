
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

#include "automata.hpp"

#include "common/assert_verify.hpp"

namespace mega::automata
{

using TokenStringArray = std::array< std::string, TOTAL_TOKENS >;

// clang-format off
static const TokenStringArray g_tokenStrings = { 
    std::string{ "}" },
    std::string{ "seq" },
    std::string{ "maybe" },
    std::string{ "or" },
    std::string{ "repeat" },
    std::string{ "interupt" }
};
// clang-format on

void tokenise( const std::string& str, TokenVector& tokens )
{
    auto iter         = str.cbegin();
    auto iterEnd      = str.cend();
    auto literalBegin = iter;

    while( iter != iterEnd )
    {
        const auto dist     = std::distance( iter, iterEnd );
        int        iCounter = 0;
        auto       iterNext = iter;
        for( const auto& tok : g_tokenStrings )
        {
            const auto tokenType = static_cast< TokenType >( iCounter );
            if( tok.size() <= dist )
            {
                if( std::equal( iter, iter + tok.size(), tok.cbegin() ) )
                {
                    bool bFound = false;
                    switch( tokenType )
                    {
                        case eBraceClose:
                        {
                            iterNext = iter + tok.size();
                            bFound   = true;
                        }
                        break;
                        default:
                        {
                            bool bConsumedOpenBrace = false;
                            auto iNext              = iter + tok.size();
                            while( iNext != iterEnd )
                            {
                                if( *iNext == '{' )
                                {
                                    ++iNext;
                                    bConsumedOpenBrace = true;
                                    break;
                                }
                                else if( std::isspace( *iNext ) )
                                {
                                    ++iNext;
                                }
                                else
                                {
                                    // comments???
                                    break;
                                }
                            }

                            if( bConsumedOpenBrace )
                            {
                                iterNext = iNext;
                                bFound   = true;
                            }
                        }
                        break;
                    }
                    if( bFound )
                    {
                        if( literalBegin != iter )
                        {
                            tokens.push_back( std::string_view{ literalBegin, iter } );
                        }
                        iter         = iterNext;
                        literalBegin = iterNext;
                        tokens.push_back( tokenType );
                        break;
                    }
                }
            }
            ++iCounter;
        }
        if( iCounter == TOTAL_TOKENS )
        {
            ++iter;
        }
    }
    if( literalBegin != iterEnd )
    {
        tokens.push_back( std::string_view{ literalBegin, iterEnd } );
    }
}

} // namespace mega::automata
