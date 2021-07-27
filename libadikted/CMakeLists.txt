##
##
##


## 3.1 -- CMAKE_C_STANDARD
cmake_minimum_required( VERSION 3.1 )


if( "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}" )
    message( FATAL_ERROR "Building in source is forbidden. Change output directory.")
endif()


project( libadikted LANGUAGES C )


if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE )
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    ## do not install accidentally to system directory
    set( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install path prefix" FORCE )
endif()


set(CMAKE_DEBUG_POSTFIX _d)


##
## set default compilation standard
##
set( CMAKE_C_STANDARD 99 )
set( CMAKE_C_STANDARD_REQUIRED ON )


##
## compiler flags, gcc does not handle "appended" values
##
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration -Wno-conversion -Wno-traditional-conversion -Wno-sign-compare" )



## ================= build =================

set( TARGET_NAME adikted )

if( NOT ${TARGET_NAME}_TYPE )
    ## build library as shared by default
    set( ${TARGET_NAME}_TYPE SHARED )
endif()

file( GLOB_RECURSE c_files *.c )
file( GLOB_RECURSE h_files *.h )

add_library( ${TARGET_NAME} ${${TARGET_NAME}_TYPE} ${c_files} )
target_include_directories( ${TARGET_NAME} PUBLIC ".." PRIVATE "." )
set_target_properties( ${TARGET_NAME} PROPERTIES PUBLIC_HEADER "${h_files}" )

install( TARGETS ${TARGET_NAME} 
         PUBLIC_HEADER DESTINATION include/lib${TARGET_NAME} COMPONENT headers
         LIBRARY DESTINATION lib COMPONENT binaries
        )
