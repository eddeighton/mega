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

#get common
include( ${WORKSPACE_ROOT_PATH}/src/common/common_include.cmake )

#################################################################
## generate colours
add_custom_target( generate_svg_colours_hxx
    COMMAND inja
        --data ${MEGA_API_DIR}/reports/colours.json
        --template ${MEGA_API_DIR}/reports/colours.hxx.jinja
        --output ${MEGA_API_DIR}/reports/colours.hxx
    DEPENDS ${MEGA_API_DIR}/reports/colours.json ${MEGA_API_DIR}/reports/colours.hxx.jinja inja
    BYPRODUCTS ${MEGA_API_DIR}/reports/colours.hxx
    COMMENT "Generating SVG Colours" 
    SOURCES ${MEGA_API_DIR}/reports/colours.json
)
add_custom_target( generate_svg_colours_cxx
    COMMAND inja
        --data ${MEGA_API_DIR}/reports/colours.json
        --template ${MEGA_SRC_DIR}/reports/colours.cxx.jinja
        --output ${MEGA_SRC_DIR}/reports/colours.cxx
    DEPENDS ${MEGA_API_DIR}/reports/colours.json ${MEGA_SRC_DIR}/reports/colours.cxx.jinja inja
    BYPRODUCTS ${MEGA_SRC_DIR}/reports/colours.cxx
    COMMENT "Generating SVG Colours" 
    SOURCES ${MEGA_API_DIR}/reports/colours.json
)

#################################################################
set( JINJA_TEMPLATES 
	${MEGA_SRC_DIR}/reports/templates/branch.jinja
	${MEGA_SRC_DIR}/reports/templates/graph.jinja
	${MEGA_SRC_DIR}/reports/templates/multiline.jinja
	${MEGA_SRC_DIR}/reports/templates/report.jinja
	${MEGA_SRC_DIR}/reports/templates/table.jinja
	)

set( REPORTS_HEADERS
    ${MEGA_API_DIR}/reports/colours.hxx
    ${MEGA_API_DIR}/reports/renderer.hpp
    ${MEGA_API_DIR}/reports/report.hpp
    ${MEGA_API_DIR}/reports/reporter_id.hpp
    ${MEGA_API_DIR}/reports/reporter.hpp
    ${MEGA_API_DIR}/reports/url.hpp
    ${MEGA_API_DIR}/reports/value.hpp
)

set( REPORTS_SOURCE
    ${MEGA_SRC_DIR}/reports/colours.cxx
    ${MEGA_SRC_DIR}/reports/renderer.cpp
)

add_library( reports
    ${REPORTS_HEADERS}
    ${REPORTS_SOURCE}
)

add_dependencies( reports generate_mega_mangle_meta )

set_target_properties( reports PROPERTIES FOLDER reports )

IF( ${WIN32} )
target_compile_options(reports PRIVATE /bigobj)
set_property(TARGET reports PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON )
ELSE( ${WIN32} )
#set_property(TARGET reports PROPERTY POSITION_INDEPENDENT_CODE ON)
ENDIF( ${WIN32} )

target_link_libraries( reports mega_mangle )

target_include_directories( reports PUBLIC ${MEGA_API_DIR} )

link_boost( reports system )
link_boost( reports filesystem )
link_boost( reports serialization )
link_json( reports )
link_inja( reports )
link_common( reports )

#################################################################
#################################################################
## install everything
install( FILES ${JINJA_TEMPLATES} DESTINATION templates )
install( FILES ${REPORTS_HEADERS} DESTINATION include/reports )
install( TARGETS reports EXPORT reports DESTINATION bin)
install( EXPORT reports NAMESPACE MEGA:: FILE reports-config.cmake DESTINATION share )

install( TARGETS reports DESTINATION bin)
