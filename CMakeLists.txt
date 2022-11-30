cmake_minimum_required (VERSION 3.5)
project(libnmea C)

set(LIBNMEA_MAJOR_VERSION 0)
set(LIBNMEA_MINOR_VERSION 1)
set(LIBNMEA_PATCH_VERSION 2)
set(LIBNMEA_VERSION
  ${LIBNMEA_MAJOR_VERSION}.${LIBNMEA_MINOR_VERSION}.${LIBNMEA_PATCH_VERSION})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

#Define cmake install path
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

option(NMEA_BUILD_STATIC_LIB "Build static libnmea" ON)
option(NMEA_BUILD_SHARED_LIB "Build shared libnmea" ON)
option(NMEA_BUILD_EXAMPLES "Build examples" ON)
option(NMEA_EXAMPLES_LINK_STATIC "Link examples statically" ON)
option(NMEA_UNIT_TESTS "Build unit tests" ON)
option(NMEA_UNIT_TESTS_LINK_STATIC "Link unit tests statically" ON)
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

# Set default warning flags for all targets in this directory
add_compile_options(-Wall -Werror)

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

set(LIBNMEA_TARGETS "")
if (NMEA_BUILD_STATIC_LIB)
    set(LIBNMEA_TARGETS ${LIBNMEA_TARGETS} nmea)
    add_library(nmea STATIC
        src/nmea/nmea.c
        src/nmea/parser_static.c
        src/parsers/parse.c)
    set_target_properties(nmea PROPERTIES VERSION ${LIBNMEA_VERSION})
    target_include_directories(nmea INTERFACE src/nmea src/parsers)
    if (UNIX)
        target_link_libraries(nmea ${CMAKE_DL_LIBS})
    endif()
    install(TARGETS nmea DESTINATION lib)
endif()

if (NMEA_BUILD_SHARED_LIB)
    if (POLICY CMP0042)
        cmake_policy(PUSH)
        cmake_policy(SET CMP0042 OLD)
    endif()

    set(LIBNMEA_TARGETS ${LIBNMEA_TARGETS} nmea_shared)

    add_library(nmea_shared SHARED ${NMEA_SRC})
    set_target_properties(nmea_shared PROPERTIES VERSION ${LIBNMEA_VERSION})
    target_include_directories(nmea_shared INTERFACE src/nmea src/parsers)

    if (UNIX)
        target_link_libraries(nmea_shared ${CMAKE_DL_LIBS})
        set_target_properties(nmea_shared PROPERTIES OUTPUT_NAME nmea)
    endif()

    install(TARGETS nmea_shared DESTINATION lib)
    target_compile_definitions(nmea_shared PRIVATE -DPARSER_PATH="${CMAKE_INSTALL_PREFIX}/lib/nmea/")

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
    if (NMEA_BUILD_STATIC_LIB)
        add_library(${PARSER_NAME} STATIC ${PARSER_SRC} src/parsers/parse.c)
    else()
        add_library(${PARSER_NAME} SHARED ${PARSER_SRC} src/parsers/parse.c)
    endif()

    # Copy parser plugins to separate cmake build directory.
    set_target_properties(${PARSER_NAME} PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY ${NMEA_PLUGIN_DIRECTORY})

    # Install to where we expect this to be on the system (hard coded).
    # Otherwise using absolute paths like this is not the CMake way.
    install(TARGETS ${PARSER_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/nmea/)
endforeach()

# Add parsers to static library
if (NMEA_BUILD_STATIC_LIB)
    list(LENGTH PARSERS_SRCS PARSER_CNT)
    target_compile_definitions(nmea PRIVATE -DPARSER_COUNT=${PARSER_CNT})
    # Add definition for each parser found and add dependency to rename symbols
    foreach(PARSER_SRC ${PARSERS_SRCS})
        get_filename_component(PARSER ${PARSER_SRC} NAME_WE)
        string(TOUPPER ${PARSER} PARSER)
        target_compile_definitions(nmea PRIVATE -DENABLE_${PARSER})
        string(TOLOWER ${PARSER} PARSER)
        add_custom_command(TARGET nmea PRE_LINK
            COMMAND ${CMAKE_OBJCOPY} --redefine-sym init=nmea_${PARSER}_init ${PARSER}.c${CMAKE_C_OUTPUT_EXTENSION}
            COMMAND ${CMAKE_OBJCOPY} --redefine-sym parse=nmea_${PARSER}_parse ${PARSER}.c${CMAKE_C_OUTPUT_EXTENSION}
            COMMAND ${CMAKE_OBJCOPY} --redefine-sym set_default=nmea_${PARSER}_set_default ${PARSER}.c${CMAKE_C_OUTPUT_EXTENSION}
            COMMAND ${CMAKE_OBJCOPY} --redefine-sym allocate_data=nmea_${PARSER}_allocate_data ${PARSER}.c${CMAKE_C_OUTPUT_EXTENSION}
            COMMAND ${CMAKE_OBJCOPY} --redefine-sym free_data=nmea_${PARSER}_free_data ${PARSER}.c${CMAKE_C_OUTPUT_EXTENSION}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/nmea.dir/src/parsers/)
    endforeach()
    target_sources(nmea PRIVATE ${PARSERS_SRCS})
endif()

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

    # Find valgrind if we're supposed to check for leaks when
    # running the unit tests.
    if (NMEA_WITH_MEMCHECK)
        find_program(VALGRIND_PROGRAM NAMES valgrind)

        if (NOT EXISTS ${VALGRIND_PROGRAM})
            message(WARNING "Valgrind not found! Tests will be run without memory check. Turn this warning off by installing valgrind or settings -DNMEA_WITH_MEMCHECK=OFF")
            set(NMEA_WITH_MEMCHECK OFF)
        else()
            message(STATUS "Found valgrind: ${VALGRIND_PROGRAM}")
        endif()
    endif()

    #
    # Lib unit tests.
    #
    add_executable(utests tests/unit-tests/test_lib.c)

    if (NMEA_UNIT_TESTS_LINK_STATIC)
        target_link_libraries(utests nmea)
    else()
        target_link_libraries(utests nmea_shared)
    endif()

    #
    # Parse unit tests.
    #
    add_executable(utests-parse src/parsers/parse.c tests/unit-tests/test_parse.c)

    #
    # NMEA unit tests.
    #
    add_executable(utests-nmea src/nmea/parser.c tests/unit-tests/test_nmea_helpers.c)
    if (UNIX)
        target_link_libraries(utests-nmea ${CMAKE_DL_LIBS})
    endif()

    set(TESTS utests utests-parse utests-nmea)

    foreach(TEST_NAME ${TESTS})
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

# Handle CMake Config
# Add all targets to the build-tree export set
export(TARGETS ${LIBNMEA_TARGETS}
  FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ${PROJECT_NAME})

# Create the ${PROJECT_NAME}Config.cmake and ${PROJECT_NAME}ConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}/include")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(${PROJECT_NAME}Config.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${LIBNMEA_CMAKE_DIR}/${REL_INCLUDE_DIR}/${PROJECT_NAME}")
configure_file(${PROJECT_NAME}Config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake" @ONLY)
# ... for both
configure_file(${PROJECT_NAME}ConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" @ONLY)

# Install the ${PROJECT_NAME}Config.cmake and ${PROJECT_NAME}ConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
# install(EXPORT ${PROJECT_NAME}Targets DESTINATION
#  "${INSTALL_CMAKE_DIR}" COMPONENT dev)
