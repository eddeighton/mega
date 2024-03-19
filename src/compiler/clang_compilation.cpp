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

#include "compiler/clang_compilation.hpp"

#include "common/assert_verify.hpp"

#include "mega/values/compilation/modes.hpp"

#include <sstream>

namespace mega
{
namespace
{
// https://maskray.me/blog/2023-07-16-precompiled-headers
// https://clang.llvm.org/docs/ClangCommandLineReference.html#cmdoption-clang-fpch-instantiate-templates
static const std::string g_PCH_In_Flags = " -Xclang -fno-pch-timestamp ";
static const std::string g_PCH_Out_Flags
    = " -Xclang -fpch-codegen -Xclang -fno-pch-timestamp -Xclang -fpch-instantiate-templates ";

} // namespace

common::Command Compilation::generatePCHVerificationCMD() const
{
    std::ostringstream osCmd;

    // the compiler itself
    osCmd << compiler_command << " -x c++ ";

    // flags
    for( const std::string& flag : flags )
    {
        VERIFY_RTE( !flag.empty() );
        osCmd << "-" << flag << " ";
    }

    // defines
    for( const std::string& strDefine : defines )
    {
        VERIFY_RTE( !strDefine.empty() );
        osCmd << "-D" << strDefine << " ";
    }

    // input pch
    for( const boost::filesystem::path& pchPath : inputPCH )
    {
        osCmd << g_PCH_In_Flags << " -Xclang -include-pch -Xclang " << pchPath.string() << " ";
    }

    VERIFY_RTE( compilationMode.has_value() );

    // eg
    if( compilationMode.value().get() != CompilationMode::eNormal )
    {
        VERIFY_RTE( compiler_plugin.has_value() );
        VERIFY_RTE( srcDir.has_value() );
        VERIFY_RTE( buildDir.has_value() );

        osCmd << "-Xclang -egdll=" << compiler_plugin.value().string() << " ";
        osCmd << "-Xclang -egmode=" << compilationMode.value() << " ";
        osCmd << "-Xclang -egsrddir=" << srcDir.value().string() << " ";
        osCmd << "-Xclang -egbuilddir=" << buildDir.value().string() << " ";

        if( sourceFile.has_value() )
        {
            osCmd << "-Xclang -egsource=" << sourceFile.value().string() << " ";
        }
    }
    else
    {
        VERIFY_RTE( !srcDir.has_value() );
        VERIFY_RTE( !buildDir.has_value() );
        VERIFY_RTE( !sourceFile.has_value() );
    }

    // include directories
    for( const boost::filesystem::path& includeDir : includeDirs )
    {
        osCmd << "-I " << includeDir.string() << " ";
    }

    // ensure no round trip debug cmd line handling in clang
    // osCmd << "-Xclang -no-round-trip-args ";

    // output
    VERIFY_RTE( !outputObject.has_value() );
    VERIFY_RTE( outputPCH.has_value() );
    osCmd << " -verify-pch " << outputPCH.value().string();

    return { osCmd.str() };
}

common::Command Compilation::generateCompilationCMD( Compilation::CompilerCacheOptions cacheOptions ) const
{
    common::Command::EnvironmentMap environmentVars;
    std::ostringstream osCmd;

    switch( cacheOptions )
    {
        case eCache_none:
        {
            osCmd << compiler_command << " ";
        }
        break;
        case eCache_cache:
        {
            osCmd << "ccache " << compiler_command << " ";
        }
        break;
        case eCache_recache:
        {
            osCmd << "ccache " << compiler_command << " ";
            environmentVars.insert( { "CCACHE_RECACHE", "true" } );
        }
        break;
        default:
        {
            THROW_RTE( "Unknown cache option" );
        }
    }

    // input
    osCmd << inputFile.string() << " ";

    // flags
    for( const std::string& flag : flags )
    {
        VERIFY_RTE( !flag.empty() );
        osCmd << "-" << flag << " ";
    }

    // defines
    for( const std::string& strDefine : defines )
    {
        VERIFY_RTE( !strDefine.empty() );
        osCmd << "-D" << strDefine << " ";
    }

    // input pch
    for( const boost::filesystem::path& pchPath : inputPCH )
    {
        osCmd << g_PCH_In_Flags << " -Xclang -include-pch -Xclang " << pchPath.string() << " ";
    }

    VERIFY_RTE( compilationMode.has_value() );

    // eg
    if( compilationMode.value().get() != CompilationMode::eNormal )
    {
        VERIFY_RTE( compiler_plugin.has_value() );
        VERIFY_RTE( srcDir.has_value() );
        VERIFY_RTE( buildDir.has_value() );

        osCmd << "-Xclang -egdll=" << compiler_plugin.value().string() << " ";
        osCmd << "-Xclang -egmode=" << compilationMode.value() << " ";
        osCmd << "-Xclang -egsrddir=" << srcDir.value().string() << " ";
        osCmd << "-Xclang -egbuilddir=" << buildDir.value().string() << " ";

        if( sourceFile.has_value() )
        {
            osCmd << "-Xclang -egsource=" << sourceFile.value().string() << " ";
        }
    }
    else
    {
        VERIFY_RTE( !srcDir.has_value() );
        VERIFY_RTE( !buildDir.has_value() );
        VERIFY_RTE( !sourceFile.has_value() );
    }

    // include directories
    for( const boost::filesystem::path& includeDir : includeDirs )
    {
        osCmd << "-I " << includeDir.string() << " ";
    }

    // ensure no round trip debug cmd line handling in clang
    osCmd << "-Xclang -no-round-trip-args ";

    // output
    if( outputPPH.has_value() )
    {
        VERIFY_RTE( !outputObject.has_value() );
        VERIFY_RTE( !outputPCH.has_value() );
        osCmd << " -E -o " << outputPPH.value().string() << " ";
    }
    else if( outputPCH.has_value() )
    {
        VERIFY_RTE( !outputPPH.has_value() );
        VERIFY_RTE( !outputObject.has_value() );
        osCmd << g_PCH_Out_Flags << " -Xclang -emit-pch -o " << outputPCH.value().string() << " ";
    }
    else if( outputObject.has_value() )
    {
        VERIFY_RTE( !outputPPH.has_value() );
        VERIFY_RTE( !outputPCH.has_value() );
        osCmd << " -c -o " << outputObject.value().string() << " ";
    }
    else
    {
        osCmd << " -Xclang -fsyntax-only ";
    }

    return { osCmd.str(), environmentVars };
}

} // namespace mega
