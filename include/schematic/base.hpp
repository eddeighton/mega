
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

#ifndef BASE_30_JAN_2021
#define BASE_30_JAN_2021

#include "schematic/schematic.hpp"

namespace schematic
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class Base : public Schematic, public boost::enable_shared_from_this< Base >
{
public:
    typedef boost::shared_ptr< Base >       Ptr;
    typedef boost::shared_ptr< const Base > PtrCst;

    static const std::string& TypeName();

    Base( const std::string& strName );
    Base( PtrCst pOriginal, Node::Ptr pParent, const std::string& strName );

    virtual Node::PtrCst getPtr() const { return shared_from_this(); }
    virtual Node::Ptr    getPtr() { return shared_from_this(); }

    virtual void      init() { Schematic::init(); }
    virtual Node::Ptr copy( Node::Ptr pParent, const std::string& strName ) const;

    virtual void load( const format::File& file );
    virtual void save( format::File& file ) const;

    virtual std::string getStatement() const { return getName(); }
};

} // namespace schematic

#endif // BASE_30_JAN_2021