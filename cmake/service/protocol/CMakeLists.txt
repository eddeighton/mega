
cmake_minimum_required( VERSION 3.1...3.16 )

#get protobuf
include( ${WORKSPACE_ROOT_PATH}/thirdparty/protobuf/protobuf_include.cmake )

set( MEGA_PROTOCOL_SCHEMA ${MEGA_SRC_DIR}/service/protocol/schema.proto )

#files
set( MEGA_PROTOCOL_DIR ${MEGA_SRC_DIR}/service/protocol )
set( MEGA_PROTOCOL_HEADER ${MEGA_PROTOCOL_DIR}/schema.pb.h )
set( MEGA_PROTOCOL_SOURCE ${MEGA_PROTOCOL_DIR}/schema.pb.cc )

add_custom_command(
	  PRE_BUILD
      OUTPUT "${MEGA_PROTOCOL_HEADER}"
             "${MEGA_PROTOCOL_SOURCE}"
      COMMAND ${Protobuf_PROTOC_EXECUTABLE}
      ARGS --cpp_out=${MEGA_PROTOCOL_DIR} --proto_path=${MEGA_PROTOCOL_DIR} ${MEGA_PROTOCOL_SCHEMA}
      DEPENDS ${MEGA_PROTOCOL_SCHEMA}
      COMMENT "Running C++ protocol buffer compiler for the mega structure protocol"
      VERBATIM )


#add_custom_command(
#    OUTPUT ${MEGASTRUCTURE_PROTOCOL_API_DIR}/megastructure.pb.h
#    COMMAND ${CMAKE_COMMAND} -E copy ${MEGA_PROTOCOL_HEADER} ${MEGASTRUCTURE_PROTOCOL_API_DIR}/megastructure.pb.hxx
#    MAIN_DEPENDENCY ${MEGASTRUCTURE_PROTOCOL_HEADER} )


set( MEGA_PROTOCOL_API 
    ${MEGA_PROTOCOL_HEADER}
)

set( MEGA_PROTOCOL_SRC 
    ${MEGA_PROTOCOL_SOURCE}
)

add_library( protocol
                    ${MEGA_PROTOCOL_API}
					${MEGA_PROTOCOL_SRC}
				)

target_include_directories( protocol PUBLIC ${MEGA_API_DIR} )

set_target_properties( protocol PROPERTIES FOLDER protocol )

link_protobuf( protocol )

install( FILES ${MEGA_PROTOCOL_API} DESTINATION include/service/protocol )
install( TARGETS protocol EXPORT protocol DESTINATION bin)
install( EXPORT protocol NAMESPACE MEGA:: FILE protocol-config.cmake DESTINATION share )
                