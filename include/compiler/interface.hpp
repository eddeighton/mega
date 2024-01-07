
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

std::vector< Aggregate* > getAggregates( NodeGroup* pNodeGroup )
{
    std::vector< Aggregate* > aggregates;
    for( auto p : pNodeGroup->get_children() )
    {
        if( auto pAgg = db_cast< Aggregate >( p ) )
        {
            aggregates.push_back( pAgg );
        }
    }
    return aggregates;
}

void getNodes( NodeGroup* pNodeGroup, std::vector< IContext* >& contexts, std::vector< Aggregate* >& aggregates )
{
    for( auto p : pNodeGroup->get_children() )
    {
        if( auto pAgg = db_cast< Aggregate >( p ) )
        {
            aggregates.push_back( pAgg );
        }
        else if( auto pContext = db_cast< IContext >( p ) )
        {
            contexts.push_back( pContext );
        }
        else
        {
            THROW_RTE( "Unknown NodeGroup Type" );
        }
    }
}

template< typename T >
static const std::string& getKind();

#define MAKE_KIND_GETTER( TypeName ) \
template<>\
inline const std::string& getKind< TypeName >()\
{\
    static const std::string s = #TypeName;\
    return s;\
}

MAKE_KIND_GETTER( Aggregate )
MAKE_KIND_GETTER( Namespace )
MAKE_KIND_GETTER( Abstract )
MAKE_KIND_GETTER( Event )
MAKE_KIND_GETTER( Object )
MAKE_KIND_GETTER( Interupt )
MAKE_KIND_GETTER( Requirement )
MAKE_KIND_GETTER( Decider )
MAKE_KIND_GETTER( Function )
MAKE_KIND_GETTER( Action )
MAKE_KIND_GETTER( Component )
MAKE_KIND_GETTER( State )
MAKE_KIND_GETTER( InvocationContext )
MAKE_KIND_GETTER( IContext )

static const std::string& getKind( const Node* pNode )
{
    using namespace std::string_literals;
    if( auto* p = db_cast< Aggregate >( pNode ) )
    {
        return getKind< Aggregate >();
    }
    else if( auto* p = db_cast< Namespace >( pNode ) )
    {
        return getKind< Namespace >();
    }
    else if( auto* p = db_cast< Abstract >( pNode ) )
    {
        return getKind< Abstract >();
    }
    else if( auto* p = db_cast< Event >( pNode ) )
    {
        return getKind< Event >();
    }
    else if( auto* p = db_cast< Object >( pNode ) )
    {
        return getKind< Object >();
    }
    else if( auto* p = db_cast< Interupt >( pNode ) )
    {
        return getKind< Interupt >();
    }
    else if( auto* p = db_cast< Requirement >( pNode ) )
    {
        return getKind< Requirement >();
    }
    else if( auto* p = db_cast< Decider >( pNode ) )
    {
        return getKind< Decider >();
    }
    else if( auto* p = db_cast< Function >( pNode ) )
    {
        return getKind< Function >();
    }
    else if( auto* p = db_cast< Action >( pNode ) )
    {
        return getKind< Action >();
    }
    else if( auto* p = db_cast< Component >( pNode ) )
    {
        return getKind< Component >();
    }
    else if( auto* p = db_cast< State >( pNode ) )
    {
        return getKind< State >();
    }
    else if( auto* p = db_cast< InvocationContext >( pNode ) )
    {
        return getKind< InvocationContext >();
    }
    else if( auto* p = db_cast< IContext >( pNode ) )
    {
        return getKind< IContext >();
    }
    else
    {
        THROW_RTE( "Unknown interface node type" );
    }
}

} // namespace Interface