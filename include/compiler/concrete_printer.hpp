
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

static const std::string getIdentifier( const Node* pNode )
{
    return pNode->get_node()->get_symbol()->get_token();
}

static std::string fullTypeName( const Node* pContext, const std::string& strDelimiter = "::" )
{
    std::ostringstream os;
    using NodeVector = std::vector< const Node* >;
    NodeVector path;
    while( pContext )
    {
        path.push_back( pContext );
        pContext = db_cast< const Node >( pContext->get_parent() );
    }
    std::reverse( path.begin(), path.end() );
    for( auto i = path.begin(), iNext = path.begin(), iEnd = path.end(); i != iEnd; ++i )
    {
        ++iNext;
        if( iNext == iEnd )
        {
            os << getIdentifier( *i );
        }
        else
        {
            os << getIdentifier( *i ) << strDelimiter;
        }
    }
    return os.str();
}

} // namespace Concrete
