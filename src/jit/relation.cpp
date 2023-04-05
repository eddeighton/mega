
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

#include "relation.hpp"

#include "symbol_utils.hpp"

#include "database/jit_database.hpp"

#include "service/network/log.hpp"

namespace mega::runtime
{

Relation::Relation( const RelationID& relationID, JITDatabase& database, JITCompiler::Module::Ptr pModule )
    : m_relationID( relationID )
    , m_pModule( pModule )
{
    SPDLOG_TRACE( "Relation::ctor for {}.{}", relationID.getLower(), relationID.getUpper() );

    {
        std::ostringstream os;
        symbolPrefix( "link_make_", relationID, os );
        os << "N4mega9referenceES0_";
        m_pMake = pModule->get< relation::LinkMake::FunctionPtr >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "link_break_", relationID, os );
        os << "N4mega9referenceES0_";
        m_pBreak = pModule->get< relation::LinkBreak::FunctionPtr >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "link_overwrite_", relationID, os );
        os << "N4mega9referenceES0_";
        m_pOverwrite = pModule->get< relation::LinkOverwrite::FunctionPtr >( os.str() );
    }
    {
        std::ostringstream os;
        symbolPrefix( "link_reset_", relationID, os );
        os << "N4mega9referenceES0_";
        m_pReset = pModule->get< relation::LinkReset::FunctionPtr >( os.str() );
    }
}

} // namespace mega::runtime