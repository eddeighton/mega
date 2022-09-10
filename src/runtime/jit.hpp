
#ifndef JIT_8_AUG_2022
#define JIT_8_AUG_2022

#include "runtime/runtime_functions.hpp"

#include <memory>
#include <set>

namespace mega
{
namespace runtime
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

        virtual mega::runtime::GetHeapFunction      getGetHeap( const std::string& strSymbol )      = 0;
        virtual mega::runtime::GetSharedFunction    getGetShared( const std::string& strSymbol )    = 0;
        virtual mega::runtime::AllocationFunction   getAllocation( const std::string& strSymbol )   = 0;
        virtual mega::runtime::DeAllocationFunction getDeAllocation( const std::string& strSymbol ) = 0;
        virtual mega::runtime::AllocateFunction     getAllocate( const std::string& strSymbol )     = 0;
        virtual mega::runtime::ReadFunction         getRead( const std::string& strSymbol )         = 0;
        virtual mega::runtime::WriteFunction        getWrite( const std::string& strSymbol )        = 0;
        virtual mega::runtime::CallFunction         getCall( const std::string& strSymbol )         = 0;
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
} // namespace runtime
} // namespace mega

#endif // JIT_8_AUG_2022
