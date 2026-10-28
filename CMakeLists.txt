cmake_minimum_required(VERSION 3.15.0)

## The version number lives in VERSION (single source of truth); editing it triggers a reconfigure
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" GDSIMS_VERSION LIMIT_COUNT 1)
string(STRIP "${GDSIMS_VERSION}" GDSIMS_VERSION)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
project(gdsims VERSION ${GDSIMS_VERSION} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_CURRENT_SOURCE_DIR}>") # $<1:...> stops VS/Xcode adding a Release/ subfolder
set(CMAKE_CXX_EXTENSIONS OFF)
 
## cmake libraries
include (FetchContent) # Needed for JSL and Catch2 

 
### ADD THE JSL LIBRARY
option(FORCE_JSL_CLONE "Force a JSL clone instead of using a local copy" OFF)
set(LOCAL_JSL "/home/frasergj/docs/Code/JSL")
if((NOT FORCE_JSL_CLONE) AND (EXISTS "${LOCAL_JSL}/CMakeLists.txt"))
	message(STATUS "Using local JSL from ${LOCAL_JSL}")
	add_subdirectory("${LOCAL_JSL}" "${CMAKE_CURRENT_BINARY_DIR}/jsl_build")
else()
	message(STATUS "Using JSL from github")
	FetchContent_Declare(
		JSL
		GIT_REPOSITORY https://github.com/DrFraserGovil/JSL.git
		GIT_TAG        v3.5.1
	)
	FetchContent_MakeAvailable(JSL)
endif()

### SET COMPILER OPTIMS & WARNINGS
	## An empty 'library' that duplicates warnings & compiler optimisations
	add_library(gdsims_warnings INTERFACE)
	if(MSVC)
		target_compile_options(gdsims_warnings INTERFACE /W4)
	else()
		target_compile_options(gdsims_warnings INTERFACE -Wall -Wextra -Wpedantic)
	endif()

	## Debug-only extras: bounds-checked STL and sanitizers (GCC/Clang only)
	option(GDSIMS_SANITIZE "Enable AddressSanitizer + UBSan in Debug builds" ON)
	if(NOT MSVC)
		target_compile_options(gdsims_warnings INTERFACE $<$<CONFIG:Debug>:-D_GLIBCXX_ASSERTIONS>)
		if(GDSIMS_SANITIZE)
			target_compile_options(gdsims_warnings INTERFACE "$<$<CONFIG:Debug>:-fsanitize=address,undefined;-fno-omit-frame-pointer>")
			target_link_options(gdsims_warnings INTERFACE "$<$<CONFIG:Debug>:-fsanitize=address,undefined>")
		endif()

		## Profile builds: keep frame pointers so `perf record -g` gives reliable call stacks
		target_compile_options(gdsims_warnings INTERFACE $<$<CONFIG:RelWithDebInfo>:-fno-omit-frame-pointer>)
	endif()

### COMPILE THE LIBRARY (Everything except src/app)
	file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
	list(FILTER LIB_SOURCES EXCLUDE REGEX "/src/app/")        # all of app/ stays out of the library

	add_library(gdsimslib STATIC ${LIB_SOURCES})
	target_link_libraries(gdsimslib PUBLIC JSL)
	target_link_libraries(gdsimslib PRIVATE gdsims_warnings)
	target_include_directories(gdsimslib PUBLIC src)

	## version.h is generated into the build dir, so `#include "version.h"` works from anywhere
	configure_file(src/version.h.in "${CMAKE_CURRENT_BINARY_DIR}/generated/version.h" @ONLY)
	target_include_directories(gdsimslib PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/generated")

### COMPILE THE EXECUTABLE
	file(GLOB APP_SOURCES CONFIGURE_DEPENDS "src/app/*.cpp")
	add_executable(gdsims ${APP_SOURCES})
	target_link_libraries(gdsims PUBLIC gdsimslib)
	target_link_libraries(gdsims PRIVATE gdsims_warnings)
	# so debug/profile builds don't overwrite ./gdsims
	set_target_properties(gdsims PROPERTIES DEBUG_POSTFIX "_debug" RELWITHDEBINFO_POSTFIX "_profile")

### UNIT TESTS (Catch2)
	option(GDSIMS_BUILD_TESTS "Build the Catch2 unit test suite" OFF)
	if(GDSIMS_BUILD_TESTS)
		enable_testing()
		add_subdirectory(tests)
	endif()


