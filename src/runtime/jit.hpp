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

#ifndef JIT_8_AUG_2022
#define JIT_8_AUG_2022

#include "runtime/functions.hpp"

#include <memory>
#include <set>

namespace mega::runtime
{

class JITCompiler
{
public:
    JITCompiler();

    class Module
    {
    protected:
        friend class JITCompiler;
        virtual ~Module();

    public:
        using Ptr = std::shared_ptr< Module >;

        virtual mega::runtime::SetAllocatorFunction getSetAllocator( const std::string& strSymbol ) = 0;
        virtual mega::runtime::GetHeapFunction      getGetHeap( const std::string& strSymbol )      = 0;
        virtual mega::runtime::GetSharedFunction    getGetShared( const std::string& strSymbol )    = 0;
        virtual mega::runtime::SharedCtorFunction   getSharedCtor( const std::string& strSymbol )   = 0;
        virtual mega::runtime::SharedDtorFunction   getSharedDtor( const std::string& strSymbol )   = 0;
        virtual mega::runtime::HeapCtorFunction     getHeapCtor( const std::string& strSymbol )     = 0;
        virtual mega::runtime::HeapDtorFunction     getHeapDtor( const std::string& strSymbol )     = 0;
        virtual mega::runtime::AllocateFunction     getAllocate( const std::string& strSymbol )     = 0;
        virtual mega::runtime::ReadFunction         getRead( const std::string& strSymbol )         = 0;
        virtual mega::runtime::WriteFunction        getWrite( const std::string& strSymbol )        = 0;
        virtual mega::runtime::CallFunction         getCall( const std::string& strSymbol )         = 0;
        virtual mega::runtime::StartFunction        getStart( const std::string& strSymbol )        = 0;
        virtual mega::runtime::StopFunction         getStop( const std::string& strSymbol )         = 0;
    };

    Module::Ptr compile( const std::string& strModule );

private:
    void unload( Module* pModule );

    struct StaticInit
    {
        StaticInit();
    };
    static StaticInit m_staticInit;

    class Pimpl;
    std::shared_ptr< Pimpl > m_pPimpl;
};

} // namespace mega::runtime

#endif // JIT_8_AUG_2022
