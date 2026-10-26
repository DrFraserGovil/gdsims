cmake_minimum_required(VERSION 3.15.0)
project(gdsims VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  
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
		GIT_TAG        v3.4.2
	)
	FetchContent_MakeAvailable(JSL)
endif()



### COMPILE THE LIBRARY (Everything except main.cpp)
	file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
	list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

	add_library(gdsimslib STATIC ${LIB_SOURCES})
	target_link_libraries(gdsimslib PUBLIC JSL)
	target_include_directories(gdsimslib PUBLIC includes)

### COMPILE THE EXECUTABLE
	add_executable(gdsims src/main.cpp)
	target_link_libraries(gdsims PUBLIC gdsimslib)


# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
