
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

#include "runtime/clang.hpp"

#include "database/FinalStage.hxx"

#include "log/log.hpp"

#include "common/process.hpp"

#include <sstream>
#include <string>

namespace mega::runtime
{
namespace
{

void runCompilation( const std::string& strCmd )
{
    SPDLOG_DEBUG( "Compiling: {}", strCmd );

    std::string strOutput, strError;
    const int   iExitCode = common::runProcess( strCmd, strOutput, strError );

    if( iExitCode != 0 )
    {
        std::istringstream isErr( strError );

        std::ostringstream osError;
        osError << common::COLOUR_RED_BEGIN << "FAILED : ";
        std::string str;
        while( isErr && std::getline( isErr, str ) )
        {
            if( !str.empty() )
            {
                osError << "\nERROR  : " << str;
            }
        }
        osError << common::COLOUR_END;
        SPDLOG_ERROR( "Error compilation invocation: {}", osError.str() );
        THROW_RTE( "Error compilation invocation: " << osError.str() );
    }
}

void compile( const boost::filesystem::path& inputCPPFilePath,
              const boost::filesystem::path& outputIRFilePath,
              std::optional< const FinalStage::Components::Component* >
                                                     pComponent,
              const mega::MegastructureInstallation& megastructureInstallation )
{
    auto startTime = std::chrono::steady_clock::now();
    {
        std::ostringstream osCmd;

        using namespace std::string_literals;
        static const std::string hackFlagsForNow = " -std=c++20 -DMEGAJIT -fexceptions"s;

        osCmd << megastructureInstallation.getClangPath() << hackFlagsForNow << " -S -emit-llvm ";

        if( pComponent.has_value() )
        {
            // flags
            for( const std::string& flag : pComponent.value()->get_cpp_flags() )
            {
                VERIFY_RTE( !flag.empty() );
                osCmd << "-" << flag << " ";
            }

            // defines
            for( const std::string& strDefine : pComponent.value()->get_cpp_defines() )
            {
                VERIFY_RTE( !strDefine.empty() );
                osCmd << "-D" << strDefine << " ";
            }

            // include directories
            for( const boost::filesystem::path& includeDir : pComponent.value()->get_include_directories() )
            {
                osCmd << "-I " << includeDir.string() << " ";
            }
        }
        else
        {
            // add megastructure include directory
            osCmd << "-I " << megastructureInstallation.getMegaIncludePath() << " ";
        }

        osCmd << "-o " << outputIRFilePath.string() << " -c " << inputCPPFilePath.string();
        runCompilation( osCmd.str() );
    }
    const auto timeDelta = std::chrono::steady_clock::now() - startTime;
    SPDLOG_TRACE( "RUNTIME: Clang Compilation time: {}", timeDelta );
}

} // namespace

Clang::Clang( const MegastructureInstallation& megaInstall )
    : m_megaInstall( megaInstall )
{
}

void Clang::compileToLLVMIR( const boost::filesystem::path& cppFile, const boost::filesystem::path& irFile,
                             std::optional< const FinalStage::Components::Component* > pComponent ) const
{
    compile( cppFile, irFile, pComponent, m_megaInstall );
}

} // namespace mega::runtime
