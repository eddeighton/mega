
#include "database/types/clang_compilation.hpp"

#include "common/assert_verify.hpp"
#include "database/types/modes.hpp"

#include <sstream>

namespace mega
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
std::string Compilation::operator()() const
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
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << inputPCH.native() << " ";
    }

    // eg
    if( compilationMode.has_value() )
    {
        osCmd << "-Xclang -egdll=" <<       compiler_plugin.value().native() << " ";
        osCmd << "-Xclang -egmode=" <<      compilationMode.value() << " ";
        osCmd << "-Xclang -egsrddir=" <<    srcDir.value().native() << " ";
        osCmd << "-Xclang -egbuilddir=" <<  buildDir.value().native() << " ";
        osCmd << "-Xclang -egsource=" <<    sourceFile.value().native() << " ";
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
        osCmd << "-Xclang -fno-pch-timestamp -Xclang -emit-pch -o " << outputPCH.value().native() << " ";
    }
    else if ( outputObject.has_value() )
    {
        VERIFY_RTE( !outputPCH.has_value() );
        osCmd << "-c -o " << outputObject.value().native() << " ";
    }
    else
    {
        THROW_RTE( "Missing compiler output" );
    }

    return osCmd.str();
}

} // namespace mega
