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

#ifndef OBJECT_ALLOCATOR_SEPT_19_2022
#define OBJECT_ALLOCATOR_SEPT_19_2022

#include "functions.hpp"
#include "orc.hpp"

#include "database/database.hpp"

#include "service/protocol/common/jit_types.hpp"

#include <memory>
#include <unordered_map>

namespace mega::runtime
{

class Allocator
{
public:
    Allocator( TypeID objectTypeID, DatabaseInstance& database, JITCompiler::Module::Ptr pModule );

    using Ptr = std::unique_ptr< Allocator >;

    const network::SizeAlignment& getSizeAlignment() const { return m_sizeAlignment; }

    SharedCtorFunction getSharedCtor() const { return m_pSharedCtor; }
    SharedDtorFunction getSharedDtor() const { return m_pSharedDtor; }
    HeapCtorFunction   getHeapCtor() const { return m_pHeapCtor; }
    HeapDtorFunction   getHeapDtor() const { return m_pHeapDtor; }
    SaveObjectFunction getSaveBin() const { return m_pSaveBin; }
    LoadObjectFunction getLoadBin() const { return m_pLoadBin; }
    SaveObjectFunction getSaveXML() const { return m_pSaveXML; }
    LoadObjectFunction getLoadXML() const { return m_pLoadXML; }

private:
    JITCompiler::Module::Ptr m_pModule;
    TypeID                   m_objectTypeID;
    network::SizeAlignment   m_sizeAlignment;
    SharedCtorFunction       m_pSharedCtor = nullptr;
    SharedDtorFunction       m_pSharedDtor = nullptr;
    HeapCtorFunction         m_pHeapCtor   = nullptr;
    HeapDtorFunction         m_pHeapDtor   = nullptr;
    SaveObjectFunction       m_pSaveBin    = nullptr;
    LoadObjectFunction       m_pLoadBin    = nullptr;
    SaveObjectFunction       m_pSaveXML    = nullptr;
    LoadObjectFunction       m_pLoadXML    = nullptr;
};

} // namespace mega::runtime

#endif // OBJECT_ALLOCATOR_SEPT_19_2022
