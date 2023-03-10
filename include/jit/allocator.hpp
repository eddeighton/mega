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

#include "object_functions.hxx"

#include "mega/type_id.hpp"
#include "mega/memory.hpp"

#include "database/database.hpp"

#include <memory>
#include <unordered_map>

namespace mega::runtime
{

class JIT_EXPORT Allocator
{
public:
    Allocator( TypeID objectTypeID, DatabaseInstance& database, JITCompiler::Module::Ptr pModule );

    using Ptr = std::shared_ptr< Allocator >;

    const mega::SizeAlignment& getSizeAlignment() const { return m_sizeAlignment; }

    object::ObjectCtor::FunctionPtr             getCtor() const { return m_pCtor; }
    object::ObjectDtor::FunctionPtr             getDtor() const { return m_pDtor; }
    object::ObjectSaveBin::FunctionPtr          getSaveBin() const { return m_pSaveBin; }
    object::ObjectLoadBin::FunctionPtr          getLoadBin() const { return m_pLoadBin; }
    object::ObjectSaveXMLStructure::FunctionPtr getSaveXMLStructure() const { return m_pSaveXMLStructure; }
    object::ObjectLoadXMLStructure::FunctionPtr getLoadXMLStructure() const { return m_pLoadXMLStructure; }
    object::ObjectSaveXML::FunctionPtr          getSaveXML() const { return m_pSaveXML; }
    object::ObjectLoadXML::FunctionPtr          getLoadXML() const { return m_pLoadXML; }

private:
    JITCompiler::Module::Ptr                    m_pModule;
    TypeID                                      m_objectTypeID;
    mega::SizeAlignment                         m_sizeAlignment;
    object::ObjectCtor::FunctionPtr             m_pCtor             = nullptr;
    object::ObjectDtor::FunctionPtr             m_pDtor             = nullptr;
    object::ObjectSaveBin::FunctionPtr          m_pSaveBin          = nullptr;
    object::ObjectLoadBin::FunctionPtr          m_pLoadBin          = nullptr;
    object::ObjectSaveXMLStructure::FunctionPtr m_pSaveXMLStructure = nullptr;
    object::ObjectLoadXMLStructure::FunctionPtr m_pLoadXMLStructure = nullptr;
    object::ObjectSaveXML::FunctionPtr          m_pSaveXML          = nullptr;
    object::ObjectLoadXML::FunctionPtr          m_pLoadXML          = nullptr;
};

} // namespace mega::runtime

#endif // OBJECT_ALLOCATOR_SEPT_19_2022
