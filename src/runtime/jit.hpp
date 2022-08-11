
#ifndef JIT_8_AUG_2022
#define JIT_8_AUG_2022

#include "runtime/runtime.hpp"

#include "database.hpp"

#include "database/model/FinalStage.hxx"

#include <memory>

#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DataLayout.h"

#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"

#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"

#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "llvm/IRReader/IRReader.h"

#include "llvm/Target/TargetMachine.h"

#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/SourceMgr.h"

namespace mega
{
    namespace runtime
    {
        class JITCompiler
        {
            struct StaticInit
            {
                StaticInit();
            };
        public:
            JITCompiler();

            mega::runtime::ReadFunction compile_read( const DatabaseInstance& database,
                const mega::invocation::ID& invocation );

            void unload_read( mega::runtime::ReadFunction pFunction );

        private:
            StaticInit m_staticInit;
            std::unique_ptr< llvm::orc::LLJIT > m_pLLJit;

            using InvocationMap = std::map< void*, std::shared_ptr< llvm::orc::JITDylib > >;
            InvocationMap m_invocations;
        };
    }
}

#endif //JIT_8_AUG_2022
