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

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get json
include( ${WORKSPACE_ROOT_PATH}/thirdparty/nlohmann/json_include.cmake )

#get inja
include( ${WORKSPACE_ROOT_PATH}/thirdparty/inja/inja_include.cmake )

#get clang
include( ${WORKSPACE_ROOT_PATH}/thirdparty/llvm/clang_include.cmake )

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

include_directories( ${MEGA_API_DIR} )
include_directories( ${MEGA_SRC_DIR} )

#get the source code
set( IL_SOURCE_DIR ${MEGA_SRC_DIR}/il/il_gen )
set( IL_TEMPLATES_DIR ${IL_SOURCE_DIR}/templates )

set( IL_SOURCES
		${IL_SOURCE_DIR}/main.cpp
		${IL_SOURCE_DIR}/parser.cpp
		${IL_SOURCE_DIR}/generator.cpp
	)
	
source_group( src FILES ${IL_SOURCES} )

add_executable( il_gen ${IL_SOURCES} )

link_boost( il_gen program_options )
link_boost( il_gen filesystem )
link_boost( il_gen serialization )
link_json( il_gen )
link_inja( il_gen )
link_clang_tooling( il_gen )
link_common( il_gen )

install( TARGETS il_gen DESTINATION bin)

###########################################################
###########################################################

set( NATIVE_FILES
	${MEGA_API_DIR}/mega/values/native_types.hpp
)

set( INLINE_FILES
	${MEGA_API_DIR}/mega/values/inline_types.h
)

set( EXTERN_FILES
	${MEGA_API_DIR}/runtime/support.hpp
)

set( MATERIALISER_FILES
	${MEGA_API_DIR}/il/materialisers/object.hpp
	${MEGA_API_DIR}/il/materialisers/relation.hpp
	${MEGA_API_DIR}/il/materialisers/invocation.hpp
)

add_custom_target( run_il_gen
    COMMAND il_gen
        --native \"${NATIVE_FILES}\"
        --inline \"${INLINE_FILES}\"
        --extern \"${EXTERN_FILES}\"
        --materialisers \"${MATERIALISER_FILES}\"
        --inja ${IL_TEMPLATES_DIR}
        --il_folder ${MEGA_API_DIR}/il/elements
        --mega_include_dir ${MEGA_API_DIR}
        --runtime_functors_include_dir ${MEGA_API_DIR}/runtime/
        --runtime_functors_impl_dir ${MEGA_SRC_DIR}/runtime/
        
    DEPENDS ${INLINE_FILES} ${EXTERN_FILES} ${TEMPLATE_FILES} il_gen
    COMMENT "Running IL Gen" 
    SOURCES ${INLINE_FILES} ${EXTERN_FILES} ${TEMPLATE_FILES}
)
add_dependencies( run_il_gen il_gen )