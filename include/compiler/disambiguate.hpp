
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

enum Disambiguation
{
    eSuccess,
    eFailure,
    eAmbiguous
};

static Disambiguation disambiguate( Derivation::Step* pStep, const std::vector< Derivation::Or* >& finalFrontier );

static Disambiguation inclusive( Derivation::Step* pStep, const std::vector< Derivation::Or* >& finalFrontier,
                                 std::optional< Disambiguation >& result )
{
    const Disambiguation stepResult = disambiguate( pStep, finalFrontier );
    switch( stepResult )
    {
        case eSuccess:
        {
            if( result.has_value() )
            {
                switch( result.value() )
                {
                    case eSuccess:
                        result = eSuccess;
                        break;
                    case eFailure:
                        result = eFailure;
                        break;
                    case eAmbiguous:
                        result = eAmbiguous;
                        break;
                }
            }
            else
            {
                result = eSuccess;
            }
        }
        break;
        case eFailure:
        {
            result = eFailure;
        }
        break;
        case eAmbiguous:
        {
            if( result.has_value() )
            {
                switch( result.value() )
                {
                    case eSuccess:
                        result = eAmbiguous;
                        break;
                    case eFailure:
                        result = eFailure;
                        break;
                    case eAmbiguous:
                        result = eAmbiguous;
                        break;
                }
            }
            else
            {
                result = eAmbiguous;
            }
        }
        break;
    }
    return stepResult;
}

static Disambiguation exclusive( Derivation::Step* pStep, const std::vector< Derivation::Or* >& finalFrontier,
                                 std::optional< Disambiguation >& result )
{
    const Disambiguation stepResult = disambiguate( pStep, finalFrontier );
    switch( stepResult )
    {
        case eSuccess:
        {
            if( result.has_value() )
            {
                switch( result.value() )
                {
                    case eSuccess:
                        result = eAmbiguous;
                        break;
                    case eFailure:
                        result = eSuccess;
                        break;
                    case eAmbiguous:
                        result = eAmbiguous;
                        break;
                }
            }
            else
            {
                result = eSuccess;
            }
        }
        break;
        case eFailure:
        {
            if( result.has_value() )
            {
                switch( result.value() )
                {
                    case eSuccess:
                        result = eSuccess;
                        break;
                    case eFailure:
                        result = eFailure;
                        break;
                    case eAmbiguous:
                        result = eAmbiguous;
                        break;
                }
            }
            else
            {
                result = eFailure;
            }
        }
        break;
        case eAmbiguous:
        {
            result = eAmbiguous;
        }
        break;
    }
    return stepResult;
}

