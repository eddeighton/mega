
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

#ifndef GUARD_2023_January_10_program
#define GUARD_2023_January_10_program

#include "api.hpp"
#include "orc.hpp"

#include "program_functions.hxx"

#include "environment/jit_database.hpp"

#include <memory>

namespace mega::runtime
{
class JIT_EXPORT Program
{
public:
    using Ptr = std::shared_ptr< Program >;

    Program( JITDatabase& database, JITCompiler::Module::Ptr pModule );

    program::ObjectSaveBin::FunctionPtr getObjectSaveBin() const { return m_objectSaveBin; }
    program::ObjectLoadBin::FunctionPtr getObjectLoadBin() const { return m_objectLoadBin; }
    program::RecordLoadBin::FunctionPtr getRecordLoadBin() const { return m_recordLoadBin; }
    program::RecordMake::FunctionPtr    getRecordMake() const { return m_recordMake; }
    program::RecordBreak::FunctionPtr   getRecordBreak() const { return m_recordBreak; }
    program::Traverse::FunctionPtr      getTraverse() const { return m_traverse; }
    program::LinkSize::FunctionPtr      getLinkSize() const { return m_pLinkSize; }
    program::LinkGet::FunctionPtr       getLinkGet() const { return m_pLinkGet; }
    program::ReadAny::FunctionPtr       getReadAny() const { return m_pReadAny; }
    program::WriteAny::FunctionPtr      getWriteAny() const { return m_pWriteAny; }
    program::Enumerate::FunctionPtr     getEnumerate() const { return m_pEnumerate; }
    program::Dispatch::FunctionPtr      getDispatch() const { return m_pDispatch; }

private:
    JITCompiler::Module::Ptr            m_pModule;
    program::ObjectSaveBin::FunctionPtr m_objectSaveBin = nullptr;
    program::ObjectLoadBin::FunctionPtr m_objectLoadBin = nullptr;
    program::RecordLoadBin::FunctionPtr m_recordLoadBin = nullptr;
    program::RecordMake::FunctionPtr    m_recordMake    = nullptr;
    program::RecordBreak::FunctionPtr   m_recordBreak   = nullptr;
    program::Traverse::FunctionPtr      m_traverse      = nullptr;
    program::LinkSize::FunctionPtr      m_pLinkSize     = nullptr;
    program::LinkGet::FunctionPtr       m_pLinkGet      = nullptr;
    program::ReadAny::FunctionPtr       m_pReadAny      = nullptr;
    program::WriteAny::FunctionPtr      m_pWriteAny     = nullptr;
    program::Enumerate::FunctionPtr     m_pEnumerate    = nullptr;
    program::Dispatch::FunctionPtr      m_pDispatch     = nullptr;
};

} // namespace mega::runtime

#endif // GUARD_2023_January_10_program
