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

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-switch" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration -Wno-conversion -Wno-traditional-conversion -Wno-sign-compare" )

# Add Math::Math
include(cmake/FindMath.cmake)

# Add examples
include(cmake/examples.cmake)
option( ADIKTED_BUILD_EXAMPLES "Build ADiKtEd examples" ON )
if( ADIKTED_BUILD_EXAMPLES )
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

add_subdirectory(libadikted)
add_subdirectory(mapslang)

install( TARGETS ${ADIKTED_EXAMPLES} RUNTIME
	DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT binaries)
