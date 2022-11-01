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

#include "allocator.hpp"

#include "jit.hpp"
#include "symbol_utils.hpp"

#include "database/database.hpp"

#include "service/network/log.hpp"

namespace mega::runtime
{

Allocator::Allocator( TypeID objectTypeID, DatabaseInstance& database, JITCompiler::Module::Ptr pModule )
    : m_pModule( pModule )
    , m_objectTypeID( objectTypeID )
    , m_sizeAlignment( database.getObjectSize( m_objectTypeID ) )
{
    SPDLOG_TRACE( "Allocator::ctor for {}", m_objectTypeID );

    {
        std::ostringstream os;
        symbolPrefix( "shared_ctor_", objectTypeID, os );
        os << "PvS_";
        m_pSharedCtor = pModule->get< SharedCtorFunction >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "shared_dtor_", objectTypeID, os );
        os << "Pv";
        m_pSharedDtor = pModule->get< SharedDtorFunction >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "heap_ctor_", objectTypeID, os );
        os << "Pv";
        m_pHeapCtor = pModule->get< HeapCtorFunction >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "heap_dtor_", objectTypeID, os );
        os << "Pv";
        m_pHeapDtor = pModule->get< HeapDtorFunction >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "save_object_bin_", objectTypeID, os );
        os << "N4mega9referenceEPv";
        m_pSaveBin = pModule->get< SaveObjectFunction >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "load_object_bin_", objectTypeID, os );
        os << "N4mega9referenceEPv";
        m_pLoadBin = pModule->get< LoadObjectFunction >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "save_object_xml_", objectTypeID, os );
        os << "N4mega9referenceEPv";
        m_pSaveXML = pModule->get< SaveObjectFunction >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "load_object_xml_", objectTypeID, os );
        os << "N4mega9referenceEPv";
        m_pLoadXML = pModule->get< LoadObjectFunction >( os.str() );
    }
}

} // namespace mega::runtime
