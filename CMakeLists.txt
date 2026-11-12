cmake_minimum_required(VERSION 3.20)
project(aurora-ui VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(aurora STATIC
    src/Style.cpp
)
target_include_directories(aurora PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_options(aurora PRIVATE -Wall -Wextra -Wpedantic)
if (NOT MSVC)
    target_compile_options(aurora PRIVATE -O2)
endif()

option(AURORA_BUILD_EXAMPLES "Build Aurora UI examples" ON)
option(AURORA_BUILD_TESTS "Build Aurora UI tests" ON)

if (AURORA_BUILD_EXAMPLES)
    add_executable(demo examples/demo.cpp)
    target_link_libraries(demo PRIVATE aurora)
    add_executable(todo examples/todo_app.cpp)
    target_link_libraries(todo PRIVATE aurora)
    add_executable(gallery examples/gallery.cpp)
    target_link_libraries(gallery PRIVATE aurora)
endif()

if (AURORA_BUILD_TESTS)
    enable_testing()
    add_executable(test_core tests/test_core.cpp)
    target_link_libraries(test_core PRIVATE aurora)
    add_test(NAME core COMMAND test_core)
    add_executable(test_widgets tests/test_widgets.cpp)
    target_link_libraries(test_widgets PRIVATE aurora)
    add_test(NAME widgets COMMAND test_widgets)
endif()
