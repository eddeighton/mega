
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

#include "service/stash_provider.hpp"

#include "common/unreachable.hpp"

#include <functional>

namespace mega
{

class MPOContext;

namespace runtime
{
class FunctorID;

class FunctionProvider
{
public:
    virtual void getFunction( service::StashProvider& stashProvider, const FunctorID& functionID, void** ppFunction )
        = 0;

    /*
        struct InvocationTypeInfo
        {
            mega::ExplicitOperationID operationType;
            std::string               mangledType = std::string{};
        };
        virtual InvocationTypeInfo compileInvocationFunction( void* pLLVMCompiler, const char* pszUnitName,
                                                              const mega::InvocationID& invocationID, void** ppFunction
       ) = 0;

        virtual void getProgramFunction( void* pLLVMCompiler, int functionType, void** ppFunction ) = 0;
        virtual void getInvocationFunction( void* pLLVMCompiler, const char* pszUnitName,
                                            const mega::InvocationID& invocationID, int functionType, void** ppFunction
       ) = 0; virtual void getObjectFunction( void* pLLVMCompiler, const char* pszUnitName, mega::TypeID typeID, int
       functionType, void** ppFunction ) = 0; virtual void getRelationFunction( void* pLLVMCompiler, const char*
       pszUnitName, const RelationID& relationID, int functionType, void** ppFunction ) = 0;

        virtual void getActionFunction( mega::TypeID concreteTypeID, void** ppFunction )                        = 0;
        virtual void getDecisionFunction( void* pLLVMCompiler, mega::TypeID concreteTypeID, void** ppFunction ) = 0;
        virtual void getPythonFunction( mega::TypeID interfaceTypeID, void** ppFunction, void* pPythonCaster )  = 0;

        virtual void getOperatorFunction( void* pLLVMCompiler, const char* pszUnitName, TypeID target, int functionType,
                                          void** ppFunction )
            = 0;*/
};

class RuntimeFunctor
{
public:
    using ExecutionFPtr = std::function< void( FunctionProvider&, service::StashProvider& stashProvider ) >;

    RuntimeFunctor() { UNREACHABLE; }

    RuntimeFunctor( ExecutionFPtr&& functor )
        : m_functor( std::move( functor ) )
    {
    }

    inline void operator()( FunctionProvider& jit, service::StashProvider& stashProvider ) const
    {
        m_functor( jit, stashProvider );
    }

    template < class Archive >
    inline void serialize( Archive&, const unsigned int )
    {
        UNREACHABLE;
    }

private:
    ExecutionFPtr m_functor;
};

class Functor
{
public:
    using ExecutionFPtr = std::function< void( MPOContext& ) >;

    Functor() { UNREACHABLE; }

    Functor( ExecutionFPtr&& functor )
        : m_functor( std::move( functor ) )
    {
    }

    inline void operator()( MPOContext& context ) const { m_functor( context ); }

    template < class Archive >
    inline void serialize( Archive&, const unsigned int )
    {
        UNREACHABLE;
    }

private:
    ExecutionFPtr m_functor;
};

} // namespace runtime
} // namespace mega

#endif // GUARD_2023_January_11_jit_base
