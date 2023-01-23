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

#include "api.hpp"
#include "orc.hpp"
#include "mega/memory.hpp"

#include "database/database.hpp"

#include <memory>
#include <unordered_map>

namespace mega::runtime
{

class JIT_EXPORT Allocator
{
public:
    using ObjectCtorFunction             = void ( * )( void* );
    using ObjectDtorFunction             = void ( * )( void* );
    using ObjectSaveBinFunction          = void ( * )( void*, void* );
    using ObjectLoadBinFunction          = void ( * )( void*, void* );
    using ObjectSaveXMLStructureFunction = void ( * )( void*, const char*, void* );
    using ObjectLoadXMLStructureFunction = void ( * )( void*, const char*, void* );
    using ObjectSaveXMLFunction          = void ( * )( void*, const char*, void* );
    using ObjectLoadXMLFunction          = void ( * )( void*, const char*, void* );

    Allocator( TypeID objectTypeID, DatabaseInstance& database, JITCompiler::Module::Ptr pModule );

    using Ptr = std::shared_ptr< Allocator >;

    const mega::SizeAlignment& getSizeAlignment() const { return m_sizeAlignment; }

    ObjectCtorFunction             getCtor() const { return m_pCtor; }
    ObjectDtorFunction             getDtor() const { return m_pDtor; }
    ObjectSaveBinFunction          getSaveBin() const { return m_pSaveBin; }
    ObjectLoadBinFunction          getLoadBin() const { return m_pLoadBin; }
    ObjectSaveXMLStructureFunction getSaveXMLStructure() const { return m_pSaveXMLStructure; }
    ObjectLoadXMLStructureFunction getLoadXMLStructure() const { return m_pLoadXMLStructure; }
    ObjectSaveXMLFunction          getSaveXML() const { return m_pSaveXML; }
    ObjectLoadXMLFunction          getLoadXML() const { return m_pLoadXML; }

private:
    JITCompiler::Module::Ptr       m_pModule;
    TypeID                         m_objectTypeID;
    mega::SizeAlignment            m_sizeAlignment;
    ObjectCtorFunction             m_pCtor             = nullptr;
    ObjectDtorFunction             m_pDtor             = nullptr;
    ObjectSaveBinFunction          m_pSaveBin          = nullptr;
    ObjectLoadBinFunction          m_pLoadBin          = nullptr;
    ObjectSaveXMLStructureFunction m_pSaveXMLStructure = nullptr;
    ObjectLoadXMLStructureFunction m_pLoadXMLStructure = nullptr;
    ObjectSaveXMLFunction          m_pSaveXML          = nullptr;
    ObjectLoadXMLFunction          m_pLoadXML          = nullptr;
};

} // namespace mega::runtime

#endif // OBJECT_ALLOCATOR_SEPT_19_2022
