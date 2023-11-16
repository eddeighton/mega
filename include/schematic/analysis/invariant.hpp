
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

#ifndef GUARD_2023_June_10_invariant
#define GUARD_2023_June_10_invariant

#include <stdexcept>
#include <sstream>
#include <exception>

#include <boost/current_function.hpp>

#include "common/requireSemicolon.hpp"
#include "common/processID.hpp"

#ifdef DEBUG

#ifndef _CRT_ASSERT
#define _CRT_ASSERT 0
#endif
#ifndef _CRT_ERROR
#define _CRT_ERROR 0
#endif

#define INVARIANT_THROW( exceptionType, msg )                  \
    DO_STUFF_AND_REQUIRE_SEMI_COLON(                 \
        std::ostringstream _os2; \
        _os2 << "Analysis Invariant failed. FILE " << __FILE__ << ":" << __LINE__ << "\nMSG:" << msg; \
        throw exceptionType( _os2.str() ); )

#else // DEBUG

#define INVARIANT_THROW( exceptionType, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::ostringstream _os2; _os2 << msg; throw exceptionType( _os2.str() ); )

#endif //_DEBUG

#define INVARIANT_IMPL( expression, exceptionType, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( if( !( expression ) ) { INVARIANT_THROW( exceptionType, msg ); } )

#define INVARIANT( expression, msg ) INVARIANT_IMPL( expression, std::runtime_error, msg )

#endif // GUARD_2023_June_10_invariant
