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

#include "map/cgalSettings.hpp"
#include "map/geometry.hpp"
#include "map/space.hpp"
#include "map/connection.hpp"
#include "map/transform.hpp"
#include "map/compilation.hpp"

#include <vector>

namespace
{
/*
inline map::Point2D make_source_point( map::Arrangement::Halfedge_const_handle halfedge )
{
    return wykobi::make_point< float >(
            CGAL::to_double( halfedge->source()->point().x() ),
            CGAL::to_double( halfedge->source()->point().y() ) );
}
inline map::Point2D make_target_point( map::Arrangement::Halfedge_const_handle halfedge )
{
    return wykobi::make_point< float >(
            CGAL::to_double( halfedge->target()->point().x() ),
            CGAL::to_double( halfedge->target()->point().y() ) );
}


map::Point2D getFaceInteriorPoint( map::Arrangement::Face_const_handle hFace )
{
    map::Arrangement::Ccb_halfedge_const_circulator halfedgeCirculator = hFace->outer_ccb();
    map::Arrangement::Halfedge_const_handle hEdge = halfedgeCirculator;

    //deterine an interior point of the face
    const map::Point2D ptSource = make_source_point( hEdge );
    const map::Point2D ptTarget = make_target_point( hEdge );

    const map::Vector2D vDir = ptTarget - ptSource;
    const map::Vector2D vNorm =
        wykobi::normalize(
            wykobi::make_vector< float >( -vDir.y, vDir.x ) );
    const map::Point2D ptMid = ptSource + vDir * 0.5f;
    const map::Point2D ptInterior = ptMid + vNorm * 0.1f;

    return ptInterior;
}  */
/*
void faceToPolygon( map::Arrangement::Face_const_handle hFace, map::Polygon2D& polygon )
{
    VERIFY_RTE( !hFace->is_unbounded() );

    map::Arrangement::Ccb_halfedge_const_circulator iter = hFace->outer_ccb();
    map::Arrangement::Ccb_halfedge_const_circulator first = iter;
    do
    {
        polygon.push_back( make_source_point( iter ) );
        ++iter;
    }
    while( iter != first );
}

void faceToPolygonWithHoles( map::Arrangement::Face_const_handle hFace, map::PolygonWithHoles& polygon )
{
    if( !hFace->is_unbounded() )
    {
        map::Arrangement::Ccb_halfedge_const_circulator iter = hFace->outer_ccb();
        map::Arrangement::Ccb_halfedge_const_circulator first = iter;
        do
        {
            polygon.outer.push_back( make_source_point( iter ) );
            ++iter;
        }
        while( iter != first );
    }

    for( map::Arrangement::Hole_const_iterator
        holeIter = hFace->holes_begin(),
        holeIterEnd = hFace->holes_end();
            holeIter != holeIterEnd; ++holeIter )
    {
        map::Polygon2D hole;
        map::Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
        map::Arrangement::Ccb_halfedge_const_circulator start = iter;
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
            map::Arrangement::Halfedge_const_handle hStart,
            map::Arrangement::Halfedge_const_handle hEnd,
            map::Wall& wall )
    {
        THROW_RTE( "TODO" );

        map::Arrangement::Halfedge_const_handle hIter = hStart;
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
    void floorToWalls( map::Arrangement::Face_const_handle hFloor, std::vector< map::Wall >& walls )
    {
        THROW_RTE( "TODO" );

        using DoorStepVector = std::vector< map::Arrangement::Halfedge_const_handle >;

        if( !hFloor->is_unbounded() )
        {
            //find the doorsteps if any
            DoorStepVector doorsteps;
            {
                //outer ccb winds COUNTERCLOCKWISE around the outer contour
                map::Arrangement::Ccb_halfedge_const_circulator iter = hFloor->outer_ccb();
                map::Arrangement::Ccb_halfedge_const_circulator first = iter;
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

                    map::Wall wall( false, true );
                    wallSection( *i, *iNext, wall );
                    walls.emplace_back( wall );
                }
            }
            else
            {
                map::Wall wall( true, true );
                map::Arrangement::Ccb_halfedge_const_circulator iter = hFloor->outer_ccb();
                map::Arrangement::Ccb_halfedge_const_circulator start = iter;
                do
                {
                    wall.points.push_back( make_source_point( iter ) );
                    ++iter;
                }
                while( iter != start );
                walls.emplace_back( wall );
            }
        }

        for( map::Arrangement::Hole_const_iterator
            holeIter = hFloor->holes_begin(),
            holeIterEnd = hFloor->holes_end();
                holeIter != holeIterEnd; ++holeIter )
        {
            DoorStepVector doorsteps;
            {
                //the hole circulators wind CLOCKWISE around the hole contours
                map::Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
                map::Arrangement::Ccb_halfedge_const_circulator start = iter;
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

                    map::Wall wall( false, false );
                    wallSection( *i, *iNext, wall );
                    walls.emplace_back( wall );
                }
            }
            else
            {
                map::Wall wall( true, false );
                map::Arrangement::Ccb_halfedge_const_circulator iter = *holeIter;
                map::Arrangement::Ccb_halfedge_const_circulator start = iter;
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
namespace map
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