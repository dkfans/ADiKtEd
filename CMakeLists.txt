cmake_minimum_required( VERSION 3.1 )

project( ADiKtED LANGUAGES C )
set(PROJECT_DESCRIPTION "Dungeon Keeper 1 map editor")

include(GNUInstallDirs)
include(FindPkgConfig)

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE )
endif()

set( CMAKE_C_STANDARD 99 )
set( CMAKE_C_STANDARD_REQUIRED ON )
set( CMAKE_C_STANDARD_LIBRARIES_INIT "-lm ${CMAKE_DL_LIBS}" )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-switch" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration -Wno-conversion -Wno-traditional-conversion -Wno-sign-compare" )

add_subdirectory(libadikted)
add_subdirectory(mapslang)

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
	target_link_libraries( ${ARG_NAME} PUBLIC
		adikted ${CMAKE_C_STANDARD_LIBRARIES} )
	find_package( SDL REQUIRED )
	target_link_libraries( ${ARG_NAME} PUBLIC ${SDL_LIBRARY} ${CMAKE_C_STANDARD_LIBRARIES} )
	target_compile_definitions( ${ARG_NAME} PUBLIC ENTRY_CONFIG_USE_SDL )
	target_link_libraries( ${ARG_NAME} PUBLIC X11 )
	target_include_directories(${ARG_NAME} PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/examples/putgems>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/lib${TARGET_NAME}>
		${SDL_INCLUDE_DIR}
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.)

	# Directory name
	set_target_properties( ${ARG_NAME} PROPERTIES FOLDER
		"${CMAKE_CURRENT_SOURCE_DIR}/examples" )
	add_test(${ARG_NAME} ${ARG_NAME})
endfunction()

option( ADIKTED_BUILD_EXAMPLES "Build ADiKtEd examples" ON )
if( ADIKTED_BUILD_EXAMPLES )
	# Add examples
	set(
		ADIKTED_EXAMPLES
		putgems
		puttrain
		viewmap
		putemple
	)

foreach( EXAMPLE ${ADIKTED_EXAMPLES} )
		add_example( ${EXAMPLE} )
	endforeach()
endif()

install( TARGETS ${ADIKTED_EXAMPLES} RUNTIME
	DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT binaries)