static Disambiguation disambiguate( Derivation::Step* pStep, const std::vector< Derivation::Or* >& finalFrontier )
{
    std::optional< Disambiguation > result;

    if( db_cast< Derivation::And >( pStep ) )
    {
        for( auto pEdge : pStep->get_edges() )
        {
            inclusive( pEdge->get_next(), finalFrontier, result );
        }
    }
    else if( Derivation::Or* pOr = db_cast< Derivation::Or >( pStep ) )
    {
        // if the OR is in final frontier then have reached the end
        if( std::find( finalFrontier.begin(), finalFrontier.end(), pOr ) != finalFrontier.end() )
        {
            ASSERT( pStep->get_edges().empty() );
            return eSuccess;
        }
        else
        {
            // otherwise we are within the tree
            ASSERT( !pStep->get_edges().empty() );
        }

        // first test all edges
        {
            auto tempResult = result;
            for( auto pEdge : pStep->get_edges() )
            {
                /*bool bContainsInvalidEdge = false;
                for( auto pGraphEdge : pEdge->get_edges() )
                {
                    switch( pGraphEdge->get_type().get() )
                    {
                        case ::mega::EdgeType::eChildNonSingular:
                             bContainsInvalidEdge = true;
                             break;
                        case ::mega::EdgeType::eChildSingular:
                        case ::mega::EdgeType::eDim:
                        case ::mega::EdgeType::eLink:
                        case ::mega::EdgeType::eParent:

                        case mega::EdgeType::eInterObjectNonOwner:
                        case mega::EdgeType::eInterObjectOwner:
                        case mega::EdgeType::eInterObjectParent:

                            break;
                        case ::mega::EdgeType::TOTAL_EDGE_TYPES:
                        default:
                            THROW_RTE( "Unknown hyper graph edge type" );
                            break;
                    }
                }
                if( bContainsInvalidEdge )
                {
                    pEdge->set_eliminated( true );
                }
                else*/
                {
                    const auto EdgeResult = exclusive( pEdge->get_next(), finalFrontier, tempResult );
                    if( EdgeResult == eFailure )
                    {
                        pEdge->set_eliminated( true );
                    }
                }
            }
        }

        // now determine highest precedence for any successful edges
        int iHighestPrecedence = std::numeric_limits< int >::lowest();
        for( auto pEdge : pStep->get_edges() )
        {
            if( !pEdge->get_eliminated() )
            {
                iHighestPrecedence = std::max( iHighestPrecedence, pEdge->get_precedence() );
            }
        }

        std::vector< Derivation::Edge* > edges;
        for( auto pEdge : pStep->get_edges() )
        {
            if( pEdge->get_precedence() == iHighestPrecedence )
            {
                edges.push_back( pEdge );
            }
            else
            {
                pEdge->set_eliminated( true );
            }
        }

        for( auto pEdge : edges )
        {
            const auto EdgeResult = exclusive( pEdge->get_next(), finalFrontier, result );
            if( EdgeResult == eFailure )
            {
                pEdge->set_eliminated( true );
            }
        }
    }
    else
    {
        THROW_RTE( "Unknown step type" );
    }

    if( !result.has_value() )
    {
        return eFailure;
    }
    else
    {
        return result.value();
    }
}

static Disambiguation disambiguate( Derivation::Root* pStep, const std::vector< Derivation::Or* >& finalFrontier )
{
    std::optional< Disambiguation > result;

    for( auto pEdge : pStep->get_edges() )
    {
        inclusive( pEdge->get_next(), finalFrontier, result );
    }

    if( !result.has_value() )
    {
        return eFailure;
    }
    else
    {
        return result.value();
    }
}

static void precedence( Derivation::Edge* pEdge )
{
    {
        auto edges = pEdge->get_edges();
        if( edges.size() == 1 )
        {
            auto pGraphEdge = edges.front();
            switch( pGraphEdge->get_type().get() )
            {
                case ::mega::EdgeType::eChildSingular:
                case ::mega::EdgeType::eChildNonSingular:
                case ::mega::EdgeType::eDim:
                    pEdge->set_precedence( 1 );
                    break;
                case ::mega::EdgeType::eLink:
                    // do not give ownership link precedence
                    if( !db_cast< Concrete::Data::OwnershipLink >( pGraphEdge->get_target() ) )
                    {
                        pEdge->set_precedence( 1 );
                    }
                    break;
                case ::mega::EdgeType::eParent:
                    break;
                case mega::EdgeType::eInterObjectNonOwner:
                case mega::EdgeType::eInterObjectOwner:
                case mega::EdgeType::eInterObjectParent:
                    break;
                case ::mega::EdgeType::TOTAL_EDGE_TYPES:
                default:
                    THROW_RTE( "Unknown hyper graph edge type" );
                    break;
            }
        }
        else
        {
            for( auto pGraphEdge : edges )
            {
                switch( pGraphEdge->get_type().get() )
                {
                    // punish parent derivation steps
                    case ::mega::EdgeType::eParent:
                    {
                        pEdge->set_precedence( pEdge->get_precedence() - 1 );
                    }
                    break;
                    default:
                    {
                        // do nothing
                    }
                    break;
                }
            }
        }
    }
    for( auto pNextEdge : pEdge->get_next()->get_edges() )
    {
        precedence( pNextEdge );
    }
}

static void precedence( Derivation::Root* pStep )
{
    for( auto pEdge : pStep->get_edges() )
    {
        precedence( pEdge );
    }
}
