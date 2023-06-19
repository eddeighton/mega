
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

#ifndef GUARD_2023_June_14_algorithms
#define GUARD_2023_June_14_algorithms

#include "schematic/analysis/edge_mask.hpp"
#include "schematic/analysis/invariant.hpp"
#include "schematic/analysis/analysis.hpp"

#include "schematic/schematic.hpp"
#include "schematic/cut.hpp"

#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_2.h>

namespace exact
{

template < typename TFace, typename TFunctor >
inline void visitEdgesOfFace( TFace hFace, TFunctor&& visitor )
{
    if( !hFace->is_unbounded() )
    {
        auto iter  = hFace->outer_ccb();
        auto start = iter;
        do
        {
            visitor( iter );
            ++iter;
        } while( iter != start );
    }

    // search through all holes
    for( auto holeIter = hFace->holes_begin(), holeIterEnd = hFace->holes_end(); holeIter != holeIterEnd; ++holeIter )
    {
        auto iter  = *holeIter;
        auto start = iter;
        do
        {
            visitor( iter );
            ++iter;
        } while( iter != start );
    }
}

// Search for reachable faces from the input startFaces set such that the polygon boundary is NEVER exceeded and
// the edges explored ALWAYS satisfy the predicate
// returns sorted vector of faces in result
template < typename HalfEdgeType, typename FaceType, typename EdgePredicate >
inline void getSortedFaces( std::set< FaceType >& startFaces, std::vector< FaceType >& result,
                            EdgePredicate&& predicate )
{
    std::set< FaceType > faces = startFaces, open = startFaces, temp;

    // greedy algorithm to exaust open set until no new faces found
    while( !open.empty() )
    {
        for( auto f : open )
        {
            // go through all edges inside face
            INVARIANT( !f->is_unbounded(), "getSortedFaces with unbounded face" );
            {
                auto iter  = f->outer_ccb();
                auto start = iter;
                do
                {
                    // if the edge IS NOT on the polygon boundary then
                    // it MUST connect to another face WITHIN the polygon
                    if( predicate( iter ) )
                    {
                        auto newFace = iter->twin()->face();
                        if( !faces.contains( newFace ) )
                        {
                            INVARIANT( !newFace->is_unbounded(), "getSortedFaces with unbounded face" );
                            temp.insert( newFace );
                            faces.insert( newFace );
                        }
                    }
                    ++iter;
                } while( iter != start );
            }

            // search through all holes
            for( auto holeIter = f->holes_begin(), holeIterEnd = f->holes_end(); holeIter != holeIterEnd; ++holeIter )
            {
                auto iter  = *holeIter;
                auto start = iter;
                do
                {
                    if( predicate( iter ) )
                    {
                        auto newFace = iter->twin()->face();
                        INVARIANT( !newFace->is_unbounded(), "getSortedFaces with unbounded face" );
                        if( !faces.contains( newFace ) )
                        {
                            INVARIANT( !newFace->is_unbounded(), "getSortedFaces with unbounded face" );
                            temp.insert( newFace );
                            faces.insert( newFace );
                        }
                    }
                    ++iter;
                } while( iter != start );
            }
        }
        open.clear();
        open.swap( temp );
    }
    result.assign( faces.begin(), faces.end() );
}

template < typename HalfEdgeType, typename FaceType >
inline void getSortedFaces( std::set< FaceType >& startFaces, std::vector< FaceType >& result,
                            const std::vector< HalfEdgeType >& polygon )
{
    std::set< HalfEdgeType > boundary;
    {
        for( auto e : polygon )
        {
            boundary.insert( e );
            boundary.insert( e->twin() );
        }
    }
    getSortedFaces< HalfEdgeType, FaceType >(
        startFaces, result, [ &boundary ]( HalfEdgeType edge ) { return !boundary.contains( edge ); } );
}

template < typename HalfEdgeType, typename FaceType >
inline bool getSortedFacesInsidePolygon( const std::vector< HalfEdgeType >& polygon, std::vector< FaceType >& result )
{
    // determine the edge boundary as lookup set
    bool bIsCounterClockwise = true;
    {
        INVARIANT( polygon.size() > 2, "Degenerate polygon in getSortedFacesInsidePolygon" );
        INVARIANT( polygon.front()->source() == polygon.back()->target(),
                   "Non closed polygon in getSortedFacesInsidePolygon" );
        exact::Polygon poly;
        for( auto e : polygon )
        {
            poly.push_back( e->source()->point() );
        }
        INVARIANT( poly.is_simple(), "Non-simple polygon passed to getSortedFacesInsidePolygon" );
        bIsCounterClockwise = poly.is_counterclockwise_oriented();
    }

    std::set< FaceType > startFaces;
    for( auto e : polygon )
    {
        if( bIsCounterClockwise )
        {
            startFaces.insert( e->face() );
        }
        else
        {
            startFaces.insert( e->twin()->face() );
        }
    }
    getSortedFaces( startFaces, result, polygon );

    return bIsCounterClockwise;
}

template < typename Predicate >
inline void getEdges( const Analysis::Arrangement& arr, Analysis::HalfEdgeSet& edges, Predicate&& predicate )
{
    for( auto i = arr.halfedges_begin(); i != arr.halfedges_end(); ++i )
    {
        if( predicate( i ) )
        {
            edges.insert( i );
        }
    }
}

template < typename Predicate >
inline void getEdges( Analysis::Arrangement& arr, std::set< Analysis::Arrangement::Halfedge_handle >& edges,
                      Predicate&& predicate )
{
    for( auto i = arr.halfedges_begin(); i != arr.halfedges_end(); ++i )
    {
        if( predicate( i ) )
        {
            edges.insert( i );
        }
    }
}

template < typename Type, typename Predicate >
inline void getSubSequences( const std::vector< Type >& sequence, std::vector< std::vector< Type > >& subSequences,
                             Predicate&& predicate )
{
    std::vector< Type > active;
    const bool          bFirstTrue = sequence.empty() ? false : predicate( sequence.front() );
    for( auto e : sequence )
    {
        if( predicate( e ) )
        {
            active.push_back( e );
        }
        else if( !active.empty() )
        {
            subSequences.push_back( active );
            active.clear();
        }
    }
    if( !active.empty() )
    {
        if( bFirstTrue )
        {
            // if the very first element passed predicate then the final sequence should be prepended
            std::copy( subSequences[ 0 ].begin(), subSequences[ 0 ].end(), std::back_inserter( active ) );
            subSequences[ 0 ] = active;
        }
        else
        {
            subSequences.push_back( active );
        }
    }
}

// Using ALL edges find closed loops of halfedges with at least three edges each.
// Each vertex found MUST have a single out edge.
// Each edge MUST occur in a single polygon.
template < typename HalfEdgeSetType, typename HalfEdgeVectorVectorType >
inline void getPolygons( const HalfEdgeSetType& edges, HalfEdgeVectorVectorType& polygons )
{
    HalfEdgeSetType open = edges;

    using HalfEdge = typename HalfEdgeSetType::value_type;

    while( !open.empty() )
    {
        typename HalfEdgeVectorVectorType::value_type polygon;

        HalfEdge edgeIter = *open.begin();
        polygon.push_back( edgeIter );
        open.erase( edgeIter );

        auto startVert = edgeIter->source();
        while( edgeIter->target() != startVert )
        {
            HalfEdge nextEdge     = {};
            auto     incidentIter = edgeIter->target()->incident_halfedges(), incidentIterEnd = incidentIter;
            do
            {
                HalfEdge outEdge = incidentIter->twin();
                if( open.contains( outEdge ) )
                {
                    INVARIANT( nextEdge == HalfEdge{}, "Duplicate next edge found" );
                    nextEdge = outEdge;
                }
                ++incidentIter;
            } while( incidentIter != incidentIterEnd );
            INVARIANT( nextEdge != HalfEdge{}, "Failed to find next edge before polygon complete" );
            edgeIter = nextEdge;

            polygon.push_back( edgeIter );
            open.erase( edgeIter );
        }

        INVARIANT( polygon.size() >= 3, "Invalid polygon found with less than three edges" );
        polygons.push_back( polygon );
    }
}

// bDir == true means make the sharpest most acute turn counter clockwise when choosing next edge
template < typename HalfEdgeSetType, typename HalfEdgeVectorVectorType >
inline void getPolygonsDir( const HalfEdgeSetType& edges, HalfEdgeVectorVectorType& polygons, bool bDir = true )
{
    HalfEdgeSetType open = edges;

    using HalfEdge = typename HalfEdgeSetType::value_type;

    while( !open.empty() )
    {
        typename HalfEdgeVectorVectorType::value_type polygon;

        HalfEdge edgeIter = *open.begin();
        polygon.push_back( edgeIter );
        open.erase( edgeIter );

        auto startVert = edgeIter->source();
        while( edgeIter->target() != startVert )
        {
            HalfEdge nextEdge     = {};
            auto     incidentIter = edgeIter->target()->incident_halfedges();
            while( incidentIter != edgeIter )
                ++incidentIter;
            auto incidentIterEnd = incidentIter;
            do
            {
                if( bDir )
                {
                    ++incidentIter;
                }
                else
                {
                    --incidentIter;
                }
                HalfEdge outEdge = incidentIter->twin();
                if( open.contains( outEdge ) )
                {
                    nextEdge = outEdge;
                    break;
                }
            } while( incidentIter != incidentIterEnd );
            INVARIANT( nextEdge != HalfEdge{}, "Failed to find next edge before polygon complete" );
            edgeIter = nextEdge;

            polygon.push_back( edgeIter );
            open.erase( edgeIter );
        }

        INVARIANT( polygon.size() >= 3, "Invalid polygon found with less than three edges" );
        polygons.push_back( polygon );
    }
}

// NOTE: bDir decides the order to consider incident edges.
// If bDir IS FALSE then the algorithm will be inclusive i.e. it will choose the largest turn it can make
// if bDir IS TRUE then the most acute turn will be made.
template < typename VertexVector, typename HalfEdgeSetType >
inline void searchPolygons( const VertexVector& startVertices, const HalfEdgeSetType& edges, bool bDir,
                            std::vector< std::vector< Analysis::Arrangement::Halfedge_handle > >& polygons )
{
    using HalfEdge = typename HalfEdgeSetType::value_type;

    HalfEdgeSetType open = edges;

    for( auto startVertex : startVertices )
    {
        for( auto e : open )
        {
            if( e->source() == startVertex )
            {
                // attempt to solve polygon from here
                std::vector< Analysis::Arrangement::Halfedge_handle > polygon;

                open.erase( e );
                polygon.push_back( e );

                while( e->target() != startVertex )
                {
                    INVARIANT( !open.empty(), "Failed to find polygon" );
                    // https://doc.cgal.org/latest/Arrangement_on_surface_2/index.html
                    // The call v->incident_halfedges() returns a circulator of the nested type
                    // Halfedge_around_vertex_circulator that enables the traversal of this circular list around a given
                    // vertex v in a clockwise order.

                    // So - the most acute angle first - so take the last one
                    bool bFoundNextVertex = false;
                    auto edgeIter         = e->target()->incident_halfedges();
                    while( edgeIter != e )
                        ++edgeIter;
                    auto edgeIterEnd = edgeIter;
                    do
                    {
                        if( bDir )
                            ++edgeIter;
                        else
                            --edgeIter;
                        HalfEdge nextEdge = edgeIter->twin();
                        if( open.contains( nextEdge ) )
                        {
                            e                = nextEdge;
                            bFoundNextVertex = true;
                            break;
                        }
                    } while( edgeIter != edgeIterEnd );

                    INVARIANT( bFoundNextVertex, "Failed to find next vertex" );

                    open.erase( e );
                    polygon.push_back( e );
                }
                polygons.push_back( polygon );
                break;
            }
        }
    }
}

} // namespace exact

#endif // GUARD_2023_June_14_algorithms
