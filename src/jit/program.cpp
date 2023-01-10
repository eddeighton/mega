
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
#include "symbol_utils.hpp"

#include "service/network/log.hpp"

#include <sstream>

namespace mega::runtime
{

Program::Program( DatabaseInstance& database, JITCompiler::Module::Ptr pModule )
    : m_pModule( pModule )
{
    SPDLOG_TRACE( "Program::ctor" );
    {
        std::ostringstream os;
        symbolPrefix( "program_load_record", os );
        os << "N4mega9referenceEPKv";
        m_loadRecord = m_pModule->get< LoadRecordFunction >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "program_save_record", os );
        os << "N4mega9referenceEPKv";
        m_saveRecord = m_pModule->get< SaveRecordFunction >( os.str() );
    }
}
} // namespace mega::runtime