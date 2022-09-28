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


#include "database/types/clang_compilation.hpp"

#include "common/assert_verify.hpp"
#include "database/types/modes.hpp"

#include <sstream>

namespace mega
{

std::string Compilation::generatePCHVerificationCMD() const
{
    std::ostringstream osCmd;

    // the compiler itself
    osCmd << compiler.native() << " -x c++ ";

    // flags
    for ( const std::string& flag : flags )
    {
        VERIFY_RTE( !flag.empty() );
        osCmd << "-" << flag << " ";
    }

    // defines
    for ( const std::string& strDefine : defines )
    {
        VERIFY_RTE( !strDefine.empty() );
        osCmd << "-D" << strDefine << " ";
    }

    // input pch
    for ( const boost::filesystem::path& inputPCH : inputPCH )
    {
        osCmd << "-Xclang -fno-pch-timestamp -Xclang -include-pch ";
        osCmd << "-Xclang " << inputPCH.native() << " ";
    }

    // eg
    if ( compilationMode.has_value() )
    {
        VERIFY_RTE( compiler_plugin.has_value() );
        VERIFY_RTE( srcDir.has_value() );
        VERIFY_RTE( buildDir.has_value() );

        osCmd << "-Xclang -egdll=" << compiler_plugin.value().native() << " ";
        osCmd << "-Xclang -egmode=" << compilationMode.value() << " ";
        osCmd << "-Xclang -egsrddir=" << srcDir.value().native() << " ";
        osCmd << "-Xclang -egbuilddir=" << buildDir.value().native() << " ";

        if ( sourceFile.has_value() )
        {
            osCmd << "-Xclang -egsource=" << sourceFile.value().native() << " ";
        }
    }
    else
    {
        VERIFY_RTE( !srcDir.has_value() );
        VERIFY_RTE( !buildDir.has_value() );
        VERIFY_RTE( !sourceFile.has_value() );
    }

    // include directories
    for ( const boost::filesystem::path& includeDir : includeDirs )
    {
        osCmd << "-I " << includeDir.native() << " ";
    }

    // ensure no round trip debug cmd line handling in clang
    // osCmd << "-Xclang -no-round-trip-args ";

    // output
    VERIFY_RTE( !outputObject.has_value() );
    VERIFY_RTE( outputPCH.has_value() );
    osCmd << " -verify-pch " << outputPCH.value().native();

    return osCmd.str();
}

std::string Compilation::generateCompilationCMD() const
{
    std::ostringstream osCmd;

    // the compiler itself
    osCmd << compiler.native() << " ";

    // flags
    for ( const std::string& flag : flags )
    {
        VERIFY_RTE( !flag.empty() );
        osCmd << "-" << flag << " ";
    }

    // defines
    for ( const std::string& strDefine : defines )
    {
        VERIFY_RTE( !strDefine.empty() );
        osCmd << "-D" << strDefine << " ";
    }

    // input pch
    for ( const boost::filesystem::path& inputPCH : inputPCH )
    {
        osCmd << "-Xclang -fno-pch-timestamp -Xclang -include-pch ";
        osCmd << "-Xclang " << inputPCH.native() << " ";
    }

    // eg
    if ( compilationMode.has_value() )
    {
        VERIFY_RTE( compiler_plugin.has_value() );
        VERIFY_RTE( srcDir.has_value() );
        VERIFY_RTE( buildDir.has_value() );

        osCmd << "-Xclang -egdll=" << compiler_plugin.value().native() << " ";
        osCmd << "-Xclang -egmode=" << compilationMode.value() << " ";
        osCmd << "-Xclang -egsrddir=" << srcDir.value().native() << " ";
        osCmd << "-Xclang -egbuilddir=" << buildDir.value().native() << " ";

        if ( sourceFile.has_value() )
        {
            osCmd << "-Xclang -egsource=" << sourceFile.value().native() << " ";
        }
    }
    else
    {
        VERIFY_RTE( !srcDir.has_value() );
        VERIFY_RTE( !buildDir.has_value() );
        VERIFY_RTE( !sourceFile.has_value() );
    }

    // include directories
    for ( const boost::filesystem::path& includeDir : includeDirs )
    {
        osCmd << "-I " << includeDir.native() << " ";
    }

    // ensure no round trip debug cmd line handling in clang
    osCmd << "-Xclang -no-round-trip-args ";

    // input
    osCmd << inputFile.native() << " ";

    // output
    if ( outputPCH.has_value() )
    {
        VERIFY_RTE( !outputObject.has_value() );
        osCmd << " -Xclang -fno-pch-timestamp -Xclang -emit-pch -o " << outputPCH.value().native() << " ";
    }
    else if ( outputObject.has_value() )
    {
        VERIFY_RTE( !outputPCH.has_value() );
        osCmd << " -c -o " << outputObject.value().native() << " ";
    }
    else
    {
        THROW_RTE( "Missing compiler output" );
    }

    return osCmd.str();
}

} // namespace mega
