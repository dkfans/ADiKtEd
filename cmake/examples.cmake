function( add_example ARG_NAME )
	list( APPEND ARG_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/examples/${ARG_NAME}" )
	set( SOURCES "" )
	set( RESOURCES "" )
	foreach( DIR ${ARG_DIRECTORIES} )
		file( GLOB GLOB_SOURCES ${DIR}/*.c ${DIR}/*.h )
		list( APPEND SOURCES ${GLOB_SOURCES} )
		file( GLOB GLOB_RESOURCES ${DIR}/*.rc )
		list( APPEND RESOURCES ${GLOB_RESOURCES} )
	endforeach()
	add_executable( ${ARG_NAME} ${SOURCES} )
	target_link_libraries( ${ARG_NAME} PUBLIC adikted )
	find_package( SDL REQUIRED )
	target_link_libraries( ${ARG_NAME} PUBLIC ${SDL_LIBRARY} )
	target_compile_definitions( ${ARG_NAME} PUBLIC ENTRY_CONFIG_USE_SDL )
	target_link_libraries( ${ARG_NAME} PUBLIC X11 Math::Math )
	target_include_directories(${ARG_NAME} PUBLIC Math::Math
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/examples/${ARG_NAME}>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/lib${TARGET_NAME}>
		${SDL_INCLUDE_DIR}
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.)

	# Directory name
	set_target_properties( ${ARG_NAME} PROPERTIES FOLDER
		"${CMAKE_CURRENT_SOURCE_DIR}/examples" )
	add_test(${ARG_NAME} ${ARG_NAME})
endfunction()

