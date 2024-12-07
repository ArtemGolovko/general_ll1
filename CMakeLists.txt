cmake_minimum_required(VERSION 3.5) 

project(general_ll1)

SET(CMAKE_CXX_FLAGS_DEDUG
    "${CMAKE_CXX_FLAGS_DEBUG}"
    CACHE STRING "Flags used by the C++ compiler during debug builds."
    FORCE )

SET(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} /TC"
    CACHE STRING "Flags used by the C compiler during debug builds."
    FORCE )

SET(CMAKE_CXX_FLAGS_DEV
    "${CMAKE_CXX_FLAGS_DEBUG} /fsanitize=address"
    CACHE STRING "Flags used by the C++ compiler during dev builds."
    FORCE )

SET(CMAKE_C_FLAGS_DEV
    "${CMAKE_C_FLAGS_DEBUG} /fsanitize=address"
    CACHE STRING "Flags used by the C compiler during dev builds."
    FORCE )
SET(CMAKE_EXE_LINKER_FLAGS_DEV
    "${CMAKE_EXE_LINKER_FLAGS_DEBUG}"
    CACHE STRING "Flags used for linking binaries during dev builds."
    FORCE )
SET(CMAKE_SHARED_LINKER_FLAGS_DEV
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}"
    CACHE STRING "Flags used by the shared libraries linker during dev builds."
    FORCE )

MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_DEV
    CMAKE_C_FLAGS_DEV
    CMAKE_EXE_LINKER_FLAGS_DEV
    CMAKE_SHARED_LINKER_FLAGS_DEV)

list(APPEND CMAKE_CONFIGURATION_TYPES Dev)

include(CMakePrintHelpers)
include(CTest)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/vendor/Check/include)

file(GLOB_RECURSE LIB_SOURCES  ${PROJECT_SOURCE_DIR}/src/*.c)
list(FILTER LIB_SOURCES EXCLUDE REGEX "main")

add_library("${PROJECT_NAME}_lib" ${LIB_SOURCES})

add_subdirectory(${PROJECT_SOURCE_DIR}/tests)

add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME} "${PROJECT_NAME}_lib")
