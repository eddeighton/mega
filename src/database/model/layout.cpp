//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

#include "database/model/layout.hpp"
#include "database/model/concrete.hpp"

namespace mega
{

void DataMember::load( io::Loader& loader )
{
    m_pDimension = loader.loadObjectRef< concrete::Dimension >();
    loader.load( m_name );
    m_pBuffer = loader.loadObjectRef< Buffer >();
}

void DataMember::store( io::Storer& storer ) const
{
    storer.storeObjectRef( m_pDimension );
    storer.store( m_name );
    storer.storeObjectRef( m_pBuffer );
}

void Buffer::load( io::Loader& loader )
{
    m_pContext = loader.loadObjectRef< concrete::Action >();
    loader.load( m_size );
    loader.load( m_name );
    loader.load( m_variable );
    loader.load( m_simple );
    loader.loadObjectVector( m_dataMembers );
}

void Buffer::store( io::Storer& storer ) const
{
    storer.storeObjectRef( m_pContext );
    storer.store( m_size );
    storer.store( m_name );
    storer.store( m_variable );
    storer.store( m_simple );
    storer.storeObjectVector( m_dataMembers );
}

void Layout::load( io::Loader& loader )
{
    loader.loadObjectVector( m_buffers );
    loader.loadObjectMap( m_dimensionMap );
}

void Layout::store( io::Storer& storer ) const
{
    storer.storeObjectVector( m_buffers );
    storer.storeObjectMap( m_dimensionMap );
}

} // namespace mega