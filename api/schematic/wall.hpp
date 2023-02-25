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

#ifndef WALL_01_DEC_2020
#define WALL_01_DEC_2020

#include "schematic/site.hpp"

namespace schematic
{

class Wall : public Site, public boost::enable_shared_from_this< Wall >
{
public:
    using Ptr    = boost::shared_ptr< Wall >;
    using PtrCst = boost::shared_ptr< const Wall >;

    static const std::string& TypeName();
    Wall( Node::Ptr pParent, const std::string& strName );
    Wall( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName );
    virtual Node::Ptr    copy( Node::Ptr pParent, const std::string& strName ) const;
    virtual Node::PtrCst getPtr() const { return shared_from_this(); }
    virtual Node::Ptr    getPtr() { return shared_from_this(); }

    virtual void load( const format::Site& site );
    void         save( format::Site& site ) const;

    virtual std::string getStatement() const;
    void                init();
    void                init( const Transform& transform );

    // schematic tasks
    virtual bool task_contour();
    virtual void task_extrusions();

    virtual Feature_Contour::Ptr getContour() const { return m_pContour; }
    virtual Rect                 getAABB() const;

    const exact::Polygon& getExteriorPolygon() const { return m_exteriorPolygon; }

private:
    Feature_Contour::Ptr m_pContour;

    Polygon            m_siteContourCache;
    Timing::UpdateTick m_siteContourTick;

    exact::Polygon m_sitePolygon;
    exact::Polygon m_exteriorPolygon;
};

} // namespace schematic

#endif // WALL_01_DEC_2020