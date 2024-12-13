if (BUILD_TESTING)
    find_library(
        CHECK_LIB check
        PATHS ${PROJECT_SOURCE_DIR}/vendor/Check/lib 
    )

    if (NOT CHECK_LIB)
        message(FATAL_ERROR "Check library not found")
    endif()

    add_custom_target(tests)
    file(GLOB CHECK_TESTS ${PROJECT_SOURCE_DIR}/tests/*.c)
    foreach (TEST ${CHECK_TESTS})
        get_filename_component(TEST_NAME ${TEST} NAME_WE)
        add_executable(${TEST_NAME} ${TEST})
        target_link_libraries(${TEST_NAME} "${PROJECT_NAME}_lib")
        target_link_libraries(${TEST_NAME} ${CHECK_LIB})
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
        add_dependencies(tests ${TEST_NAME})
    endforeach()
endif()
