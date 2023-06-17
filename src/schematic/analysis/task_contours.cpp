
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

#include "constructions.hpp"

#include "schematic/analysis/analysis.hpp"
#include "schematic/schematic.hpp"

#include <vector>
#include <tuple>

namespace exact
{

void Analysis::contours()
{
    auto sites = m_pSchematic->getSites();
    INVARIANT( sites.size() < 2, "Schematic contains more than one root site" );
    INVARIANT( sites.size() == 1, "Schematic missing root site" );

    schematic::Space::Ptr pRootSpace = boost::dynamic_pointer_cast< schematic::Space >( sites.front() );
    INVARIANT( pRootSpace, "Root site is not a space" );
    renderContour( m_arr, pRootSpace->getAbsoluteExactTransform(), pRootSpace->getInteriorPolygon(),
                EdgeMask::ePerimeter, EdgeMask::ePerimeterBoundary, pRootSpace, {} );

    contoursRecurse( pRootSpace );
}

void Analysis::contoursRecurse( schematic::Site::Ptr pSite )
{
    if( schematic::Space::Ptr pSpace = boost::dynamic_pointer_cast< schematic::Space >( pSite ) )
    {
        const exact::Transform transform = pSpace->getAbsoluteExactTransform();

        // render the interior polygon
        {
            renderContour( m_arr, transform, pSpace->getInteriorPolygon(), EdgeMask::eInterior,
                           EdgeMask::eInteriorBoundary, pSpace, {} );
        }

        // render the exterior polygons
        {
            for( const exact::Polygon& p : pSpace->getInnerExteriorUnions() )
            {
                renderContour( m_arr, transform, p, EdgeMask::eExteriorBoundary, EdgeMask::eExterior, {}, pSpace );
            }
        }
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        contoursRecurse( pNestedSite );
    }
}

}
