
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

#ifndef GUARD_2023_March_21_api
#define GUARD_2023_March_21_api

#ifdef _WIN64
using MEGA_64 = unsigned long long;
#else
using MEGA_64 = unsigned long int;
#endif

#if defined( _WIN32 )
#    ifdef MEGA_PLUGIN_SHARED_MODULE
#        define MEGA_PLUGIN_EXPORT __declspec( dllexport )
#    else
#        define MEGA_PLUGIN_EXPORT __declspec( dllimport )
#    endif
#elif defined( __GNUC__ )
#    ifdef MEGA_PLUGIN_SHARED_MODULE
#        define MEGA_PLUGIN_EXPORT __attribute__( ( visibility( "default" ) ) )
#    else
#        define MEGA_PLUGIN_EXPORT
#    endif
#endif

extern "C"
{
    // lifetime
    MEGA_PLUGIN_EXPORT void mp_initialise( const char* pszConsoleLogLevel, const char* pszFileLogLevel );
    MEGA_PLUGIN_EXPORT void mp_shutdown();

    MEGA_PLUGIN_EXPORT MEGA_64 mp_database_hashcode();
    MEGA_PLUGIN_EXPORT const char* mp_database();

    MEGA_PLUGIN_EXPORT const void* mp_downstream();
    MEGA_PLUGIN_EXPORT void mp_upstream( float delta, void* pRange );
}

#endif //GUARD_2023_March_21_api
