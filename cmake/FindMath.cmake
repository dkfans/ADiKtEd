include (CheckLibraryExists)
set(Math_FOUND FALSE)
set(CMAKE_REQUIRED_QUIET_SAVE ${CMAKE_REQUIRED_QUIET})
set(CMAKE_REQUIRED_QUIET ${Math_FIND_QUIETLY})

if(CMAKE_C_COMPILER_LOADED)
  include (CheckIncludeFile)
  include (CheckCSourceCompiles)
elseif(CMAKE_CXX_COMPILER_LOADED)
  include (CheckIncludeFileCXX)
  include (CheckCXXSourceCompiles)
else()
  message(FATAL_ERROR "FindMath only works if either C or CXX language is enabled")
endif()

set(MATH_C_CXX_TEST_SOURCE [====[
#include <math.h>

int main()
{
  const double x = 7.3;
  ceil(x);
  acos(x);
  sin(x);
  sqrt(x);

  return 0;
}
]====])


if(CMAKE_C_COMPILER_LOADED)
	CHECK_INCLUDE_FILE("math.h" CMAKE_HAVE_MATH_H)
else()
	CHECK_INCLUDE_FILE_CXX("math.h" CMAKE_HAVE_MATH_H)
endif()

if(CMAKE_HAVE_MATH_H)
	set(CMAKE_HAVE_MATH_LIBRARY)

	set(CMAKE_REQUIRED_LIBRARIES -lm)
	if(CMAKE_C_COMPILER_LOADED)
		CHECK_C_SOURCE_COMPILES("${MATH_C_CXX_TEST_SOURCE}" CMAKE_HAVE_LIBC_LIBM)
	elseif(CMAKE_CXX_COMPILER_LOADED)
		CHECK_CXX_SOURCE_COMPILES("${MATH_C_CXX_TEST_SOURCE}" CMAKE_HAVE_LIBC_LIBM)
	endif()

	if(CMAKE_HAVE_LIBC_LIBM)
		set(CMAKE_MATH_LIBS_INIT ${CMAKE_REQUIRED_LIBRARIES})
		set(CMAKE_HAVE_MATH_LIBRARY 1)
		set(Math_FOUND TRUE)
	else()
		set(Math_FOUND FALSE)
	endif()

	unset(CMAKE_REQUIRED_LIBRARIES)
	set(CMAKE_REQUIRED_QUIET ${CMAKE_REQUIRED_QUIET_SAVE})

	include(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(Math DEFAULT_MSG Math_FOUND)

	if(MATH_FOUND AND NOT TARGET Math::Math)
		add_library(Math::Math INTERFACE IMPORTED)

		if(CMAKE_MATH_LIBS_INIT)
			set_property(TARGET Math::Math PROPERTY INTERFACE_LINK_LIBRARIES "${CMAKE_MATH_LIBS_INIT}")
		endif()
	elseif(NOT MATH_FOUND)
		file(APPEND
			${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
			"Determining if compiler accepts -lm failed with the following output:\n${_cmake_find_math_output}\n\n")
		message(SEND_ERROR "Required Math library not found!")
	endif()
endif()

unset(_cmake_find_math_output)
