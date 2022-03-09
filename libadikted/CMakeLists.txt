cmake_minimum_required( VERSION 3.1 )

project( libadikted LANGUAGES C )
set(PROJECT_DESCRIPTION "Dungeon Keeper 1 ADiKtEd map editor C library"

include(GNUInstallDirs)
include(FindPkgConfig)

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE )
endif()

set(CMAKE_DEBUG_POSTFIX _d)

set( CMAKE_C_STANDARD 99 )
set( CMAKE_C_STANDARD_REQUIRED ON )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration -Wno-conversion -Wno-traditional-conversion -Wno-sign-compare" )



## ================= build =================

option( USE_RNG_MT "Use Mersenne Twister RNG" ON )
if( USE_RNG_MT )
    add_definitions( -DRNG_MT )
endif()


set( TARGET_NAME adikted )

if( NOT ${TARGET_NAME}_TYPE )
    ## build library as shared by default
    set( ${TARGET_NAME}_TYPE SHARED )
endif()


file( GLOB_RECURSE c_files *.c )
file( GLOB_RECURSE h_files *.h )

if( NOT USE_RNG_MT )
    list(REMOVE_ITEM h_files "${CMAKE_CURRENT_SOURCE_DIR}/mtwist/mtwist.h" )
    list(REMOVE_ITEM c_files "${CMAKE_CURRENT_SOURCE_DIR}/mtwist/mtwist.c" )
endif()

add_library( ${TARGET_NAME} ${${TARGET_NAME}_TYPE} ${c_files} )

target_include_directories(${TARGET_NAME} PUBLIC
			".."
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/lib${TARGET_NAME}>
			PRIVATE "."
			)

set_target_properties( ${TARGET_NAME} PROPERTIES PUBLIC_HEADER "${h_files}" )

# pkg-config variables
set(target_name "${TARGET_NAME}")
set(install_prefix "${CMAKE_INSTALL_PREFIX}")
set(install_libdir "${CMAKE_INSTALL_LIBDIR}")
set(install_includedir "${CMAKE_INSTALL_INCLUDEDIR}")

configure_file(lib${TARGET_NAME}.pc.in ${PROJECT_BINARY_DIR}/lib${TARGET_NAME}.pc @ONLY)

set(ENV{PKG_CONFIG_PATH} "${PROJECT_BINARY_DIR}:$ENV{PKG_CONFIG_PATH}")

install( TARGETS ${TARGET_NAME} EXPORT lib${TARGET_NAME}Config 
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT binaries
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lib${TARGET_NAME} COMPONENT headers
        )

install(FILES ${PROJECT_BINARY_DIR}/lib${TARGET_NAME}.pc DESTINATION
	${CMAKE_INSTALL_LIBDIR}/pkgconfig/)
