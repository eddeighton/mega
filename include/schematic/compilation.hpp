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

#ifndef COMPILATION_15_DEC_2020
#define COMPILATION_15_DEC_2020

#include "schematic/geometry.hpp"
#include "schematic/transform.hpp"
#include "schematic/space.hpp"
#include "schematic/cgalSettings.hpp"

#include "boost/shared_ptr.hpp"
#include "boost/filesystem/path.hpp"

#include <memory>
#include <map>

namespace schematic
{
class Schematic;
}

namespace analysis
{

class Compilation
{
    using Arrangement  = ::exact::Arrangement;
    using Point        = ::exact::Point;
    using Curve_handle = ::exact::Curve_handle;
    using Curve        = ::exact::Curve;

    friend class Analysis;
    Compilation();

public:
    using Ptr = std::shared_ptr< Compilation >;

    Compilation( boost::shared_ptr< schematic::Schematic > pSchematic );

    static void renderContour( Arrangement& arr, const exact::Transform& transform, const exact::Polygon& poly );

    using FaceHandle    = Arrangement::Face_const_handle;
    using FaceHandleSet = std::set< FaceHandle >;
    
    void getFaces( FaceHandleSet& floorFaces, FaceHandleSet& fillerFaces );
    void getEdges( std::vector< schematic::Segment >& edges );

    // html svg utilities
    // void render( const boost::filesystem::path& filepath );
    // void renderFloors( const boost::filesystem::path& filepath );
    // void renderFillers( const boost::filesystem::path& filepath );

    // io
    void save( std::ostream& os ) const;
    void load( std::istream& is );

private:
    void recurse( schematic::Site::Ptr pSpace );
    void recursePost( schematic::Site::Ptr pSpace );
    void connect( schematic::Site::Ptr pConnection );
    void findSpaceFaces( schematic::Space::Ptr pSpace, FaceHandleSet& faces, FaceHandleSet& spaceFaces );

    Arrangement m_arr;
};
} // namespace analysis

#endif // COMPILATION_15_DEC_2020