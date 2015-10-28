cmake_minimum_required (VERSION 2.8)
project(libnmea C)

option(NMEA_BUILD_STATIC_LIB "Build static libnmea" ON)
option(NMEA_BUILD_SHARED_LIB "Build shared libnmea" ON)
option(NMEA_BUILD_EXAMPLES "Build examples" ON)
option(NMEA_EXAMPLES_LINK_STATIC "Link examples statically" OFF)
option(NMEA_UNIT_TESTS "Build unit tests" ON)
option(NMEA_UNIT_TESTS_LINK_STATIC "Link unit tests statically" OFF)
option(NMEA_WITH_MEMCHECK "Run unit tests in valgrind" ON)

if (NOT NMEA_BUILD_STATIC_LIB AND NOT NMEA_BUILD_SHARED_LIB)
    message(FATAL_ERROR "You must build either shared or static lib, or both")
endif()

if (NOT NMEA_BUILD_SHARED_LIB)
    set(NMEA_EXAMPLES_LINK_STATIC ON)
    set(NMEA_UNIT_TESTS_LINK_STATIC ON)
endif()

if (NOT NMEA_BUILD_STATIC_LIB)
    message("Linking examples/unit tests to shared lib since NMEA_BUILD_STATIC_LIB is turned off")
    set(NMEA_EXAMPLES_LINK_STATIC OFF)
    set(NMEA_UNIT_TESTS_LINK_STATIC OFF)
endif()

# Set some nicer output dirs.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# This is the location of the parser libraries
set(NMEA_PLUGIN_DIRECTORY ${PROJECT_BINARY_DIR}/parsers/)

set(NMEA_SRC
    src/nmea/nmea.c
    src/nmea/parser.c)

set(NMEA_HDR
    src/nmea/nmea.h
    src/nmea/parser.h
    src/nmea/parser_types.h)

source_group("Headers" FILES ${NMEA_HDR})
source_group("Source" FILES ${NMEA_SRC})

if (NMEA_BUILD_STATIC_LIB)
    add_library(nmea STATIC ${NMEA_SRC})
    target_link_libraries(nmea dl)
    install(TARGETS nmea DESTINATION lib)
endif()

if (NMEA_BUILD_SHARED_LIB)
    if (POLICY CMP0042)
        cmake_policy(PUSH)
        cmake_policy(SET CMP0042 OLD)
    endif()

    add_library(nmea_shared SHARED ${NMEA_SRC})
    target_link_libraries(nmea_shared dl)

    if (UNIX)
        set_target_properties(nmea_shared PROPERTIES OUTPUT_NAME nmea)
    endif()

    install(TARGETS nmea_shared DESTINATION lib)

    if (POLICY CMP0042)
        cmake_policy(POP)
    endif()
endif()

# Build parsers.
file(GLOB PARSERS_SRCS
    RELATIVE "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/src/parsers/*.c")
list(REMOVE_ITEM PARSERS_SRCS src/parsers/parse.c)

source_group("Parser Sources" FILES ${PARSERS_SRCS})

foreach(PARSER_SRC ${PARSERS_SRCS})
    get_filename_component(PARSER_NAME ${PARSER_SRC} NAME_WE)
    add_library(${PARSER_NAME} SHARED ${PARSER_SRC} src/parsers/parse.c)

    # Copy parser plugins to separate cmake build directory.
    set_target_properties(${PARSER_NAME} PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY ${NMEA_PLUGIN_DIRECTORY})

    # Install to where we expect this to be on the system (hard coded).
    # Otherwise using absolute paths like this is not the CMake way.
    install(TARGETS ${PARSER_NAME} DESTINATION /usr/lib/nmea/)
endforeach()

# Find parser headers as well.
file(GLOB PARSERS_HDRS
    RELATIVE "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/src/parsers/*.h")
