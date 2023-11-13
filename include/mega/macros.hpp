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

#ifndef EG_MACROS_2_NOV_2022
#define EG_MACROS_2_NOV_2022

#include "mega/values/native_types_io.hpp"
#include "mega/values/runtime/maths_types_io.hpp"
#include "mega/values/runtime/reference_io.hpp"

#include "service/protocol/common/context.hpp"
#include "log/file_log.hpp"

#include <sstream>

#define EG_DO_STUFF_AND_REQUIRE_SEMI_COLON( stuff ) \
    do                                              \
    {                                               \
        stuff                                       \
    } while ( ( void )0, 0 )

#define LOG( __level, __msg )                                                               \
    EG_DO_STUFF_AND_REQUIRE_SEMI_COLON(                                                     \
        std::ostringstream _os_msg; _os_msg << __FILE__ << ":" << __LINE__ << " " << __msg; \
        mega::Context::get()->getLog().record( mega::log::Log::Write( mega::log::Log::e##__level, _os_msg.str() ) ); )

#endif // EG_MACROS_2_NOV_2022
