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

#ifndef VISIBILITY_15_DEC_2020
#define VISIBILITY_15_DEC_2020

#include "schematic/cgalSettings.hpp"
#include "schematic/schematic.hpp"

#include <memory>

namespace analysis
{

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class FloorAnalysis
{
    using Arrangement  = ::exact::Arrangement;
    using Point        = ::exact::Point;
    using Curve_handle = ::exact::Curve_handle;
    using Curve        = ::exact::Curve;
    using Segment      = ::exact::Segment;

    friend class Analysis;
    FloorAnalysis();

public:
    using VertexHandle = Arrangement::Vertex_const_handle;

    FloorAnalysis( Compilation& compilation, boost::shared_ptr< schematic::Schematic > pSchematic );

    const Arrangement&                   getFloor() const { return m_arr; }
    const Arrangement::Face_const_handle getFloorFace() const { return m_hFloorFace; }

    bool                     isWithinFloor( VertexHandle v1, VertexHandle v2 ) const;
    boost::optional< Curve > getFloorBisector( VertexHandle v1, VertexHandle v2, bool bKeepSingleEnded ) const;
    boost::optional< Curve > getFloorBisector( const Segment& segment, bool bKeepSingleEnded ) const;

    void render( const boost::filesystem::path& filepath ) const;

    void save( std::ostream& os ) const;
    void load( std::istream& is );

private:
    void findFloorFace();
    void calculateBounds();

    void recurseObjects( schematic::Site::Ptr pSpace );

    mutable Arrangement      m_arr;
    Arrangement::Face_handle m_hFloorFace;
    exact::Rect              m_boundingBox;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class Visibility
{
    using Arrangement  = ::exact::Arrangement;
    using Point        = ::exact::Point;
    using Curve_handle = ::exact::Curve_handle;
    using Curve        = ::exact::Curve;
    using Segment      = ::exact::Segment;

    friend class Analysis;

    Visibility();

public:
    Visibility( FloorAnalysis& floor );

    const Arrangement& getArrangement() const { return m_arr; }

    void render( const boost::filesystem::path& filepath ) const;

    void save( std::ostream& os ) const;
    void load( std::istream& is );

private:
    Arrangement m_arr;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class Analysis
{
    using Arrangement  = ::exact::Arrangement;
    using Point        = ::exact::Point;
    using Curve_handle = ::exact::Curve_handle;
    using Curve        = ::exact::Curve;
    using Segment      = ::exact::Segment;

    Analysis();
    Analysis( boost::shared_ptr< schematic::Schematic > pSchematic );

public:
    using Ptr = std::shared_ptr< Analysis >;

    static Ptr constructFrommission( boost::shared_ptr< schematic::Schematic > pSchematic );
    static Ptr constructFromStream( std::istream& is );

    struct IPainter
    {
        virtual void moveTo( float x, float y ) = 0;
        virtual void lineTo( float x, float y ) = 0;
    };

    void renderFloor( IPainter& painter ) const;

    void save( std::ostream& os ) const;

private:
    Compilation   m_compilation;
    FloorAnalysis m_floor;
    Visibility    m_visibility;
};

} // namespace analysis

#endif // VISIBILITY_15_DEC_2020