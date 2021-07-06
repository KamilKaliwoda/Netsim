cmake_minimum_required(VERSION 3.13)
project(net_simulation)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Werror=switch)

# All the header files.
include_directories(include)

# A variable for all source files.
# When you create a file, add the path to it here.
set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        src/helpers.cpp
        src/factory.cpp
        src/nodes.cpp include/reports.hpp src/reports.cpp include/simulation.hpp src/simulation.cpp)

# Test files.
# Analogous to the source files.
set(SOURCE_FILES_TESTS
        test/main_test.cpp
        test/test_storage_types.cpp
        test/test_package.cpp
        test/test_nodes.cpp
        test/test_Factory.cpp
        test/test_factory_io.cpp
        test/test_reports.cpp
        test/test_simulate.cpp)

# Sets the build configuration of the application, based on the .cpp file, here test.
add_executable(main_tests ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_test.cpp test/test_factory_io.cpp)

target_compile_definitions(main_tests PUBLIC)

target_include_directories(main_tests PUBLIC
        googletest-master/googlemock/include
        googletest-master/googletest/include
        mocks
        )

add_subdirectory(googletest-master)

# When you create a new test file, you should also add the gmock library to it
# library to it using this command.
# NOTE: main_test here means a variable referring to the file test/main_test.cpp.
target_link_libraries(main_tests gmock)


