cmake_minimum_required( VERSION 3.0 )
project(mapslang LANGUAGES C)

include(FindPkgConfig)
include(CMakePackageConfigHelpers)

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set( CMAKE_C_STANDARD 99 )
set( CMAKE_C_STANDARD_REQUIRED ON )
set( CMAKE_C_EXTENSIONS ON )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-discarded-qualifiers -Wno-unused-result -Wno-unused-parameter" )

set( MAPSLANG_LIBRARY_TYPE     "STATIC" CACHE STRING "Linking type for library" )

set_property( CACHE MAPSLANG_LIBRARY_TYPE PROPERTY STRINGS STATIC SHARED )

if( MAPSLANG_LIBRARY_TYPE MATCHES "SHARED" )
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

# Grab the keeperfx source files
file( GLOB MAPSLANG_SOURCES input_kb.c main.c output_scr.c scr_actn.c scr_clm.c scr_cube.c scr_help.c scr_list.c scr_rwrk.c scr_slab.c scr_thing.c scr_tileset.c scr_txted.c scr_txtgen.c textmenu.c var_utils.c )

add_executable( map ${MAPSLANG_SOURCES} )
target_include_directories( map
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if( UNIX AND NOT APPLE AND NOT EMSCRIPTEN AND NOT ANDROID )
	pkg_search_module(SLANG REQUIRED slang)
	pkg_search_module(ADIKTED REQUIRED libadikted)
	target_compile_options( map PUBLIC ${ADIKTED_CFLAGS} ${SLANG_CFLAGS})
	target_link_libraries( map PUBLIC ${ADIKTED_LIBRARIES}
		${SLANG_LIBRARIES} -ldl -lm)
endif()

# Put in a "mapslang" folder in Visual Studio
set_target_properties( map PROPERTIES FOLDER "mapslang" )

# Special Visual Studio Flags
if( MSVC )
	target_compile_definitions( map PRIVATE "_CRT_SECURE_NO_WARNINGS" )
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "None")
endif ()

set(uppercase_CMAKE_BUILD_TYPE TOUPPER "${CMAKE_BUILD_TYPE}" "NONE")
# Add debug config required in keeperfx headers since bx is private
if ( uppercase_CMAKE_BUILD_TYPE} STREQUAL "DEBUG" )
	target_compile_definitions( map PUBLIC "MAP_DEBUG=1" )
else()
	target_compile_definitions( map PUBLIC "MAP_DEBUG=0" )
endif()

install( TARGETS map
	EXPORT "${TARGETS_EXPORT_NAME}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	)

