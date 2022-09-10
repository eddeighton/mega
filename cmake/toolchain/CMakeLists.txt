

cmake_minimum_required(VERSION 3.2)

# determine toolchain locations
set( MEGA_INSTALL_PATH ${CMAKE_INSTALL_PREFIX} )
find_path(LLVM_CONFIGURATION_PATH NAMES "llvm/install" PATHS ${BUILD_PATH}/${CFG_TUPLE} REQUIRED NO_DEFAULT_PATH)
find_file(CLANG_COMPILER NAMES clang++ PATHS ${LLVM_CONFIGURATION_PATH}/llvm/install/bin REQUIRED NO_DEFAULT_PATH)

add_custom_target( GenerateToolChainXML
	COMMAND mega --tool_chain_hash
	
		--mega_compiler 	${MEGA_INSTALL_PATH}/bin/$<TARGET_FILE_NAME:compiler>
		--clang_compiler 	${CLANG_COMPILER}
		--parser_dll 		${MEGA_INSTALL_PATH}/bin/$<TARGET_FILE_NAME:parser>
		--clang_plugin 		${MEGA_INSTALL_PATH}/bin/$<TARGET_FILE_NAME:clang_plugin>
		--database_dll 		${MEGA_INSTALL_PATH}/bin/$<TARGET_FILE_NAME:database>
	
		--output_xml		${MEGA_INSTALL_PATH}/etc/toolchain.xml

		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
    
add_dependencies( GenerateToolChainXML install )
