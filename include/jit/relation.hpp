
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

#ifndef GUARD_2023_January_20_relation
#define GUARD_2023_January_20_relation

#include "api.hpp"
#include "orc.hpp"

#include "relation_functions.hxx"

#include "database/database.hpp"

#include "mega/relation_id.hpp"

#include <memory>

namespace mega::runtime
{
class JIT_EXPORT Relation
{
public:
    using Ptr = std::shared_ptr< Relation >;

    Relation( const RelationID& relationID, DatabaseInstance& database, JITCompiler::Module::Ptr pModule );

    relation::LinkMake::FunctionPtr      getMake() const { return m_pMake; }
    relation::LinkBreak::FunctionPtr     getBreak() const { return m_pBreak; }
    relation::LinkOverwrite::FunctionPtr getOverwrite() const { return m_pOverwrite; }
    relation::LinkReset::FunctionPtr     getReset() const { return m_pReset; }

private:
    RelationID                           m_relationID;
    JITCompiler::Module::Ptr             m_pModule;
    relation::LinkMake::FunctionPtr      m_pMake      = nullptr;
    relation::LinkBreak::FunctionPtr     m_pBreak     = nullptr;
    relation::LinkOverwrite::FunctionPtr m_pOverwrite = nullptr;
    relation::LinkReset::FunctionPtr     m_pReset     = nullptr;
};
} // namespace mega::runtime

#endif // GUARD_2023_January_20_relation