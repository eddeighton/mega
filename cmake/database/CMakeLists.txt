
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

set( MEGA_LIBRARY
	${MEGA_API_DIR}/mega/allocator.hpp
	${MEGA_API_DIR}/mega/backtrace.hpp
	${MEGA_API_DIR}/mega/clock.cpp
	${MEGA_API_DIR}/mega/clock.hpp
	${MEGA_API_DIR}/mega/common.hpp
	${MEGA_API_DIR}/mega/coroutine.hpp
	${MEGA_API_DIR}/mega/default_traits.hpp
	${MEGA_API_DIR}/mega/eg_common_strings.hpp
	${MEGA_API_DIR}/mega/event.hpp
	${MEGA_API_DIR}/mega/event_log.cpp
	${MEGA_API_DIR}/mega/frame.hpp
	${MEGA_API_DIR}/mega/frame.cpp
	${MEGA_API_DIR}/mega/include.hpp
	${MEGA_API_DIR}/mega/iterators.hpp
	${MEGA_API_DIR}/mega/macros.hpp
	${MEGA_API_DIR}/mega/operations.hpp
	${MEGA_API_DIR}/mega/resource.cpp
	${MEGA_API_DIR}/mega/resource.hpp
	${MEGA_API_DIR}/mega/result_type.hpp
	${MEGA_API_DIR}/mega/scheduler.hpp
	${MEGA_API_DIR}/mega/basic_scheduler.cpp
	${MEGA_API_DIR}/mega/type_path.hpp
	${MEGA_API_DIR}/mega/variant.hpp
	)
	
set( MEGA_DATABASE_MODEL
    ${MEGA_API_DIR}/database/model/eg.hpp
    ${MEGA_API_DIR}/database/model/objects.hpp
    ${MEGA_API_DIR}/database/model/input.hpp
    ${MEGA_API_DIR}/database/model/interface.hpp
    ${MEGA_API_DIR}/database/model/allocator.hpp
    ${MEGA_API_DIR}/database/model/concrete.hpp
    ${MEGA_API_DIR}/database/model/derivation.hpp
    ${MEGA_API_DIR}/database/model/link.hpp
    ${MEGA_API_DIR}/database/model/translation_unit.hpp
    ${MEGA_API_DIR}/database/model/layout.hpp
    ${MEGA_API_DIR}/database/model/identifiers.hpp
    ${MEGA_API_DIR}/database/model/name_resolution.hpp
    ${MEGA_API_DIR}/database/model/invocation.hpp
    ${MEGA_API_DIR}/database/model/instruction.hpp
	)
	
set( MEGA_DATABASE_SESSIONS
    ${MEGA_API_DIR}/database/stages/stage.hpp
    ${MEGA_API_DIR}/database/stages/parser.hpp
    ${MEGA_API_DIR}/database/stages/interface.hpp
    ${MEGA_API_DIR}/database/stages/implementation.hpp
    ${MEGA_API_DIR}/database/stages/operations.hpp
	)
	
set( MEGA_DATABASE_IO
    ${MEGA_API_DIR}/database/io/archive.hpp
    ${MEGA_API_DIR}/database/io/indexed_object.hpp
    ${MEGA_API_DIR}/database/io/loader.hpp
    ${MEGA_API_DIR}/database/io/storer.hpp
	)
	
set( MEGA_DATABASE_SOURCES
    ${MEGA_SRC_DIR}/database/io/indexed_object.cpp
    ${MEGA_SRC_DIR}/database/io/loader.cpp
    ${MEGA_SRC_DIR}/database/io/storer.cpp
    
    ${MEGA_SRC_DIR}/database/model/eg.cpp
    ${MEGA_SRC_DIR}/database/model/objects.cpp
    ${MEGA_SRC_DIR}/database/model/input.cpp
    ${MEGA_SRC_DIR}/database/model/interface.cpp
    ${MEGA_SRC_DIR}/database/model/allocator.cpp
    ${MEGA_SRC_DIR}/database/model/concrete.cpp
    ${MEGA_SRC_DIR}/database/model/derivation.cpp
    ${MEGA_SRC_DIR}/database/model/link.cpp
    ${MEGA_SRC_DIR}/database/model/translation_unit.cpp
    ${MEGA_SRC_DIR}/database/model/layout.cpp
    ${MEGA_SRC_DIR}/database/model/identifiers.cpp
    ${MEGA_SRC_DIR}/database/model/name_resolution.cpp
    ${MEGA_SRC_DIR}/database/model/invocation.cpp
    ${MEGA_SRC_DIR}/database/model/instruction.cpp
    
    ${MEGA_SRC_DIR}/database/stages/stage.cpp
    ${MEGA_SRC_DIR}/database/stages/parser.cpp
    ${MEGA_SRC_DIR}/database/stages/interface.cpp
    ${MEGA_SRC_DIR}/database/stages/implementation.cpp
    ${MEGA_SRC_DIR}/database/stages/operations.cpp
    
    )
    
add_library( database
                    ${MEGA_DATABASE_IO}
					${MEGA_DATABASE_MODEL}
					${MEGA_DATABASE_STAGES}
					${MEGA_DATABASE_SOURCES}
				)

set_target_properties( database PROPERTIES FOLDER database )
set_property(TARGET database PROPERTY POSITION_INDEPENDENT_CODE ON)

link_boost( database filesystem )
link_boost( database iostreams )
link_boost( database serialization )
link_common( database )

install( FILES ${MEGA_LIBRARY} DESTINATION include/database/mega )
install( FILES ${MEGA_DATABASE_IO} DESTINATION include/database/io )
install( FILES ${MEGA_DATABASE_MODEL} DESTINATION include/database/model )
install( FILES ${MEGA_DATABASE_STAGES} DESTINATION include/database/stages )
install( TARGETS database EXPORT database DESTINATION lib)
install( EXPORT database NAMESPACE MEGA:: FILE database-config.cmake DESTINATION share )