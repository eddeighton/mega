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

#include "schematic/cgalSettings.hpp"
#include "schematic/geometry.hpp"
#include "schematic/space.hpp"
#include "schematic/connection.hpp"
#include "schematic/transform.hpp"
#include "schematic/compilation.hpp"

#include <vector>

namespace
{
/*
inline schematic::Point2D make_source_point( schematic::Arrangement::Halfedge_const_handle halfedge )
{
    return wykobi::make_point< float >(
            CGAL::to_double( halfedge->source()->point().x() ),
            CGAL::to_double( halfedge->source()->point().y() ) );
}
inline schematic::Point2D make_target_point( schematic::Arrangement::Halfedge_const_handle halfedge )
{
    return wykobi::make_point< float >(
            CGAL::to_double( halfedge->target()->point().x() ),
            CGAL::to_double( halfedge->target()->point().y() ) );
}


schematic::Point2D getFaceInteriorPoint( schematic::Arrangement::Face_const_handle hFace )
{
    schematic::Arrangement::Ccb_halfedge_const_circulator halfedgeCirculator = hFace->outer_ccb();
    schematic::Arrangement::Halfedge_const_handle hEdge = halfedgeCirculator;

    //deterine an interior point of the face
    const schematic::Point2D ptSource = make_source_point( hEdge );
    const schematic::Point2D ptTarget = make_target_point( hEdge );

    const schematic::Vector2D vDir = ptTarget - ptSource;
    const schematic::Vector2D vNorm =
        wykobi::normalize(
            wykobi::make_vector< float >( -vDir.y, vDir.x ) );
    const schematic::Point2D ptMid = ptSource + vDir * 0.5f;
    const schematic::Point2D ptInterior = ptMid + vNorm * 0.1f;

    return ptInterior;
}  */
/*
void faceToPolygon( schematic::Arrangement::Face_const_handle hFace, schematic::Polygon2D& polygon )
{
    VERIFY_RTE( !hFace->is_unbounded() );

    schematic::Arrangement::Ccb_halfedge_const_circulator iter = hFace->outer_ccb();
    schematic::Arrangement::Ccb_halfedge_const_circulator first = iter;
    do
    {
        polygon.push_back( make_source_point( iter ) );
        ++iter;
    }
    while( iter != first );
}

void faceToPolygonWithHoles( schematic::Arrangement::Face_const_handle hFace, schematic::PolygonWithHoles& polygon )
{
    if( !hFace->is_unbounded() )
    {
        schematic::Arrangement::Ccb_halfedge_const_circulator iter = hFace->outer_ccb();
        schematic::Arrangement::Ccb_halfedge_const_circulator first = iter;
        do
        {
            polygon.outer.push_back( make_source_point( iter ) );
            ++iter;
        }
        while( iter != first );
    }

    for( schematic::Arrangement::Hole_const_iterator
        holeIter = hFace->holes_begin(),
        holeIterEnd = hFace->holes_end();
            holeIter != holeIterEnd; ++holeIter )
    {
        schematic::Polygon2D hole;
        schematic::Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
        schematic::Arrangement::Ccb_halfedge_const_circulator start = iter;
        do
        {
            hole.push_back( make_source_point( iter ) );
            --iter;
        }
        while( iter != start );
        polygon.holes.emplace_back( hole );
    }
}*/
/*
    void wallSection(
            schematic::Arrangement::Halfedge_const_handle hStart,
            schematic::Arrangement::Halfedge_const_handle hEnd,
            schematic::Wall& wall )
    {
        THROW_RTE( "TODO" );

        schematic::Arrangement::Halfedge_const_handle hIter = hStart;
        do
        {
            if( hStart != hIter )
            {
                VERIFY_RTE_MSG( !hIter->data().get(), "Door step error" );
            }
            wall.points.push_back( make_target_point( hIter ) );
            hIter = hIter->next();
        }
        while( hIter != hEnd );
    }*/
/*
    void floorToWalls( schematic::Arrangement::Face_const_handle hFloor, std::vector< schematic::Wall >& walls )
    {
        THROW_RTE( "TODO" );

        using DoorStepVector = std::vector< schematic::Arrangement::Halfedge_const_handle >;

        if( !hFloor->is_unbounded() )
        {
            //find the doorsteps if any
            DoorStepVector doorsteps;
            {
                //outer ccb winds COUNTERCLOCKWISE around the outer contour
                schematic::Arrangement::Ccb_halfedge_const_circulator iter = hFloor->outer_ccb();
                schematic::Arrangement::Ccb_halfedge_const_circulator first = iter;
                do
                {
                    if( iter->data().get() )
                        doorsteps.push_back( iter );
                    ++iter;
                }
                while( iter != first );
            }

            if( !doorsteps.empty() )
            {
                //iterate between each doorstep pair
                for( DoorStepVector::iterator i = doorsteps.begin(),
                    iNext = doorsteps.begin(),
                    iEnd = doorsteps.end(); i!=iEnd; ++i )
                {
                    ++iNext;
                    if( iNext == iEnd )
                        iNext = doorsteps.begin();

                    schematic::Wall wall( false, true );
                    wallSection( *i, *iNext, wall );
                    walls.emplace_back( wall );
                }
            }
            else
            {
                schematic::Wall wall( true, true );
                schematic::Arrangement::Ccb_halfedge_const_circulator iter = hFloor->outer_ccb();
                schematic::Arrangement::Ccb_halfedge_const_circulator start = iter;
                do
                {
                    wall.points.push_back( make_source_point( iter ) );
                    ++iter;
                }
                while( iter != start );
                walls.emplace_back( wall );
            }
        }

        for( schematic::Arrangement::Hole_const_iterator
            holeIter = hFloor->holes_begin(),
            holeIterEnd = hFloor->holes_end();
                holeIter != holeIterEnd; ++holeIter )
        {
            DoorStepVector doorsteps;
            {
                //the hole circulators wind CLOCKWISE around the hole contours
                schematic::Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
                schematic::Arrangement::Ccb_halfedge_const_circulator start = iter;
                do
                {
                    if( iter->data().get() )
                        doorsteps.push_back( iter );
                    ++iter;
                }
                while( iter != start );
            }

            //iterate between each doorstep pair
            if( !doorsteps.empty() )
            {
                for( DoorStepVector::iterator i = doorsteps.begin(),
                    iNext = doorsteps.begin(),
                    iEnd = doorsteps.end(); i!=iEnd; ++i )
                {
                    ++iNext;
                    if( iNext == iEnd )
                        iNext = doorsteps.begin();

                    schematic::Wall wall( false, false );
                    wallSection( *i, *iNext, wall );
                    walls.emplace_back( wall );
                }
            }
            else
            {
                schematic::Wall wall( true, false );
                schematic::Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
                schematic::Arrangement::Ccb_halfedge_const_circulator start = iter;
                do
                {
                    wall.points.push_back( make_source_point( iter ) );
                    ++iter;
                }
                while( iter != start );
                walls.emplace_back( wall );
            }
        }
    }*/

} // namespace

