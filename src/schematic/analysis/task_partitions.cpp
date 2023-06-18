
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

#include "algorithms.hpp"

#include "schematic/analysis/invariant.hpp"
#include "schematic/analysis/analysis.hpp"

#include <vector>
#include <tuple>

namespace exact
{

inline void locateHoleBoundariesFromDoorStep( Analysis::Arrangement::Halfedge_handle                    doorStep,
                                              const std::set< Analysis::Arrangement::Halfedge_handle >& boundaryEdges,
                                              const std::vector< Analysis::Arrangement::Halfedge_handle >& floorEdges,
                                              std::set< Analysis::Arrangement::Halfedge_handle >& innerBoundaries,
                                              std::vector< Analysis::Arrangement::Face_handle >&  result )
{
    std::set< Analysis::Arrangement::Face_handle > startFaces;
    startFaces.insert( doorStep->face() );
    getSortedFaces< Analysis::Arrangement::Halfedge_handle, Analysis::Arrangement::Face_handle >(
        startFaces, result,

        // if predicate returns TRUE then getSortedFaces will cross edge to get adjacent face
        // NOTE: getSortedFaces will ALSO ONLY cross edges NOT within 'boundary' variable
        [ &boundaryEdges, &floorEdges, &innerBoundaries ]( Analysis::Arrangement::Halfedge_handle edge )
        {
            const auto& flags = edge->data().flags;

            // boundary edges are a superset of the floor edges
            // can have situation where outer room contains multiple inner rooms
            // where floor edges would only be outer contour
            const bool bIsBoundaryEdge = boundaryEdges.find( edge ) != boundaryEdges.end();
            const bool bIsFloorEdge    = std::find( floorEdges.begin(), floorEdges.end(), edge ) != floorEdges.end();

            // clang-format off
            const bool bIsHoleEdge
                = ( 
                    flags.test( EdgeMask::eInterior ) || 
                    flags.test( EdgeMask::eExterior ) || 
                    flags.test( EdgeMask::eConnectionBisector ) || 
                    flags.test( EdgeMask::eDoorStep ) 
                 )
                  && !flags.test( EdgeMask::eConnectionBreak );
            // clang-format on

            if( !bIsFloorEdge && bIsHoleEdge )
            {
                // collect discovered edges NOT within outer boundary
                innerBoundaries.insert( edge );
            }

            // Prevent searching to the unbounded edge by not allowing EdgeMask::ePerimeter
            return !bIsBoundaryEdge && !flags.test( EdgeMask::ePerimeter );
        } );
}

void Analysis::partition()
{
    std::vector< Arrangement::Vertex_handle >          doorStepVertices;
    std::set< Analysis::Arrangement::Halfedge_handle > boundaryEdges;
    std::set< Analysis::Arrangement::Halfedge_handle > doorSteps;
    {
        getEdges( m_arr, doorSteps,
                  []( Arrangement::Halfedge_handle edge ) { return edge->data().flags.test( EdgeMask::eDoorStep ); } );
        for( auto d : doorSteps )
        {
            doorStepVertices.push_back( d->source() );
        }
    }

    {
        using HalfEdgeSet = std::set< Analysis::Arrangement::Halfedge_handle >;
        HalfEdgeSet floorEdges;
        getEdges( m_arr, floorEdges,
                  []( Arrangement::Halfedge_handle edge )
                  {
                      auto& flags = edge->data().flags;
                      return ( flags.test( EdgeMask::eInterior ) || flags.test( EdgeMask::eExterior )
                               || flags.test( EdgeMask::eConnectionBisector ) || flags.test( EdgeMask::eDoorStep ) )

                             && !flags.test( EdgeMask::eConnectionBreak );
                  } );

        std::vector< std::vector< Arrangement::Halfedge_handle > > floorPolygons;
        searchPolygons( doorStepVertices, floorEdges, true, floorPolygons );

        for( auto floorBoundary : floorPolygons )
        {
            // boundary MAY have already been classified i.e. two areas within parent and search from
            // one doorstep - will classify other where ALL part of same partition
            // yet the other floorboundary will be in floorPolygons.
            int iClassification = 0;
            for( auto e : floorBoundary )
            {
                if( e->data().pPartition )
                {
                    INVARIANT( iClassification == 0 || iClassification == 1, "Inconsistent classification" );
                    iClassification = 1;
                }
                else
                {
                    INVARIANT( iClassification == 0 || iClassification == 2, "Inconsistent classification" );
                    iClassification = 2;
                }
            }
            if( iClassification == 1 )
                continue;

            Partition::Ptr                      pPartition = std::make_unique< Partition >();
            std::set< schematic::Site::PtrCst > sites;

            // classify the outer floor boundary that has been reachable from the doorstep
            HalfEdgeSet floorDoorSteps;
            for( auto e : floorBoundary )
            {
                if( e->data().flags.test( EdgeMask::eInterior ) || e->data().flags.test( EdgeMask::eExterior ) )
                {
                    INVARIANT( !e->data().sites.empty(), "Interior or exterior edge missing site" );
                    sites.insert( e->data().sites.back() );
                }
                e->data().pPartition = pPartition.get();
                classify( e, EdgeMask::ePartitionFloor );
                if( !e->data().flags.test( EdgeMask::eDoorStep ) )
                {
                    if( !e->data().flags.test( EdgeMask::ePerimeter ) )
                    {
                        classify( e->twin(), EdgeMask::ePartitionBoundary );
                        boundaryEdges.insert( e->twin() );
                    }
                }
                else
                {
                    floorDoorSteps.insert( e );
                }
            }

            // now search WITHIN the door steps of the floor boundary
            for( auto e : floorDoorSteps )
            {
                // first locate all reachable faces from the door step that are contained in the floor boundary
                // the algorithm returns the set of halfedges in holeBoundaries that
                // are reachable through the faces but are NOT in floorBoundary

                // NOTE: MUST pass ALL floorEdges here not just the floorBoundary
                std::set< Analysis::Arrangement::Halfedge_handle > holeBoundaries;
                {
                    std::vector< Analysis::Arrangement::Face_handle > faces;
                    locateHoleBoundariesFromDoorStep( e, floorEdges, floorBoundary, holeBoundaries, faces );
                    for( auto f : faces )
                    {
                        INVARIANT( ( !f->data().pPartition ) || ( f->data().pPartition == pPartition.get() ),
                                   "Conflicting face partition" );
                        f->data().pPartition = pPartition.get();
                    }
                }

                // Because searching from all door steps - if there are no obstacles will get the
                // same result each time - i.e. the algorithm must be correct if setting the same
                // result multiple times

                std::vector< std::vector< Arrangement::Halfedge_handle > > floorHoles;
                getPolygons( holeBoundaries, floorHoles );

                for( auto innerWallBoundary : floorHoles )
                {
                    for( auto innerWallEdge : innerWallBoundary )
                    {
                        innerWallEdge->data().pPartition = pPartition.get();
                        classify( innerWallEdge, EdgeMask::ePartitionFloor );
                        if( !innerWallEdge->data().flags.test( EdgeMask::eDoorStep ) )
                        {
                            if( !innerWallEdge->data().flags.test( EdgeMask::ePerimeter ) )
                            {
                                classify( innerWallEdge->twin(), EdgeMask::ePartitionBoundary );
                                boundaryEdges.insert( innerWallEdge->twin() );
                            }
                        }
                    }
                }
            }

            INVARIANT( sites.size() != 0, "Floor has no sites" );
            INVARIANT( sites.size() == 1, "Floor has multiple sites" );
            pPartition->pSite = *sites.begin();
            m_floors.emplace_back( std::move( pPartition ) );
        }
    }

    // The resultant partition boundary edges can no be used to determine the boundaries
    {
        // for starting points just use ALL boundary vertices
        std::vector< Arrangement::Vertex_handle > boundaryVertices;
        for( auto e : boundaryEdges )
        {
            boundaryVertices.push_back( e->source() );
        }

        std::vector< std::vector< Arrangement::Halfedge_handle > > boundaryPolygons;
        searchPolygons( boundaryVertices, boundaryEdges, false, boundaryPolygons );

        for( auto& boundary : boundaryPolygons )
        {
            // determine if there is already a partition - will have two sides to boundary
            Partition*                                        pPartition = nullptr;
            std::vector< Analysis::Arrangement::Face_handle > faces;
            {
                getSortedFacesInsidePolygon( boundary, faces );
                for( auto f : faces )
                {
                    if( f->data().pPartition )
                    {
                        pPartition = f->data().pPartition;
                        break;
                    }
                }
            }
            INVARIANT( !pPartition, "Boundary already has partition" );
            if( !pPartition )
            {
                Partition::Ptr p = std::make_unique< Partition >();
                pPartition       = p.get();
                m_boundaries.emplace_back( std::move( p ) );
            }

            for( auto f : faces )
            {
                INVARIANT(
                    ( !f->data().pPartition ) || ( f->data().pPartition == pPartition ), "Face partition already set" );
                f->data().pPartition = pPartition;
            }

            for( auto e : boundary )
            {
                e->data().pPartition = pPartition;
            }
        }
    }

    // test
    {
        for( auto e : boundaryEdges )
        {
            INVARIANT( e->data().pPartition, "Boundary edge with no partition" );
        }
    }

    // now determine all boundary segments
    {
        // Get all boundary edges COMBINED with the cut edges
        // NOTE that cut edges are on both sides of the cut curve
        std::set< Analysis::Arrangement::Halfedge_handle > cutEdges;
        getEdges( m_arr, cutEdges,
                  []( Arrangement::Halfedge_handle edge ) { return edge->data().flags.test( EdgeMask::eCut ); } );

        // just add ALL vertices from the boundary as start vertices
        std::vector< Arrangement::Vertex_handle > boundarySegmentStartVertices;
        {
            for( auto e : boundaryEdges )
            {
                boundarySegmentStartVertices.push_back( e->source() );
            }
        }

        std::set< Analysis::Arrangement::Halfedge_handle > boundaryAndCutEdges = boundaryEdges;
        boundaryAndCutEdges.insert( cutEdges.begin(), cutEdges.end() );

        std::vector< std::vector< Arrangement::Halfedge_handle > > boundarySegmentPolygons;
        searchPolygons( boundarySegmentStartVertices, boundaryAndCutEdges, true, boundarySegmentPolygons );

        for( auto segmentBoundary : boundarySegmentPolygons )
        {
            // only admit boundary segments that have atleast three edges AND
            // ALWAYS contain atleast one partition boundary edge
            if( segmentBoundary.size() > 2 )
            {
                Partition* pPartition = nullptr;
                for( auto e : segmentBoundary )
                {
                    if( e->data().flags.test( EdgeMask::ePartitionBoundary ) )
                    {
                        pPartition = e->data().pPartition;
                        INVARIANT( pPartition, "Boundary without paritition" );
                        break;
                    }
                }
                if( pPartition )
                {
                    PartitionSegment* pPartitionSegment = nullptr;

                    std::vector< Analysis::Arrangement::Face_handle > faces;
                    getSortedFacesInsidePolygon( segmentBoundary, faces );
                    for( auto f : faces )
                    {
                        if( f->data().pPartitionSegment )
                        {
                            pPartitionSegment = f->data().pPartitionSegment;
                            break;
                        }
                    }
                    if( !pPartitionSegment )
                    {
                        PartitionSegment::Ptr p = std::make_unique< PartitionSegment >();
                        pPartitionSegment       = p.get();
                        pPartition->segments.push_back( pPartitionSegment );
                        m_boundarySegments.emplace_back( std::move( p ) );
                    }

                    // set the faces with partitionSegment
                    for( auto f : faces )
                    {
                        INVARIANT(
                            ( !f->data().pPartitionSegment ) || ( f->data().pPartitionSegment == pPartitionSegment ),
                            "Face partition segment already set" );
                        f->data().pPartitionSegment = pPartitionSegment;
                    }

                    for( auto e : segmentBoundary )
                    {
                        classify( e, EdgeMask::ePartitionBoundarySegment );
                        e->data().pPartitionSegment = pPartitionSegment;
                        if( e->data().pPartition )
                        {
                            INVARIANT( e->data().pPartition == pPartition, "Inconstitent partition" );
                        }
                        else
                        {
                            e->data().pPartition = pPartition;
                        }
                    }
                }
            }
        }
    }
}
} // namespace exact
