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

#include "mega/reference.hpp"
#include "traits.hpp"

#include <algorithm>

namespace mega::mangle
{
namespace
{
template < typename T >
inline T& reify( void* p )
{
    return *reinterpret_cast< T* >( p );
}
template < typename T >
inline const T& reify( const void* p )
{
    return *reinterpret_cast< const T* >( p );
}

inline bool contains( const ReferenceVector& refVector, const reference& ref )
{
    return std::find( refVector.cbegin(), refVector.cend(), ref ) != refVector.end();
}
bool hyper_is_link( const reference& srcContext, const reference& srcValue, const reference& targetContext,
                    const reference& targetValue )
{
    return ( srcContext == targetValue ) && ( targetContext == srcValue );
}

bool hyper_is_link( const reference& srcContext, const ReferenceVector& srcValue, const reference& targetContext,
                    const reference& targetValue )
{
    return ( srcContext == targetValue ) && contains( srcValue, targetContext );
}
bool hyper_is_link( const reference& srcContext, const reference& srcValue, const reference& targetContext,
                    const ReferenceVector& targetValue )
{
    return contains( targetValue, srcContext ) && ( targetContext == srcValue );
}
bool hyper_is_link( const reference& srcContext, const ReferenceVector& srcValue, const reference& targetContext,
                    const ReferenceVector& targetValue )
{
    return contains( targetValue, srcContext ) && contains( srcValue, targetContext );
}

} // namespace

bool hyper_is_link_r_r( const void* srcContext, const void* srcValue, const void* targetContext,
                        const void* targetValue )
{
    return hyper_is_link( reify< reference >( srcContext ),
                          reify< reference >( srcValue ),
                          reify< reference >( targetContext ),
                          reify< reference >( targetValue ) );
}

bool hyper_is_link_r_v( const void* srcContext, const void* srcValue, const void* targetContext,
                        const void* targetValue )
{
    return hyper_is_link( reify< reference >( srcContext ),
                          reify< ReferenceVector >( srcValue ),
                          reify< reference >( targetContext ),
                          reify< reference >( targetValue ) );
}
bool hyper_is_link_v_r( const void* srcContext, const void* srcValue, const void* targetContext,
                        const void* targetValue )
{
    return hyper_is_link( reify< reference >( srcContext ),
                          reify< reference >( srcValue ),
                          reify< reference >( targetContext ),
                          reify< ReferenceVector >( targetValue ) );
}
bool hyper_is_link_v_v( const void* srcContext, const void* srcValue, const void* targetContext,
                        const void* targetValue )
{
    return hyper_is_link( reify< reference >( srcContext ),
                          reify< ReferenceVector >( srcValue ),
                          reify< reference >( targetContext ),
                          reify< ReferenceVector >( targetValue ) );
}

/*
void schedule_start( const reference& ref )
{
    mega::log::Storage& log = mega::getMPOContext()->getLog();
    log.record( log::SchedulerRecord( ref, log::SchedulerRecord::Start ) );
}

void schedule_stop( const reference& ref )
{
    mega::log::Storage& log = mega::getMPOContext()->getLog();
    log.record( log::SchedulerRecord( ref, log::SchedulerRecord::Stop ) );
}
*/
} // namespace mega
