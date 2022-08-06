

#include <gtest/gtest.h>

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

#include <iostream>

class OrcTest : public ::testing::Test
{
public:
    void SetUp()
    {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
    }

    void TearDown() {}
};

llvm::ExitOnError ExitOnErr;

inline llvm::Error createSMDiagnosticError( llvm::SMDiagnostic& Diag )
{
    using namespace llvm;
    std::string Msg;
    {
        raw_string_ostream OS( Msg );
        Diag.print( "", OS );
    }
    return make_error< StringError >( std::move( Msg ), inconvertibleErrorCode() );
}

inline llvm::Expected< llvm::orc::ThreadSafeModule > parseExampleModule( llvm::StringRef Source, llvm::StringRef Name )
{
    using namespace llvm;
    auto         Ctx = std::make_unique< LLVMContext >();
    SMDiagnostic Err;
    if ( auto M = parseIR( MemoryBufferRef( Source, Name ), Err, *Ctx ) )
        return orc::ThreadSafeModule( std::move( M ), std::move( Ctx ) );

    return createSMDiagnosticError( Err );
}

extern "C"
{
    //__attribute__( ( visibility( "default" ) ) ) 
    int32_t add2( int32_t arg ) { return arg + 2; }
}

TEST_F( OrcTest, Basic )
{
    const llvm::StringRef Add3Example =
        R"(
  declare i32 @add2(i32 %x)
  define i32 @add3(i32 %x) {
  entry:
    %t = call i32 @add2(i32 %x)
    %r = add nsw i32 %t, 1
    ret i32 %r
  }
)";

    const llvm::StringRef Add1Example =
        R"(
    
  define i32 @add1(i32 %x) {
  entry:
    %r = add nsw i32 %x, 1
    ret i32 %r
  }
)";

    ASSERT_EQ( add2( 1 ), 3 );

    // llvm::LLVMContext llvmContext;

    std::unique_ptr< llvm::orc::LLJIT > pJit = ExitOnErr( llvm::orc::LLJITBuilder().create() );
    ASSERT_TRUE( pJit );

    // llvm::orc::JITDylib& testDynamicLib = ExitOnErr( pJit->createJITDylib( "testDynamicLib" ) );
    // ExitOnErr( pJit->getExecutionSession().removeJITDylib( testDynamicLib ) );

    auto DLSG = ExitOnErr( llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess( pJit->getDataLayout().getGlobalPrefix() ) );
    pJit->getMainJITDylib().addGenerator( std::move( DLSG ) );

    {
        auto module = ExitOnErr( parseExampleModule( Add1Example, "Add1Module" ) );
        ExitOnErr( pJit->addIRModule( std::move( module ) ) );
        auto add1Address          = ExitOnErr( pJit->lookup( "add1" ) );
        int ( *pFuncAdd1 )( int ) = add1Address.toPtr< int( int ) >();
        ASSERT_EQ( pFuncAdd1( 123 ), 124 );
    }
    {
        auto module2 = ExitOnErr( parseExampleModule( Add3Example, "Add3Module" ) );
        ExitOnErr( pJit->addIRModule( std::move( module2 ) ) );

        auto add3Address          = ExitOnErr( pJit->lookup( "add3" ) );
        int ( *pFuncAdd3 )( int ) = add3Address.toPtr< int( int ) >();
        ASSERT_EQ( pFuncAdd3( 123 ), 126 );
    }
}