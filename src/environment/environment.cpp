
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

#include "environment/environment.hpp"

namespace mega::service
{
namespace
{
static constexpr char CCACHE_DIR[]          = "CCACHE_DIR";
static constexpr char CCACHE_CONFIGPATH[]   = "CCACHE_CONFIGPATH";
static constexpr char BUILD_PATH[]          = "BUILD_PATH";          // /build
static constexpr char WORKSPACE_ROOT_PATH[] = "WORKSPACE_ROOT_PATH"; // /workspace/root
static constexpr char CFG_TUPLE[]           = "CFG_TUPLE";
static constexpr char CFG_LINK[]            = "CFG_LINK";
static constexpr char CFG_TYPE[]            = "CFG_TYPE";
static constexpr char CFG_CMAKE[]           = "CFG_CMAKE";
static constexpr char UNITY_EDITOR[]        = "UNITY_EDITOR";
static constexpr char CFG_CXXFLAGS[]        = "CFG_CXXFLAGS";
static constexpr char CFG_CFLAGS[]          = "CFG_CFLAGS";
static constexpr char CFG_BUILD_TOOL[]      = "CFG_BUILD_TOOL";
static constexpr char CFG_BUILD_CMD[]       = "CFG_BUILD_CMD";
} // namespace

const char* Environment::getenv( const char* pszKey )
{
    if( const char* pEnvValue = std::getenv( pszKey ); pEnvValue )
    {
        return pEnvValue;
    }
    else
    {
        THROW_RTE( "Failed to find environment key: " << pszKey );
    }
}

const char* Environment::getenv( const char* pszKey, const char* pszDefault )
{
    if( const char* pEnvValue = std::getenv( pszKey ); pEnvValue )
    {
        return pEnvValue;
    }
    else
    {
        return pszDefault;
    }
}

const char* Environment::user()
{
    return getenv( "USER" );
}

const char* Environment::workPrefix()
{
    return "test_";
}

const char* Environment::cfgType()
{
    return getenv( CFG_TYPE );
}

const char* Environment::cfgTuple()
{
    return getenv( CFG_TUPLE );
}

const char* Environment::buildCmd()
{
    return getenv( CFG_BUILD_CMD );
}

const char* Environment::workspace()
{
    return getenv( WORKSPACE_ROOT_PATH );
}

const char* Environment::build()
{
    return getenv( BUILD_PATH );
}

Environment::Path Environment::work()
{
    std::ostringstream os;
    os << "/home/" << user() << "/" << workPrefix() << cfgType();
    return os.str();
}

Environment::Path Environment::workTmp()
{
    return work() / "tmp";
}

Environment::Path Environment::workBin()
{
    return work() / "bin";
}

Environment::Path Environment::srcDir( const Project& project )
{
    std::ostringstream os;
    os << workspace() << "/src/" << project.str() << "/src";
    return os.str();
}

Environment::Path Environment::buildDir( const Project& project )
{
    std::ostringstream os;
    os << build() << '/' << cfgTuple() << '/' << project.str() << "/build";
    return os.str();
}

Environment::Path Environment::prog( const Program& program )
{
    std::ostringstream os;
    os << program;
    return workBin() / os.str();
}

ProgramManifest Environment::load( const Program& program )
{
    const Path      programPath = prog( program );
    ProgramManifest result;
    if( boost::filesystem::exists( programPath ) )
    {
        auto                         pFileStream = boost::filesystem::createBinaryInputFileStream( programPath );
        boost::archive::xml_iarchive xml( *pFileStream );
        xml&                         boost::serialization::make_nvp( "program_manifest", result );
    }
    return result;
}

Environment::Path Environment::database( const Program& program )
{
    auto p = load( program );
    return p.getDatabase();
}
} // namespace mega::service