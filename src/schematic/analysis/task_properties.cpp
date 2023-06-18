
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

#include "schematic/analysis/analysis.hpp"
#include "schematic/schematic.hpp"
#include "schematic/feature.hpp"
#include "schematic/property.hpp"

#include <vector>
#include <tuple>

namespace exact
{

void Analysis::properties()
{
    auto sites = m_pSchematic->getSites();
    INVARIANT( sites.size() < 2, "Schematic contains more than one root site" );
    INVARIANT( sites.size() == 1, "Schematic missing root site" );

    std::vector< schematic::Feature_Pin::Ptr > pins;
    std::vector< schematic::Property::Ptr >    properties;

    schematic::Space::Ptr pRootSpace = boost::dynamic_pointer_cast< schematic::Space >( sites.front() );
    propertiesRecurse( pRootSpace, pins, properties );

    Analysis::Point_location pointLocation( m_arr );

    for( auto pPin : pins )
    {
        const schematic::Point& pt = pPin->getPoint( 0 );
        // locate partition containing point

        CGAL::Object                 result = pointLocation.locate( exact::Point( pt.x(), pt.y() ) );
        Arrangement::Face_handle     face;
        Arrangement::Halfedge_handle halfedge;
        Arrangement::Vertex_handle   vertex;
        if( CGAL::assign( face, result ) )
        {
            if( auto pPartition = face->data().pPartition )
            {
                pPartition->pins.push_back( pPin );
            }
            else
            {
                INVARIANT( false, "Pin face has no parition: " << pPin->getName() );
            }
            if( auto pPartitionSegment = face->data().pPartitionSegment )
            {
                pPartitionSegment->pins.push_back( pPin );
            }
        }
        else if( CGAL::assign( halfedge, result ) )
        {
            INVARIANT( false, "Pin intersects halfedge: " << pPin->getName() );
        }
        else if( CGAL::assign( vertex, result ) )
        {
            INVARIANT( false, "Pin intersects vertex: " << pPin->getName() );
        }
        else
        {
            INVARIANT( false, "Pin does not intersect anything: " << pPin->getName() );
        }
    }
}

void collectProperties( schematic::Node::Ptr                        pNode,
                        std::vector< schematic::Feature_Pin::Ptr >& pins,
                        std::vector< schematic::Property::Ptr >&    properties )
{
    if( schematic::Property::Ptr pProperty = boost::dynamic_pointer_cast< schematic::Property >( pNode ) )
    {
        properties.push_back( pProperty );
    }
    else if( schematic::Feature_Pin::Ptr pPin = boost::dynamic_pointer_cast< schematic::Feature_Pin >( pNode ) )
    {
        pins.push_back( pPin );
    }

    for( auto pChild : pNode->getChildren() )
    {
        // ignore nested sites
        if( schematic::Site::Ptr pNestedSite = boost::dynamic_pointer_cast< schematic::Site >( pChild ) )
        {
            continue;
        }
        else
        {
            // collect everything else
            collectProperties( pChild, pins, properties );
        }
    }
}

void Analysis::propertiesRecurse( schematic::Site::Ptr                        pSite,
                                  std::vector< schematic::Feature_Pin::Ptr >& pins,
                                  std::vector< schematic::Property::Ptr >&    properties )
{
    collectProperties( pSite, pins, properties );

    // find all floors of this site
    std::vector< Partition* > sitePartitions;
    for( const auto& pPartition : m_floors )
    {
        if( pPartition->pSite == pSite )
        {
            sitePartitions.push_back( pPartition.get() );
        }
    }

    for( auto pPartition : sitePartitions )
    {
        for( auto pProperty : properties )
        {
            pPartition->properties.push_back( pProperty );
        }
    }

    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        std::vector< schematic::Feature_Pin::Ptr > nestedPins       = pins;
        std::vector< schematic::Property::Ptr >&   nestedProperties = properties;
        propertiesRecurse( pNestedSite, nestedPins, properties );
    }
}
} // namespace exact
