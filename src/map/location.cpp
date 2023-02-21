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

#include "map/location.hpp"
#include "map/sector.hpp"
#include "map/mission.hpp"

namespace map
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
const std::string& Location::TypeName()
{
    static const std::string strTypeName( "location" );
    return strTypeName;
}

Location::Location( Sector::Ptr pParent, const std::string& strName )
    : GlyphSpecProducer( pParent, strName )
{
}

Location::Location( PtrCst pOriginal, Sector::Ptr pParent, const std::string& strName )
    : GlyphSpecProducer( pOriginal, pParent, strName )
{
}

Node::Ptr Location::copy( Node::Ptr pParent, const std::string& strName ) const
{
    Sector::Ptr pSiteParent = boost::dynamic_pointer_cast< Sector >( pParent );
    VERIFY_RTE( pSiteParent || !pParent );
    return Node::copy< Location >(
        boost::dynamic_pointer_cast< const Location >( shared_from_this() ), pSiteParent, strName );
}

CompilationStage Location::getCompilationStage() const
{
    return Mission::eStage_Location;
}

} // namespace map
