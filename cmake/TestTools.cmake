##############################################################################
#                      Varity of utilities for tests                         #
##############################################################################
include(CTest)

macro(create_test_command test_case)
    if(ENABLE_COVERAGE AND MSVC)
        set(TEST_COMMAND ${OPENCPPCOVERAGE_PATH} --export_type=binary:coverage/${test_case}.cov --sources $<SHELL_PATH:${CMAKE_SOURCE_DIR}/src> -- "$<TARGET_FILE:${test_case}>")
        set(REPORT_FILES "${REPORT_FILES} input_coverage=coverage/${test_case}.cov" CACHE INTERNAL "List of coverage file to use in the report")
    else(ENABLE_COVERAGE AND MSVC)
        set(TEST_COMMAND ${test_case})
    endif(ENABLE_COVERAGE AND MSVC)
endmacro(create_test_command)

macro(add_test_case test_case)
    add_executable(${test_case} "${test_case}.c")
    target_link_libraries(${test_case} gst-inspector)
    target_compile_definitions(${test_case} PRIVATE GST_USE_UNSTABLE_API)
    target_include_directories(${test_case} PRIVATE $<SHELL_PATH:${CMAKE_SOURCE_DIR}/tests>)
    
    create_test_command(${test_case})

    add_test(NAME ${test_case} COMMAND ${TEST_COMMAND} WORKING_DIRECTORY $<TARGET_FILE_DIR:gst-inspector>)

    # When building on VS, add GStreamer's root to PATH when running the tests
    if(MSVC)
    set_property(TEST ${test_case} PROPERTY ENVIRONMENT PATH=$<SHELL_PATH:${GSTREAMER_BIN_PATH}>;$ENV{Path})
    endif(MSVC)
endmacro(add_test_case)

# Give the option to create code coverage report for the tests
option(ENABLE_COVERAGE "Enable code coverage" OFF)

if (ENABLE_COVERAGE)
    if (MSVC)
        # Allow one to give a path to opencppcoverage
        set(REPORT_FILES "" CACHE INTERNAL "List of coverage file to use in the report")
        set(OPENCPPCOVERAGE_PATH opencppcoverage CACHE FILEPATH "Path to opencppcoverage program to use")
    else(MSVC)
        # Look for gcov and gcovr
    endif(MSVC)
endif(ENABLE_COVERAGE)
