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

#include "service/protocol/common/environment.hpp"

#include <cstdlib>
#include <string>
#include <sstream>

namespace mega::network
{

short MegaDaemonPort()
{
    if ( const char* pEnvValue = std::getenv( mega::environment::CFG_DAEMON_PORT ); pEnvValue )
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
    if ( const char* pEnvValue = std::getenv( mega::environment::CFG_ROOT_PORT ); pEnvValue )
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

} // namespace mega::network
