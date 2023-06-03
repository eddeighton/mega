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

#ifndef CUT_3_JUN_2023
#define CUT_3_JUN_2023

#include "schematic/cgalSettings.hpp"
#include "schematic/site.hpp"

namespace schematic
{

class Cut : public Site, public boost::enable_shared_from_this< Cut >
{
public:
    using Ptr    = boost::shared_ptr< Cut >;
    using PtrCst = boost::shared_ptr< const Cut >;

    static const std::string& TypeName();

    Cut( Node::Ptr pParent, const std::string& strName );
    Cut( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName );

    virtual Node::Ptr    copy( Node::Ptr pParent, const std::string& strName ) const;
    virtual Node::PtrCst getPtr() const { return shared_from_this(); }
    virtual Node::Ptr    getPtr() { return shared_from_this(); }

    virtual void load( const format::Node& node );
    virtual void save( format::Node& node ) const;

    virtual std::string getStatement() const;
    virtual void        init();
    virtual void        init( const Transform& transform );

    // schematic tasks
    virtual bool task_contour();

    const Segment& getSegment() const { return m_segment; }

    virtual Feature_Contour::Ptr getContour() const { return ( Feature_Contour::Ptr() ); }
    virtual Rect                 getAABB() const;

private:
    Polygon calculateContour();

private:
    Feature_LineSegment::Ptr m_pLineSegment;

    Segment            m_segment;
    Polygon            m_siteContourCache;
    Timing::UpdateTick m_siteContourTick;
};

} // namespace schematic

#endif // CUT_3_JUN_2023