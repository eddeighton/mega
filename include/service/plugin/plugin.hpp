
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

#ifndef GUARD_2023_March_07_plugin
#define GUARD_2023_March_07_plugin

#ifdef _WIN64
using MEGA_U64 = unsigned long long;
#else
using MEGA_U64 = unsigned long int;
#endif

extern "C"
{
    // lifetime
    void mp_initialise( const char* pszConsoleLogLevel, const char* pszFileLogLevel );
    void mp_update();
    void mp_shutdown();

    // network connection
    MEGA_U64 mp_network_count();
    const char* mp_network_name( MEGA_U64 networkID );
    void mp_network_connect( MEGA_U64 networkID );
    void mp_network_disconnect();

    // planet
    void mp_planet_create();
    void mp_planet_destroy();
}

#endif //GUARD_2023_March_07_plugin
