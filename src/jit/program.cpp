
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

#include "jit/program.hpp"

#include "jit.hpp"
#include "symbol.hpp"

#include "service/network/log.hpp"

#include <sstream>

namespace mega::runtime
{

Program::Program( JITDatabase& database, JITCompiler::Module::Ptr pModule )
    : m_pModule( pModule )
{
    SPDLOG_TRACE( "Program::ctor" );
    m_objectSaveBin = m_pModule->get< program::ObjectSaveBin::FunctionPtr >(
        Symbol( "object_save_bin", Symbol::ID_VStar_VStar ) );
    m_objectLoadBin = m_pModule->get< program::ObjectLoadBin::FunctionPtr >( 
        Symbol( "object_load_bin", Symbol::ID_VStar_VStar ) );
    m_recordLoadBin = m_pModule->get< program::RecordLoadBin::FunctionPtr >( 
        Symbol( "record_load_bin", Symbol::Ref_VStar_U64 ) );
    m_recordMake = m_pModule->get< program::RecordMake::FunctionPtr >( 
        Symbol( "record_make", Symbol::Ref_Ref ) );
    m_recordBreak = m_pModule->get< program::RecordBreak::FunctionPtr >( 
        Symbol( "record_break", Symbol::Ref_Ref ) );
}
} // namespace mega::runtime