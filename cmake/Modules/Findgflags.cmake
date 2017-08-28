# - Find gflags 
# Find the gflags compression library and includes
#
#  GFLAGS_INCLUDE_DIR - where to find gflags/gflags.h, etc.
#  GFLAGS_LIBRARIES   - List of libraries when using gflags.
#  GFLAGS_FOUND       - True if gflags found.

find_path(GFLAGS_INCLUDE_DIR gflags/gflags.h NO_DEFAULT_PATH PATHS
  ${SQPKV_DEPENDENCY_INCLUDE_DIR}
  /usr/include
  /opt/local/include
  /usr/local/include
)

set(GFLAGS_NAMES ${GFLAGS_NAMES} gflags)
find_library(GFLAGS_LIBRARY NAMES ${GFLAGS_NAMES} NO_DEFAULT_PATH PATHS
  ${SQPKV_DEPENDENCY_LIB_DIR}
  /usr/local/lib
  /opt/local/lib
  /usr/lib
)

if (GFLAGS_INCLUDE_DIR AND GFLAGS_LIBRARY)
  set(GFLAGS_FOUND TRUE)
  set( GFLAGS_LIBRARIES ${GFLAGS_LIBRARY} )
else ()
  set(GFLAGS_FOUND FALSE)
  set( GFLAGS_LIBRARIES )
endif ()

if (GFLAGS_FOUND)
  message(STATUS "Found gflags: ${GFLAGS_LIBRARY}")
else ()
  message(STATUS "Not Found gflags: ${GFLAGS_LIBRARY}")
  if (GFLAGS_FIND_REQUIRED)
    message(STATUS "Looked for gflags libraries named ${GFLAGS_NAMES}.")
    message(FATAL_ERROR "Could NOT find gflags library")
  endif ()
endif ()

mark_as_advanced(
  GFLAGS_LIBRARY
  GFLAGS_INCLUDE_DIR
)
