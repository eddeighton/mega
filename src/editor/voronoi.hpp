

#ifndef VORONOI_19_FEB_2021
#define VORONOI_19_FEB_2021

#include "map/cgalSettings.hpp"

#include "boost/polygon/voronoi.hpp"

namespace boost {
namespace polygon {

    template <>
    struct geometry_concept< map::Point > 
    {
      typedef point_concept type;
    };

    template <>
    struct point_traits< map::Point > 
    {
      typedef int coordinate_type;

      static inline coordinate_type get(
          const map::Point& point, orientation_2d orient ) 
      {
        return (orient == HORIZONTAL) ? point.x() : point.y();
      }
    };

    template <>
    struct geometry_concept< map::Segment > 
    {
      typedef segment_concept type;
    };

    template <>
    struct segment_traits< map::Segment > 
    {
      typedef int coordinate_type;
      typedef map::Point point_type;

      static inline point_type get( const map::Segment& segment, direction_1d dir ) 
      {
        return dir.to_int() ? segment[ 1 ] : segment[ 0 ];
      }
    };

}  // polygon
}  // boost

#endif //VORONOI_19_FEB_2021