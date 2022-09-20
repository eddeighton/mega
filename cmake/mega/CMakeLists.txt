

cmake_minimum_required( VERSION 3.1...3.16 )

set( MEGA_LIBRARY
    ${MEGA_API_DIR}/mega/allocator.hpp
    ${MEGA_API_DIR}/mega/backtrace.hpp
    ${MEGA_API_DIR}/mega/basic_scheduler.hpp 
    ${MEGA_API_DIR}/mega/basic_scheduler.cpp 
    ${MEGA_API_DIR}/mega/clock.cpp
    ${MEGA_API_DIR}/mega/clock.hpp
    ${MEGA_API_DIR}/mega/common.hpp 
    ${MEGA_API_DIR}/mega/common_strings.hpp 
    ${MEGA_API_DIR}/mega/native_types.hpp
    ${MEGA_API_DIR}/mega/reference.hpp
    ${MEGA_API_DIR}/mega/coroutine.hpp
    ${MEGA_API_DIR}/mega/default_traits.hpp
    ${MEGA_API_DIR}/mega/eg_common_strings.hpp
    ${MEGA_API_DIR}/mega/event_log.cpp
    ${MEGA_API_DIR}/mega/event.hpp
    ${MEGA_API_DIR}/mega/include.hpp
    ${MEGA_API_DIR}/mega/iterators.hpp
    ${MEGA_API_DIR}/mega/invocation_id.hpp
    ${MEGA_API_DIR}/mega/macros.hpp
    ${MEGA_API_DIR}/mega/operations.hpp
    ${MEGA_API_DIR}/mega/result_type.hpp
    ${MEGA_API_DIR}/mega/root.hpp
    ${MEGA_API_DIR}/mega/scheduler.hpp
    ${MEGA_API_DIR}/mega/type_path.hpp
    ${MEGA_API_DIR}/mega/variant.hpp
)


install( FILES ${MEGA_LIBRARY} DESTINATION include/mega )