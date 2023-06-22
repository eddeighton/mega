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

#ifndef GLYPHSPEC_PRODUCER_07_NOV_2020
#define GLYPHSPEC_PRODUCER_07_NOV_2020

#include "glyphSpec.hpp"
#include "node.hpp"

#include "common/stl.hpp"
#include "common/compose.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/optional.hpp>
#include <boost/chrono.hpp>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

namespace schematic
{

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
class GlyphSpecProducer : public Node
{
public:
    using Ptr    = boost::shared_ptr< GlyphSpecProducer >;
    using PtrCst = boost::shared_ptr< const GlyphSpecProducer >;

    GlyphSpecProducer( Node::Ptr pParent, const std::string& strName )
        : Node( pParent, strName )
    {
    }
    GlyphSpecProducer( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName )
        : Node( pOriginal, pParent, strName )
    {
    }

    const ControlPoint::Set& getControlPoints() const { return m_controlPointSet; }

    virtual void getMarkupPolygonGroups( MarkupPolygonGroup::List& ) {}
    virtual void getMarkupTexts( MarkupText::List& ) {}
    virtual void getImages( ImageSpec::List& ) {}
    virtual bool cmd_delete( const std::vector< const GlyphSpec* >& ) { return false; }
    virtual void getImageSpecs() {}

protected:
    ControlPoint::Set m_controlPointSet;
};

} // namespace schematic

#endif // GLYPHSPEC_PRODUCER_07_NOV_2020