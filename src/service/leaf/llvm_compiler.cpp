
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

#include "llvm_compiler.hpp"

#include "service/network/log.hpp"

#include "service/protocol/model/leaf_daemon.hxx"
#include "service/protocol/model/stash.hxx"

#include "service/network/conversation.hpp"

#include "common/file.hpp"
#include "common/stash.hpp"

#include <boost/process.hpp>

#include <sstream>
#include <string>

namespace mega::service
{
namespace
{

void runCompilation( const std::string& strCmd )
{
    SPDLOG_DEBUG( "Compiling: {}", strCmd );

    std::ostringstream osOutput, osError;
    int                iCompilationResult = 0;
    {
        namespace bp = boost::process;

        bp::ipstream errStream, outStream; // reading pipe-stream
        bp::child    c( strCmd, bp::std_out > outStream, bp::std_err > errStream );

        std::string strOutputLine;
        while ( c.running() && std::getline( outStream, strOutputLine ) )
        {
            if ( !strOutputLine.empty() )
            {
                osOutput << "\nOUT    : " << strOutputLine;
            }
        }

        c.wait();
        iCompilationResult = c.exit_code();
        if ( iCompilationResult )
        {
            osError << common::COLOUR_RED_BEGIN << "FAILED : "; // << m_sourcePath.string();
            while ( errStream && std::getline( errStream, strOutputLine ) )
            {
                if ( !strOutputLine.empty() )
                {
                    osError << "\nERROR  : " << strOutputLine;
                }
            }
            osError << common::COLOUR_END;
        }
    }
    if ( iCompilationResult != 0 )
    {
        SPDLOG_ERROR( "Error compilation invocation: {}", osError.str() );
    }
    VERIFY_RTE_MSG( iCompilationResult == 0, "Error compilation invocation: " << osError.str() );
}

void compile( const boost::filesystem::path& clangPath, const boost::filesystem::path& inputCPPFilePath,
              const boost::filesystem::path&                            outputIRFilePath,
              std::optional< const FinalStage::Components::Component* > pComponent,
              const mega::network::MegastructureInstallation&           megastructureInstallation )
{
    auto startTime = std::chrono::steady_clock::now();
    {
        std::ostringstream osCmd;

        osCmd << clangPath << " -S -emit-llvm ";

        if ( pComponent.has_value() )
        {
            // flags
            for ( const std::string& flag : pComponent.value()->get_cpp_flags() )
            {
                VERIFY_RTE( !flag.empty() );
                osCmd << "-" << flag << " ";
            }

            // defines
            for ( const std::string& strDefine : pComponent.value()->get_cpp_defines() )
            {
                VERIFY_RTE( !strDefine.empty() );
                osCmd << "-D" << strDefine << " ";
            }

            // include directories
            for ( const boost::filesystem::path& includeDir : pComponent.value()->get_include_directories() )
            {
                osCmd << "-I " << includeDir.native() << " ";
            }
        }
        else
        {
            // add megastructure include directory
            osCmd << "-I " << megastructureInstallation.getMegaIncludePath() << " ";
        }

        osCmd << "-o " << outputIRFilePath.native() << " -c " << inputCPPFilePath.native();
        runCompilation( osCmd.str() );
    }
    const auto timeDelta = std::chrono::steady_clock::now() - startTime;
    SPDLOG_TRACE( "RUNTIME: Clang Compilation time: {}", timeDelta );
}

void compileToLLVMIRImpl( const LLVMCompilerImpl& compiler, const std::string& strName, const std::string& strCPPCode,
                      std::ostream& osIR, std::optional< const FinalStage::Components::Component* > pComponent )
{
    const boost::filesystem::path irFilePath = compiler.getTempDir() / ( strName + ".ir" );

    const task::DeterminantHash determinant{ strCPPCode };
    if ( compiler.restore( irFilePath.native(), determinant.get() ) )
    {
        boost::filesystem::loadAsciiFile( irFilePath, osIR );
    }
    else
    {
        boost::filesystem::path inputCPPFilePath = compiler.getTempDir() / ( strName + ".cpp" );
        {
            auto pFStream = boost::filesystem::createNewFileStream( inputCPPFilePath );
            *pFStream << strCPPCode;
        }
        compile( compiler.getClangPath(), inputCPPFilePath, irFilePath, pComponent, compiler.getMegaInstall() );
        compiler.stash( irFilePath.native(), determinant.get() );
        boost::filesystem::loadAsciiFile( irFilePath, osIR );
    }
}
} // namespace

void LLVMCompilerImpl::compileToLLVMIR( const std::string& strName, const std::string& strCPPCode, std::ostream& osIR,
                                        std::optional< const FinalStage::Components::Component* > pComponent ) const
{
    compileToLLVMIRImpl( *this, strName, strCPPCode, osIR, pComponent );
}

void LLVMCompilerImpl::stash( const std::string& filePath, mega::U64 determinant ) const
{
    // Conversation
    network::leaf_daemon::Request_Sender router( m_conversation, m_sender, m_yield_ctx );
    network::stash::Request_Encoder      rq(
        [ &router ]( const network::Message& msg ) { return router.LeafRoot( msg ); }, m_conversation.getID() );
    rq.StashStash( filePath, determinant );
}

bool LLVMCompilerImpl::restore( const std::string& filePath, mega::U64 determinant ) const
{
    network::leaf_daemon::Request_Sender router( m_conversation, m_sender, m_yield_ctx );
    network::stash::Request_Encoder      rq(
        [ &router ]( const network::Message& msg ) { return router.LeafRoot( msg ); }, m_conversation.getID() );
    return rq.StashRestore( filePath, determinant );
}

} // namespace mega::service