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

#ifndef LOCATION_30_JAN_2021
#define LOCATION_30_JAN_2021

#include "schematic/schematic.hpp"
#include "schematic/glyphSpecProducer.hpp"

namespace schematic
{

class Sector;
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class Location : public GlyphSpecProducer, public Origin, public boost::enable_shared_from_this< Location >
{
public:
    using Ptr    = boost::shared_ptr< Location >;
    using PtrCst = boost::shared_ptr< const Location >;

    static const std::string& TypeName();

    Location( boost::shared_ptr< Sector > pParent, const std::string& strName );
    Location( PtrCst pOriginal, boost::shared_ptr< Sector > pParent, const std::string& strName );

    virtual Node::PtrCst getPtr() const { return shared_from_this(); }
    virtual Node::Ptr    getPtr() { return shared_from_this(); }

    virtual void      init() { GlyphSpecProducer::init(); }
    virtual Node::Ptr copy( Node::Ptr pParent, const std::string& strName ) const;

    virtual std::string getStatement() const { return getName(); }

    // GlyphSpec
    virtual bool                     canEdit() const { return true; }
    virtual const std::string&       getName() const { return Node::getName(); }
    virtual const GlyphSpec*         getParent() const { return nullptr; }
    virtual CompilationStage         getCompilationStage() const;
    virtual const GlyphSpecProducer* getProducer() const { return this; }
};
} // namespace schematic

#endif // LOCATION_30_JAN_2021