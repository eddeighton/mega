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

#ifndef MEGA_ENVIRONMENT_9_SEPT
#define MEGA_ENVIRONMENT_9_SEPT

namespace mega::environment
{

// environment variables setup by the environment configuration scripts
static const char* CCACHE_DIR          = "CCACHE_DIR";
static const char* BUILD_PATH          = "BUILD_PATH";
static const char* WORKSPACE_ROOT_PATH = "WORKSPACE_ROOT_PATH";
static const char* CFG_TUPLE           = "CFG_TUPLE";
static const char* CFG_LINK            = "CFG_LINK";
static const char* CFG_TYPE            = "CFG_TYPE";
static const char* CFG_CMAKE           = "CFG_CMAKE";
static const char* CFG_CXXFLAGS        = "CFG_CXXFLAGS";
static const char* CFG_CFLAGS          = "CFG_CFLAGS";
static const char* CFG_BUILD_TOOL      = "CFG_BUILD_TOOL";
static const char* CFG_BUILD_CMD       = "CFG_BUILD_CMD";
static const char* CFG_BOOST_TOOLSET   = "CFG_BOOST_TOOLSET";
static const char* CFG_BOOST_B2        = "CFG_BOOST_B2";
static const char* CFG_BOOST_B2_EXE    = "CFG_BOOST_B2_EXE";

} // namespace mega::environment

#endif // MEGA_ENVIRONMENT_9_SEPT
