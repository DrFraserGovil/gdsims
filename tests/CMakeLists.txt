FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")
add_executable(gdsims_tests ${TEST_SOURCES})
target_link_libraries(gdsims_tests PRIVATE gdsimslib gdsims_warnings Catch2::Catch2WithMain)

# put the test binary in tests/ (alongside the test sources) rather than the repo root
set_target_properties(gdsims_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_CURRENT_SOURCE_DIR}>")

list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(Catch)
catch_discover_tests(gdsims_tests WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
