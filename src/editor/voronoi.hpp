

#ifndef VORONOI_19_FEB_2021
#define VORONOI_19_FEB_2021

#include "schematic/cgalSettings.hpp"

#include "boost/polygon/voronoi.hpp"

namespace boost {
namespace polygon {

    template <>
    struct geometry_concept< schematic::Point > 
    {
      typedef point_concept type;
    };

    template <>
    struct point_traits< schematic::Point > 
    {
      typedef int coordinate_type;

      static inline coordinate_type get(
          const schematic::Point& point, orientation_2d orient ) 
      {
        return (orient == HORIZONTAL) ? point.x() : point.y();
      }
    };

    template <>
    struct geometry_concept< schematic::Segment > 
    {
      typedef segment_concept type;
    };

    template <>
    struct segment_traits< schematic::Segment > 
    {
      typedef int coordinate_type;
      typedef schematic::Point point_type;

      static inline point_type get( const schematic::Segment& segment, direction_1d dir ) 
      {
        return dir.to_int() ? segment[ 1 ] : segment[ 0 ];
      }
    };

}  // polygon
}  // boost

#endif //VORONOI_19_FEB_2021