list(REMOVE_ITEM PARSERS_HDRS parse.h)
list(APPEND ${NMEA_PUB_HDR} ${PARSERS_HDRS})

# Install headers.
install(FILES src/nmea/nmea.h DESTINATION include)
install(FILES ${PARSERS_HDRS} DESTINATION include/nmea)

# And copy headers to build dir.
configure_file(src/nmea/nmea.h ${PROJECT_BINARY_DIR}/include/nmea.h COPYONLY)
foreach (HDR ${PARSERS_HDRS})
    get_filename_component(HDR_NAME ${HDR} NAME_WE)
    message("${HDR}")
    configure_file(${HDR} ${PROJECT_BINARY_DIR}/include/nmea/${HDR_NAME}.h COPYONLY)
endforeach()

include_directories("${PROJECT_BINARY_DIR}/include/")

if (NMEA_BUILD_EXAMPLES)
    # Find all example sources.
    file(GLOB EXAMPLE_SRCS
        RELATIVE "${PROJECT_SOURCE_DIR}"
        "${PROJECT_SOURCE_DIR}/examples/*.c")

    foreach (EXAMPLE_SRC ${EXAMPLE_SRCS})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_SRC} NAME_WE)

        add_executable(${EXAMPLE_NAME} ${EXAMPLE_SRC})

        if (NMEA_EXAMPLES_LINK_STATIC)
            target_link_libraries(${EXAMPLE_NAME} nmea)
        else()
            target_link_libraries(${EXAMPLE_NAME} nmea_shared)
        endif()
    endforeach()
endif()

if (NMEA_UNIT_TESTS)
    ENABLE_TESTING()

    # Find test sources.
    file(GLOB TEST_SRCS
        RELATIVE "${PROJECT_SOURCE_DIR}"
        "${PROJECT_SOURCE_DIR}/tests/test_*.c")

    # Find valgrind if we're supposed to check for leaks when
    # running the unit tests.
    if (NMEA_WITH_MEMCHECK)
        find_program(VALGRIND_PROGRAM NAMES valgrind)

        if (NOT EXISTS ${VALGRIND_PROGRAM})
            message(WARNING "Valgrind not found! Tests will be run without memory check. Turn this warning off by installing valgrind or settings -DNMEA_WITH_MEMCHECK=OFF")
            set(NMEA_WITH_MEMCHECK OFF)
        else()
            message("Found valgrind: ${VALGRIND_PROGRAM}")
        endif()
    endif()

    #
    # Lib unit tests.
    #
    add_executable(utests tests/test_lib.c)

    if (NMEA_UNIT_TESTS_LINK_STATIC)
        target_link_libraries(utests nmea)
    else()
        target_link_libraries(utests nmea_shared)
    endif()

    #
    # Parse unit tests.
    #
    add_executable(utests-parse src/parsers/parse.c tests/test_parse.c)

    #
    # NMEA unit tests.
    #
    add_executable(utests-nmea src/nmea/parser.c tests/test_nmea_helpers.c)
    target_link_libraries(utests-nmea dl)

    set(TESTS utests utests-parse utests-nmea)

    foreach(TEST_NAME ${TESTS})
        message("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_NAME}")
        if (NMEA_WITH_MEMCHECK)
            add_test("${TEST_NAME}_memchk" ${VALGRIND_PROGRAM} --gen-suppressions=all --error-exitcode=5 --leak-check=full ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_NAME})

            set_property(TEST ${TEST_NAME}_memchk PROPERTY ENVIRONMENT NMEA_PARSER_PATH=${NMEA_PLUGIN_DIRECTORY})
        else()
            add_test("${TEST_NAME}" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_NAME})

            # We must set this so we can find the plugins in the Cmake build dir when testing.
            set_property(TEST ${TEST_NAME} PROPERTY ENVIRONMENT NMEA_PARSER_PATH=${NMEA_PLUGIN_DIRECTORY})
        endif()
    endforeach()
endif()

