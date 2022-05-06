
#include "utilities/clang_compilation.hpp"

#include "common/assert_verify.hpp"

#include <sstream>

namespace mega
{
namespace utilities
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
    if ( egdb.has_value() )
    {
        osCmd << "-Xclang -egdb=" << egdb.value().native() << " ";
        VERIFY_RTE( compiler_plugin.has_value() );
        osCmd << "-Xclang -egdll=" << compiler_plugin.value().native() << " ";

        if ( egtu.has_value() )
        {
            VERIFY_RTE( egtuid.has_value() );
            osCmd << "-Xclang -egtu=" << egtu.value().native() << " ";
            osCmd << "-Xclang -egtuid=" << egtuid.value() << " ";
        }
        else
        {
            VERIFY_RTE( !egtuid.has_value() );
        }
    }
    else
    {
        VERIFY_RTE( !egtu.has_value() );
        VERIFY_RTE( !egtuid.has_value() );
    }

    // include directories
    for ( const boost::filesystem::path& includeDir : includeDirs )
    {
        osCmd << "-I " << includeDir.native() << " ";
    }

    // input
    osCmd << inputFile.native() << " ";

    // output
    if ( outputPCH.has_value() )
    {
        VERIFY_RTE( !outputObject.has_value() );
        osCmd << "-Xclang -emit-pch -o " << outputPCH.value().native() << " ";
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

} // namespace utilities
} // namespace mega
