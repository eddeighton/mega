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

#include "symbol.hpp"

#include "database/jit_database.hpp"

#include "service/network/log.hpp"

namespace mega::runtime
{

Allocator::Allocator( TypeID objectTypeID, JITDatabase& database, JITCompiler::Module::Ptr pModule )
    : m_pModule( pModule )
    , m_objectTypeID( objectTypeID )
    , m_sizeAlignment( database.getObjectSize( m_objectTypeID ) )
{
    SPDLOG_TRACE( "Allocator::ctor for {}", m_objectTypeID );

    m_pCtor = pModule->get< object::ObjectCtor::FunctionPtr >( 
        Symbol( "ctor_", objectTypeID, Symbol::VStar ) );
    m_pDtor = pModule->get< object::ObjectDtor::FunctionPtr >( 
        Symbol( "dtor_", objectTypeID, Symbol::Ref_VStar_Bool ) );
    m_pSaveBin = pModule->get< object::ObjectSaveBin::FunctionPtr >( 
        Symbol( "save_object_bin_", objectTypeID, Symbol::VStar_VStar ) );
    m_pLoadBin = pModule->get< object::ObjectLoadBin::FunctionPtr >( 
        Symbol( "load_object_bin_", objectTypeID, Symbol::VStar_VStar ) );
    m_pUnparent = pModule->get< object::ObjectUnparent::FunctionPtr >( 
        Symbol( "unparent_", objectTypeID, Symbol::Ref ) );
    m_pTraverse = pModule->get< object::ObjectTraverse::FunctionPtr >( 
        Symbol( "traverse_", objectTypeID, Symbol::VStar ) );
    m_pLinkSize = pModule->get< object::LinkSize::FunctionPtr >( 
        Symbol( "link_size_", objectTypeID, Symbol::Ref ) );
    m_pLinkObject = pModule->get< object::LinkObject::FunctionPtr >( 
        Symbol( "link_object_", objectTypeID, Symbol::Ref_U64 ) );
    m_pReadAny = pModule->get< object::ReadAny::FunctionPtr >( 
        Symbol( "read_any_", objectTypeID, Symbol::Ref ) );
    m_pWriteAny = pModule->get< object::WriteAny::FunctionPtr >( 
        Symbol( "write_any_", objectTypeID, Symbol::Ref_Any ) );
}

} // namespace mega::runtime
