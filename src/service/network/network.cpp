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

#include "service/network/network.hpp"

#include "common/assert_verify.hpp"

#include <cstdlib>
#include <string>
#include <sstream>

namespace mega::network
{

short MegaDaemonPort()
{
    static const char* CFG_DAEMON_PORT = "CFG_DAEMON_PORT";
    if( const char* pEnvValue = std::getenv( CFG_DAEMON_PORT ); pEnvValue )
    {
        std::string        str( pEnvValue );
        std::istringstream is( str );
        short              port;
        is >> port;
        return port;
    }
    else
    {
        return 4237;
    }
}

short MegaRootPort()
{
    static const char* CFG_ROOT_PORT = "CFG_ROOT_PORT";
    if( const char* pEnvValue = std::getenv( CFG_ROOT_PORT ); pEnvValue )
    {
        std::string        str( pEnvValue );
        std::istringstream is( str );
        short              port;
        is >> port;
        return port;
    }
    else
    {
        return 4238;
    }
}

std::optional< std::string > MegaWorkspaceRootPath()
{
    static const char* CFG = "WORKSPACE_ROOT_PATH";

    if( const char* pEnvValue = std::getenv( CFG ); pEnvValue )
    {
        return std::string{ pEnvValue };
    }
    else
    {
        return {};
    }
}

std::optional< std::string > MegaBuildPath()
{
    static const char* CFG = "BUILD_PATH";
    if( const char* pEnvValue = std::getenv( CFG ); pEnvValue )
    {
        return std::string{ pEnvValue };
    }
    else
    {
        return {};
    }
}

std::optional< std::string > MegaType()
{
    static const char* CFG = "CFG_TYPE";
    if( const char* pEnvValue = std::getenv( CFG ); pEnvValue )
    {
        return std::string{ pEnvValue };
    }
    else
    {
        return {};
    }
}

std::optional< std::string > MegaTuple()
{
    static const char* CFG = "CFG_TUPLE";
    if( const char* pEnvValue = std::getenv( CFG ); pEnvValue )
    {
        return std::string{ pEnvValue };
    }
    else
    {
        return {};
    }
}

} // namespace mega::network
