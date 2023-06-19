
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
namespace
{

void collectPins( schematic::Node::Ptr pNode, std::vector< schematic::Feature_Pin::Ptr >& pins )
{
    if( schematic::Feature_Pin::Ptr pPin = boost::dynamic_pointer_cast< schematic::Feature_Pin >( pNode ) )
    {
        pins.push_back( pPin );
    }
    for( auto pChild : pNode->getChildren() )
    {
        collectPins( pChild, pins );
    }
}

void collectProperties( schematic::Node::Ptr pNode, std::vector< schematic::Property::PtrCst >& properties )
{
    if( schematic::Property::PtrCst pProperty = boost::dynamic_pointer_cast< const schematic::Property >( pNode ) )
    {
        properties.push_back( pProperty );
    }
    for( auto pChild : pNode->getChildren() )
    {
        collectProperties( pChild, properties );
    }
}

void collectPropertiesForSite( schematic::Node::Ptr pNode, std::vector< schematic::Property::Ptr >& properties )
{
    if( schematic::Property::Ptr pProperty = boost::dynamic_pointer_cast< schematic::Property >( pNode ) )
    {
        properties.push_back( pProperty );
    }

    for( auto pChild : pNode->getChildren() )
    {
        // ignore nested sites
        if( boost::dynamic_pointer_cast< schematic::Site >( pChild ) )
        {
            continue;
        }
        else if( boost::dynamic_pointer_cast< schematic::Feature_Pin >( pChild ) )
        {
            continue;
        }
        else
        {
            // collect everything else
            collectPropertiesForSite( pChild, properties );
        }
    }
}

} // namespace

void Analysis::properties()
{
    auto sites = m_pSchematic->getSites();
    INVARIANT( sites.size() < 2, "Schematic contains more than one root site" );
    INVARIANT( sites.size() == 1, "Schematic missing root site" );

    schematic::Space::Ptr pRootSpace = boost::dynamic_pointer_cast< schematic::Space >( sites.front() );

    std::vector< schematic::Property::Ptr > properties;
    propertiesRecurse( pRootSpace, properties );

    Analysis::Point_location pointLocation( m_arr );

    std::vector< schematic::Feature_Pin::Ptr > pins;
    collectPins( pRootSpace, pins );

    for( auto pPin : pins )
    {
        std::optional< exact::Transform > transformOpt;
        {
            schematic::Node::Ptr pParent = pPin->schematic::Node::getParent();
            while( pParent )
            {
                if( schematic::Site::Ptr pSite = boost::dynamic_pointer_cast< schematic::Site >( pParent ) )
                {
                    transformOpt = pSite->getAbsoluteExactTransform();
                    break;
                }
                pParent = pParent->getParent();
            }
        }

        static const exact::InexactToExact converter;
        exact::Point                       pt = converter( pPin->getPoint( 0 ) );
        if( transformOpt.has_value() )
        {
            pt = transformOpt.value()( pt );
        }

        // locate partition containing point
        CGAL::Object                       result = pointLocation.locate( pt );
        Arrangement::Face_const_handle     face;
        Arrangement::Halfedge_const_handle halfedge;
        Arrangement::Vertex_const_handle   vertex;
        if( CGAL::assign( face, result ) )
        {
            if( auto pPartition = face->data().pPartition )
            {
                pPartition->pins.push_back( pPin );
                collectProperties( pPin, pPartition->properties );
            }
            else
            {
                INVARIANT( false, "Pin face has no parition: " << pPin->getName() );
            }
            if( auto pPartitionSegment = face->data().pPartitionSegment )
            {
                pPartitionSegment->pins.push_back( pPin );
                collectProperties( pPin, pPartitionSegment->properties );

                // determine the partition segment plane
                for( const auto pProperty : pPartitionSegment->properties )
                {
                    if( pProperty->getName() == "plane" )
                    {
                        if( pProperty->getValue() == "Hole" )
                        {
                            pPartitionSegment->plane = PartitionSegment::eHole;
                        }
                        else if( pProperty->getValue() == "Ground" )
                        {
                            pPartitionSegment->plane = PartitionSegment::eGround;
                        }
                        else if( pProperty->getValue() == "Mid" )
                        {
                            pPartitionSegment->plane = PartitionSegment::eMid;
                        }
                        else if( pProperty->getValue() == "Ceiling" )
                        {
                            pPartitionSegment->plane = PartitionSegment::eCeiling;
                        }
                        else
                        {
                            INVARIANT( false, "Unrecognised plane type: " << pProperty->getValue() );
                        }
                    }
                }
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
void Analysis::propertiesRecurse( schematic::Site::Ptr pSite, std::vector< schematic::Property::Ptr >& properties )
{
    collectPropertiesForSite( pSite, properties );

    // find all partitions of this site
    std::vector< Partition* > sitePartitions;
    for( const auto& pPartition : m_floors )
    {
        if( pPartition->pSite == pSite )
        {
            sitePartitions.push_back( pPartition.get() );
        }
    }

    // propagate all properties to partitions
    for( auto pPartition : sitePartitions )
    {
        for( auto pProperty : properties )
        {
            pPartition->properties.push_back( pProperty );
        }
    }

    // recurse
    for( schematic::Site::Ptr pNestedSite : pSite->getSites() )
    {
        std::vector< schematic::Property::Ptr > nestedProperties = properties;
        propertiesRecurse( pNestedSite, nestedProperties );
    }
}
} // namespace exact
