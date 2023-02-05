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
        symbolPrefix( "ctor_", objectTypeID, os );
        os << "Pv";
        m_pCtor = pModule->get< object::ObjectCtor::FunctionPtr >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "dtor_", objectTypeID, os );
        os << "Pv";
        m_pDtor = pModule->get< object::ObjectDtor::FunctionPtr >( os.str() );
    }

    {
        std::ostringstream os;
        symbolPrefix( "save_object_bin_", objectTypeID, os );
        os << "PvS_";
        m_pSaveBin = pModule->get< object::ObjectSaveBin::FunctionPtr >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "load_object_bin_", objectTypeID, os );
        os << "PvS_";
        m_pLoadBin = pModule->get< object::ObjectLoadBin::FunctionPtr >( os.str() );
    }

    {
        std::ostringstream os;
        symbolPrefix( "save_object_xml_structure_", objectTypeID, os );
        os << "N4mega9referenceEPv";
        m_pSaveXMLStructure = pModule->get< object::ObjectSaveXMLStructure::FunctionPtr >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "load_object_xml_structure_", objectTypeID, os );
        os << "N4mega9referenceEPv";
        m_pLoadXMLStructure = pModule->get< object::ObjectLoadXMLStructure::FunctionPtr >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "save_object_xml_", objectTypeID, os );
        os << "N4mega9referenceEPv";
        m_pSaveXML = pModule->get< object::ObjectSaveXML::FunctionPtr >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "load_object_xml_", objectTypeID, os );
        os << "N4mega9referenceEPv";
        m_pLoadXML = pModule->get< object::ObjectLoadXML::FunctionPtr >( os.str() );
    }
}

} // namespace mega::runtime
