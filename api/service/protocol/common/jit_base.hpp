
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

#ifndef GUARD_2023_January_11_jit_base
#define GUARD_2023_January_11_jit_base

#include "mega/invocation_id.hpp"
#include "mega/relation_id.hpp"

#include "common/unreachable.hpp"

#include <functional>

namespace mega::runtime
{

class JITBase
{
public:
    virtual void getProgramFunction( void* pLLVMCompiler, int functionType, void** ppFunction ) = 0;
    virtual void getInvocationFunction( void* pLLVMCompiler, const char* pszUnitName,
                                        const mega::InvocationID& invocationID, int functionType, void** ppFunction )
        = 0;
    virtual void getObjectFunction( void* pLLVMCompiler, const char* pszUnitName, mega::TypeID typeID, int functionType,
                                    void** ppFunction )
        = 0;
    virtual void getRelationFunction( void* pLLVMCompiler, const char* pszUnitName, const RelationID& relationID,
                                      int functionType, void** ppFunction )
        = 0;

    struct ActionInfo
    {
        enum Type
        {
            eCycle,
            eAction,
            TOTAL_TYPES
        };
        Type type;
    };
    virtual void getActionFunction( mega::TypeID typeID, void** ppFunction, ActionInfo& actionInfo ) = 0;

    virtual void getOperatorFunction( void* pLLVMCompiler, const char* pszUnitName, TypeID target, int functionType,
                                      void** ppFunction )
        = 0;
};

class JITFunctor
{
public:
    using ExecutionFPtr = std::function< void( JITBase&, void* pLLVMCompiler ) >;

    JITFunctor() { UNREACHABLE; }

    JITFunctor( ExecutionFPtr&& functor )
        : m_functor( std::move( functor ) )
    {
    }

    inline void operator()( JITBase& jit, void* pLLVMCompiler ) const { m_functor( jit, pLLVMCompiler ); }

    template < class Archive >
    inline void serialize( Archive&, const unsigned int )
    {
        UNREACHABLE;
    }

private:
    ExecutionFPtr m_functor;
};

} // namespace mega::runtime

#endif // GUARD_2023_January_11_jit_base
