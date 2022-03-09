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
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-switch -Wno-unused-parameter -Wno-unused-result" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration -Wno-conversion -Wno-traditional-conversion -Wno-sign-compare" )

file( GLOB_RECURSE EXAMPLES_PUTGEMS_SOURCES examples/putgems/*.c
	examples/putgems/*.h )
file( GLOB_RECURSE EXAMPLES_PUTTRAIN_SOURCES examples/puttrain/*.c
	examples/puttrain/*.h )
file( GLOB_RECURSE EXAMPLES_VIEWMAP_SOURCES examples/viewmap/*.c
	examples/viewmap/*.h )
file( GLOB_RECURSE EXAMPLES_PUTEMPLE_SOURCES examples/putemple/*.c
	examples/putemple/*.h )

find_package( SDL REQUIRED SDL::Main )
include_directories( ${SDL_INCLUDE_DIR} )

add_subdirectory( libadikted )
add_subdirectory( mapslang   )

add_executable( putgems ${EXAMPLES_PUTGEMS_SOURCES} )
target_link_libraries( putgems PUBLIC adikted m dl)
set_target_properties( putgems PROPERTIES RUNTIME_OUTPUT_DIRECTORY
	${CMAKE_BINARY_DIR} )
add_test( putgems putgems)

add_executable( puttrain ${EXAMPLES_PUTTRAIN_SOURCES} )
target_link_libraries( puttrain PUBLIC adikted m dl)
set_target_properties( putgems PROPERTIES RUNTIME_OUTPUT_DIRECTORY
	${CMAKE_BINARY_DIR} )
add_test( puttrain puttrain )

add_executable( viewmap ${EXAMPLES_VIEWMAP_SOURCES} )
target_link_libraries( viewmap PUBLIC adikted m dl ${SDL_LIBRARIES})
set_target_properties( viewmap PROPERTIES RUNTIME_OUTPUT_DIRECTORY
	${CMAKE_BINARY_DIR} )
add_test( viewmap viewmap )

add_executable( putemple ${EXAMPLES_PUTEMPLE_SOURCES} )
target_link_libraries( putemple PRIVATE adikted m dl ${SDL_LIBRARIES})
set_target_properties( putemple PROPERTIES RUNTIME_OUTPUT_DIRECTORY
	${CMAKE_BINARY_DIR} )
add_test( putemple putemple )

install(TARGETS putgems puttrain viewmap putemple EXPORT ${PROJECT_NAME}Config
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	PUBLIC_HEADER DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}
	)