/*
namespace analysis
{

void Compilation::findSpaceFaces( Space::Ptr pSpace, FaceHandleSet& faces, FaceHandleSet& spaceFaces )
{

THROW_RTE( "TODO" );
//check orientation
Polygon2D spaceContourPolygon = pSpace->getContourPolygon().get();
if( wykobi::polygon_orientation( spaceContourPolygon ) == wykobi::Clockwise )
    std::reverse( spaceContourPolygon.begin(), spaceContourPolygon.end() );
const Transform transform = pSpace->getAbsoluteTransform();
for( Point2D& pt : spaceContourPolygon )
    transform.transform( pt.x, pt.y );

std::vector< FaceHandleSet::iterator > removals;
for( FaceHandleSet::iterator i = faces.begin(); i != faces.end(); ++i )
{
    FaceHandle hFace = *i;
    if( !hFace->is_unbounded() )
    {
        //does the floor belong to the space?
        const Point2D ptInterior = getFaceInteriorPoint( hFace );
        if( wykobi::point_in_polygon( ptInterior, spaceContourPolygon ) )
        {
            spaceFaces.insert( hFace );
            removals.push_back( i );
        }
    }
}

for( std::vector< FaceHandleSet::iterator >::reverse_iterator
        i = removals.rbegin(),
        iEnd = removals.rend(); i!=iEnd; ++i )
{
    faces.erase( *i );
}
}*/
/*
void Compilation::recursePolyMap( Site::Ptr pSite, SpacePolyMap& spacePolyMap,
        FaceHandleSet& floorFaces, FaceHandleSet& fillerFaces )
{

    THROW_RTE( "TODO" );

    //bottom up recursion
    for( Site::Ptr pNestedSite : pSite->getSites() )
    {
        recursePolyMap( pNestedSite, spacePolyMap, floorFaces, fillerFaces );
    }

    if( Space::Ptr pSpace = boost::dynamic_pointer_cast< Space >( pSite ) )
    {
        FaceHandleSet spaceFloors;
        findSpaceFaces( pSpace, floorFaces, spaceFloors );

        FaceHandleSet spaceFillers;
        findSpaceFaces( pSpace, fillerFaces, spaceFillers );

        SpacePolyInfo::Ptr pSpacePolyInfo( new SpacePolyInfo );
        {
            for( FaceHandle hFloor : spaceFloors )
            {
                PolygonWithHoles polygon;
                faceToPolygonWithHoles( hFloor, polygon );
                pSpacePolyInfo->floors.emplace_back( polygon );
            }

            for( FaceHandle hFiller : spaceFillers )
            {
                Polygon2D polygon;
                faceToPolygon( hFiller, polygon );
                pSpacePolyInfo->fillers.emplace_back( polygon );
            }

            //determine walls
            for( FaceHandle hFloor : spaceFloors )
            {
                floorToWalls( hFloor, pSpacePolyInfo->walls );
            }

        }

        spacePolyMap.insert( std::make_pair( pSpace, pSpacePolyInfo ) );

    }

}*/
/*
void Compilation::getSpacePolyMap( SpacePolyMap& spacePolyMap )
{
    //get all the floors and faces
    FaceHandleSet floorFaces, fillerFaces;
    getFaces( floorFaces, fillerFaces );

    for( Site::Ptr pSite : m_pmission->getSites() )
    {
        recursePolyMap( pSite, spacePolyMap, floorFaces, fillerFaces );
    }

}

}*/