
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

#ifndef GUARD_2023_September_26_disambiguate
#define GUARD_2023_September_26_disambiguate

enum Disambiguation
{
    eSuccess,
    eFailure,
    eAmbiguous
};

Disambiguation disambiguate( Step* pStep, const std::vector< Or* >& finalFrontier );

void inclusive( Step* pStep, const std::vector< Or* >& finalFrontier, std::optional< Disambiguation >& result )
{
    switch( disambiguate( pStep, finalFrontier ) )
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
}

void exclusive( Step* pStep, const std::vector< Or* >& finalFrontier, std::optional< Disambiguation >& result )
{
    switch( disambiguate( pStep, finalFrontier ) )
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
}

Disambiguation disambiguate( Step* pStep, const std::vector< Or* >& finalFrontier )
{
    std::optional< Disambiguation > result;

    if( And* pAnd = db_cast< And >( pStep ) )
    {
        for( auto pEdge : pStep->get_edges() )
        {
            inclusive( pEdge->get_next(), finalFrontier, result );
        }
    }
    else if( Or* pOr = db_cast< Or >( pStep ) )
    {
        if( std::find( finalFrontier.begin(), finalFrontier.end(), pOr ) != finalFrontier.end() )
        {
            ASSERT( pStep->get_edges().empty() );
            return eSuccess;
        }

        for( auto pEdge : pStep->get_edges() )
        {
            exclusive( pEdge->get_next(), finalFrontier, result );
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

Disambiguation disambiguate( Root* pStep, const std::vector< Or* >& finalFrontier )
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

#endif // GUARD_2023_September_26_disambiguate
