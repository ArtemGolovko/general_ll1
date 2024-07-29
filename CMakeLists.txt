cmake_minimum_required(VERSION 3.5) 

project(general_ll1)

include(CTest)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(/TC /Zi /fsanitize=address)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/vendor/Check/include)

file(GLOB_RECURSE LIB_SOURCES  ${PROJECT_SOURCE_DIR}/src/*.c)
list(FILTER LIB_SOURCES EXCLUDE REGEX "main")

add_library("${PROJECT_NAME}_lib" ${LIB_SOURCES})

add_subdirectory(${PROJECT_SOURCE_DIR}/tests)

add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME} "${PROJECT_NAME}_lib")
