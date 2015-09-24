# - Try to find zmq library
# Once done this will define
#
#  ZMQ_FOUND - system has zmq
#  ZMQ_INCLUDE_DIR - the zmq include directory
#  ZMQ_LIBRARY - link these to use zmq
#

if (ZMQ_LIBRARY AND ZMQ_INCLUDE_DIR)
  set(ZMQ_FOUND TRUE)
else (ZMQ_LIBRARY AND ZMQ_INCLUDE_DIR)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  find_package(PkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(_ZMQ zmq)
  endif (PKG_CONFIG_FOUND)

  find_path(ZMQ_INCLUDE_DIR
    NAMES zmq.hpp
    PATHS ${_ZMQ_INCLUDE_DIRS} /usr/include /usr/local/include /opt/local/include
  )

  find_library(ZMQ_LIBRARY
    NAMES zmq
    PATHS ${_ZMQ_LIBRARY_DIRS} /usr/lib /usr/local/lib /opt/local/lib
  )

  if (ZMQ_LIBRARY AND ZMQ_INCLUDE_DIR)
    set(ZMQ_FOUND TRUE)
  endif()

  if (ZMQ_FOUND)
    if (NOT ZMQ_FIND_QUIETLY)
      message(STATUS "Found ZMQ: ${ZMQ_LIBRARY}, ${ZMQ_INCLUDE_DIR}")
    endif (NOT ZMQ_FIND_QUIETLY)
  else (ZMQ_FOUND)
    if (ZMQ_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find ZMQ")
    endif (ZMQ_FIND_REQUIRED)
  endif (ZMQ_FOUND)

endif (ZMQ_LIBRARY AND ZMQ_INCLUDE_DIR)
