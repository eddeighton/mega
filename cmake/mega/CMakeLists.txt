##  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
##  Author: Edward Deighton
##  License: Please see license.txt in the project root folder.

##  Use and copying of this software and preparation of derivative works
##  based upon this software are permitted. Any copy of this software or
##  of any derivative work must include the above copyright notice, this
##  paragraph and the one after it.  Any distribution of this software or
##  derivative works must comply with all applicable laws.

##  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
##  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
##  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
##  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
##  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
##  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
##  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
##  OF THE POSSIBILITY OF SUCH DAMAGES.





cmake_minimum_required( VERSION 3.1...3.16 )

set( MEGA_LIBRARY
    ${MEGA_API_DIR}/mega/jit_interface.hpp
    ${MEGA_API_DIR}/mega/component_interface.hpp

    ${MEGA_API_DIR}/mega/address_table.hpp
    ${MEGA_API_DIR}/mega/allocator.hpp
    ${MEGA_API_DIR}/mega/backtrace.hpp
    ${MEGA_API_DIR}/mega/bin_archive.hpp
    ${MEGA_API_DIR}/mega/clock.hpp
    ${MEGA_API_DIR}/mega/common_strings.hpp 
    ${MEGA_API_DIR}/mega/common.hpp 
    ${MEGA_API_DIR}/mega/coroutine.hpp
    ${MEGA_API_DIR}/mega/eg_common_strings.hpp
    ${MEGA_API_DIR}/mega/event_log.hpp
    ${MEGA_API_DIR}/mega/event.hpp
    ${MEGA_API_DIR}/mega/include.hpp
    ${MEGA_API_DIR}/mega/invocation_id.hpp
    ${MEGA_API_DIR}/mega/invocation_io.hpp
    ${MEGA_API_DIR}/mega/iterators.hpp
    ${MEGA_API_DIR}/mega/macros.hpp
    ${MEGA_API_DIR}/mega/mpo.hpp
    ${MEGA_API_DIR}/mega/native_types.hpp
    ${MEGA_API_DIR}/mega/operations.hpp
    ${MEGA_API_DIR}/mega/reference_io.hpp
    ${MEGA_API_DIR}/mega/reference.hpp
    ${MEGA_API_DIR}/mega/result_type.hpp
    ${MEGA_API_DIR}/mega/scheduler.hpp
    ${MEGA_API_DIR}/mega/snapshot.hpp
    ${MEGA_API_DIR}/mega/type_id.hpp
    ${MEGA_API_DIR}/mega/type_instance.hpp
    ${MEGA_API_DIR}/mega/type_path.hpp
    ${MEGA_API_DIR}/mega/variant.hpp
    ${MEGA_API_DIR}/mega/xml_archive.hpp

)

set( MEGA_TYPES
    ${MEGA_API_DIR}/mega/types/traits.hpp
    ${MEGA_API_DIR}/mega/types/traits.cpp

    ${MEGA_API_DIR}/mega/types/simple.xmc
    ${MEGA_API_DIR}/mega/types/non_simple.xmc
    ${MEGA_API_DIR}/mega/types/allocators_32.xmc
    ${MEGA_API_DIR}/mega/types/allocators_64.xmc
    ${MEGA_API_DIR}/mega/types/allocators_128.xmc
)

install( FILES ${MEGA_LIBRARY} DESTINATION include/mega )
install( FILES ${MEGA_TYPES} DESTINATION include/mega/types )