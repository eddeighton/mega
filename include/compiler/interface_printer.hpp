
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

static const std::string& getKind( const Node* pNode )
{
    using namespace std::string_literals;
    if( auto* p = db_cast< ParsedAggregate >( pNode ) )
    {
        static const std::string s = "ParsedAggregate"s;
        return s;
    }
    else if( auto* p = db_cast< GeneratedAggregate >( pNode ) )
    {
        static const std::string s = "GeneratedAggregate"s;
        return s;
    }
    else if( auto* p = db_cast< OwnershipLink >( pNode ) )
    {
        static const std::string s = "OwnershipLink"s;
        return s;
    }
    else if( auto* p = db_cast< ActivationBitSet >( pNode ) )
    {
        static const std::string s = "ActivationBitSet"s;
        return s;
    }
    else if( auto* p = db_cast< Aggregate >( pNode ) )
    {
        static const std::string s = "Aggregate"s;
        return s;
    }
    else if( auto* p = db_cast< Namespace >( pNode ) )
    {
        static const std::string s = "Namespace"s;
        return s;
    }
    else if( auto* p = db_cast< Abstract >( pNode ) )
    {
        static const std::string s = "Abstract"s;
        return s;
    }
    else if( auto* p = db_cast< Event >( pNode ) )
    {
        static const std::string s = "Event"s;
        return s;
    }
    else if( auto* p = db_cast< Object >( pNode ) )
    {
        static const std::string s = "Object"s;
        return s;
    }
    else if( auto* p = db_cast< Interupt >( pNode ) )
    {
        static const std::string s = "Interupt"s;
        return s;
    }
    else if( auto* p = db_cast< Requirement >( pNode ) )
    {
        static const std::string s = "Requirement"s;
        return s;
    }
    else if( auto* p = db_cast< Decider >( pNode ) )
    {
        static const std::string s = "Decider"s;
        return s;
    }
    else if( auto* p = db_cast< Function >( pNode ) )
    {
        static const std::string s = "Function"s;
        return s;
    }
    else if( auto* p = db_cast< Action >( pNode ) )
    {
        static const std::string s = "Action"s;
        return s;
    }
    else if( auto* p = db_cast< Component >( pNode ) )
    {
        static const std::string s = "Component"s;
        return s;
    }
    else if( auto* p = db_cast< State >( pNode ) )
    {
        static const std::string s = "State"s;
        return s;
    }
    else if( auto* p = db_cast< InvocationContext >( pNode ) )
    {
        static const std::string s = "InvocationContext"s;
        return s;
    }
    else if( auto* p = db_cast< IContext >( pNode ) )
    {
        static const std::string s = "IContext"s;
        return s;
    }
    else
    {
        THROW_RTE( "Unknown interface node type" );
    }
}

static const std::string& getIdentifier( const Node* pNode )
{
    return pNode->get_symbol()->get_token();
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
/*
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
}*/

} // namespace Interface
