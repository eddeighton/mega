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

#ifndef DATABITMAP_14_09_2013
#define DATABITMAP_14_09_2013

#include "schematic/cgalSettings.hpp"

#include "buffer.hpp"

#include <vector>

namespace schematic
{
    
class Site;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class DataBitmap
{
public:
    class Claim
    {
    public:
        typedef std::vector< Claim > Vector;

        Claim( Float x, Float y, NavBitmap::Ptr pBuffer, boost::shared_ptr< Site > pSite );

        Float m_x, m_y;
        NavBitmap::Ptr m_pBuffer;
        boost::shared_ptr< Site > m_pSite;
    };

    bool makeClaim( const Claim& claim );

    const Claim::Vector& getClaims() const { return m_claims; }
private:
    Claim::Vector m_claims;
};

}

#endif //DATABITMAP_14_09_2013