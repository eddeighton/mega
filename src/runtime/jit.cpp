
#include "jit.hpp"

#include "service/network/log.hpp"

namespace mega
{
namespace runtime
{
llvm::ExitOnError ExitOnErr;

JITCompiler::StaticInit::StaticInit()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
}

JITCompiler::JITCompiler()
    : m_pLLJit( ExitOnErr( llvm::orc::LLJITBuilder().create() ) )
{
    m_pLLJit->getMainJITDylib().addGenerator(
        ExitOnErr( llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
            m_pLLJit->getDataLayout().getGlobalPrefix() ) ) );
            
}

void* testFunc( const mega::reference& )
{
    static int x = 234;
    return &x;
}

mega::runtime::ReadFunction JITCompiler::compile_read( const DatabaseInstance& database,
    const mega::InvocationID& invocation )
{
    const FinalStage::Operations::Invocation* pInvocation = database.getInvocation( invocation );

    using namespace FinalStage::Invocations;
    //Variables::Context* pRootVariable = pInvocation->get_root_variable();

    //pInvocation->get_root_instruction()

    mega::runtime::ReadFunction pResultFunction = testFunc;

    return pResultFunction;
}

void JITCompiler::unload_read( mega::runtime::ReadFunction pFunction )
{
}

} // namespace runtime
} // namespace mega
