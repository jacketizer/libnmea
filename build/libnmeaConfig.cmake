# - Config file for the libnmea package

# Compute paths
get_filename_component(LIBNMEA_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(LIBNMEA_INCLUDE_DIRS "/home/aurelien/dev-tmp/libnmea;/home/aurelien/dev-tmp/libnmea/build")

# Our library dependencies (contains definitions for IMPORTED targets)
# if(NOT TARGET libnmea)
#   include("${LIBNMEA_CMAKE_DIR}/libnmeaTargets.cmake")
# endif()

# These are IMPORTED targets created by libnmeaTargets.cmake
IF(WITH_DBUS)
    set(LIBNMEA_LIBRARIES libnmea libnmea-dbus)
ELSE(WITH_DBUS)
    set(LIBNMEA_LIBRARIES libnmea)
ENDIF(WITH_DBUS)
