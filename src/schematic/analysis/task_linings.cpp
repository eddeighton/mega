
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
#include "constructions.hpp"
#include "polygon_tree.hpp"

#include "schematic/cgalUtils.hpp"
#include "schematic/analysis/analysis.hpp"
#include "schematic/analysis/polygon_with_holes.hpp"

#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_from_polygon_with_holes_2.h>
#include <CGAL/create_offset_polygons_2.h>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#include <tuple>

namespace exact
{
namespace
{

}

void calculateLinings( Analysis::Arrangement& arr, const Analysis::HalfEdgePolygonWithHoles& poly )
{
    const exact::Polygon_with_holes polygonWithHoles
        = exact::fromHalfEdgePolygonWithHolesFiltered< Analysis::Vertex >( poly );
    using StraightSkeleton    = CGAL::Straight_skeleton_2< exact::Kernel >;
    using StraightSkeletonPtr = boost::shared_ptr< StraightSkeleton >;

    StraightSkeletonPtr pStraightSkeleton = CGAL::create_interior_straight_skeleton_2(
        polygonWithHoles.outer_boundary().begin(), polygonWithHoles.outer_boundary().end(),
        polygonWithHoles.holes_begin(), polygonWithHoles.holes_end(), exact::Kernel() );

    std::map< int, exact::Point >                  pointIDMap;
    std::map< exact::Point, int >                  idPointMap;
    std::vector< StraightSkeleton::Vertex_handle > skeletonVertices, contourVertices;
    for( StraightSkeleton::Vertex_handle v : pStraightSkeleton->vertex_handles() )
    {
        pointIDMap.insert( { v->id(), v->point() } );
        idPointMap.insert( { v->point(), v->id() } );

        if( v->is_skeleton() )
        {
            INVARIANT( !v->is_contour(), "Vertex is contour and skeleton" );
            skeletonVertices.push_back( v );
        }
        else
        {
            INVARIANT( v->is_contour(), "Vertex not contour or skeleton" );
            contourVertices.push_back( v );
        }
    }

    // check ALL points can be found in the straight skeleton
    for( auto e : polygonWithHoles.outer_boundary() )
    {
        INVARIANT( idPointMap.find( e ) != idPointMap.end(), "Failed to locate point in buildLiningHorizontalMesh" );
    }
    for( const auto& polygon : polygonWithHoles.holes() )
    {
        for( auto e : polygon )
        {
            INVARIANT(
                idPointMap.find( e ) != idPointMap.end(), "Failed to locate point in buildLiningHorizontalMesh" );
        }
    }

    for( auto h : pStraightSkeleton->halfedge_handles() )
    {
        StraightSkeleton::Halfedge_handle e = h;
        if( !e->is_border() && ( e->vertex()->is_skeleton() || e->next()->vertex()->is_skeleton() ) )
        {
            renderCurve( arr, Curve( e->vertex()->point(), e->next()->vertex()->point() ), EdgeMask::eSkeleton,
                         EdgeMask::eSkeleton );
        }
    }
}

void Analysis::linings()
{
    HalfEdgePolygonWithHoles::Vector lanes, laneLinings, pavementLinings;
    {
        enum NodeType
        {
            eNodeLaneSpace,
            eNodeLaneLining,
            eNodePavementLining,
            TOTAL_NODE_TYPES
        };
        using Node       = exact::PolygonNodeT< HalfEdge, Face, NodeType >;
        using NodePtr    = typename Node::Ptr;
        using NodeVector = std::vector< NodePtr >;
        NodeVector nodes;
        {
            {
                HalfEdgeVectorVector polygons;
                HalfEdgeSet          edges;
                getEdges( m_arr, edges, []( HalfEdge edge ) { return test( edge, EdgeMask::eLaneSpace ); } );
                getPolygonsDir( edges, polygons, true );
                for( auto& poly : polygons )
                {
                    nodes.push_back( std::make_shared< Node >( eNodeLaneSpace, poly ) );
                }
            }
            {
                HalfEdgeVectorVector polygons;
                HalfEdgeSet          edges;
                getEdges( m_arr, edges, []( HalfEdge edge ) { return test( edge, EdgeMask::eLaneLining ); } );
                getPolygonsDir( edges, polygons, true );
                for( auto& poly : polygons )
                {
                    nodes.push_back( std::make_shared< Node >( eNodeLaneLining, poly ) );
                }
            }
            {
                HalfEdgeVectorVector polygons;
                HalfEdgeSet          edges;
                getEdges( m_arr, edges, []( HalfEdge edge ) { return test( edge, EdgeMask::ePavementLining ); } );
                getPolygonsDir( edges, polygons, true );
                for( auto& poly : polygons )
                {
                    nodes.push_back( std::make_shared< Node >( eNodePavementLining, poly ) );
                }
            }
        }

        auto rootNodes = sortPolygonNodes< HalfEdge, Face, NodeType >( nodes );

        struct Visitor
        {
            HalfEdgePolygonWithHoles::Vector& lanes;
            HalfEdgePolygonWithHoles::Vector& laneLinings;
            HalfEdgePolygonWithHoles::Vector& pavementLinings;

            void visit( NodePtr pNode )
            {
                if( pNode->face() )
                {
                    switch( pNode->type() )
                    {
                        case eNodeLaneSpace:
                        {
                            HalfEdgePolygonWithHoles lane;
                            lane.outer = pNode->polygon();
                            for( auto pContour : pNode->contained() )
                            {
                                lane.holes.push_back( pContour->polygon() );
                            }
                            lanes.push_back( lane );
                        }
                        break;
                        case eNodeLaneLining:
                        {
                            HalfEdgePolygonWithHoles laneLining;
                            laneLining.outer = pNode->polygon();
                            for( auto pContour : pNode->contained() )
                            {
                                laneLining.holes.push_back( pContour->polygon() );
                            }
                            laneLinings.push_back( laneLining );
                        }
                        break;
                        case eNodePavementLining:
                        {
                            HalfEdgePolygonWithHoles pavementLining;
                            pavementLining.outer = pNode->polygon();
                            for( auto pContour : pNode->contained() )
                            {
                                pavementLining.holes.push_back( pContour->polygon() );
                            }
                            pavementLinings.push_back( pavementLining );
                        }
                        break;
                        case TOTAL_NODE_TYPES:
                        default:
                        {
                            INVARIANT( false, "Unknown node type" );
                        }
                        break;
                    }
                }

                for( auto pContour : pNode->contained() )
                {
                    visit( pContour );
                }
            }
        } visitor{ lanes, laneLinings, pavementLinings };

        for( auto r : rootNodes )
        {
            visitor.visit( r );
        }
    }

    for( const auto& poly : lanes )
    {
        calculateLinings( m_arr, poly );
    }
    for( const auto& poly : laneLinings )
    {
        calculateLinings( m_arr, poly );
    }
    for( const auto& poly : pavementLinings )
    {
        calculateLinings( m_arr, poly );
    }
}
} // namespace exact