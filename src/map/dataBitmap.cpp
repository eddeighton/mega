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

#include "map/dataBitmap.hpp"

#include "map/site.hpp"

namespace map
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DataBitmap::Claim::Claim( Float x, Float y, NavBitmap::Ptr pBuffer, boost::shared_ptr< Site > pSite )
    : m_x( x )
    , m_y( y )
    , m_pBuffer( pBuffer )
    , m_pSite( pSite )
{
}

bool DataBitmap::makeClaim( const DataBitmap::Claim& claim )
{
    bool bValid = true;

    // verify the claim is valid...
    /*for( Claim::Vector::iterator i = m_claims.begin(),
        iEnd = m_claims.end(); i!=iEnd; ++i )
    {

    }*/

    m_claims.push_back( claim );

    return bValid;
}

} // namespace map