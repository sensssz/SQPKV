# - Find LZ4 
# Find the lz4 compression library and includes
#
#  LZ4_INCLUDE_DIR - where to find lz4.h, etc.
#  LZ4_LIBRARIES   - List of libraries when using lz4.
#  LZ4_FOUND       - True if lz4 found.

find_path(LZ4_INCLUDE_DIR lz4.h NO_DEFAULT_PATH PATHS
  ${SQPKV_DEPENDENCY_INCLUDE_DIR}
  /usr/include
  /opt/local/include
  /usr/local/include
)

set(LZ4_NAMES ${LZ4_NAMES} lz4)
find_library(LZ4_LIBRARY NAMES ${LZ4_NAMES} NO_DEFAULT_PATH PATHS
  ${SQPKV_DEPENDENCY_LIB_DIR}
  /usr/local/lib
  /opt/local/lib
  /usr/lib
)

if (LZ4_INCLUDE_DIR AND LZ4_LIBRARY)
  set(LZ4_FOUND TRUE)
  set( LZ4_LIBRARIES ${LZ4_LIBRARY} )
else ()
  set(LZ4_FOUND FALSE)
  set( LZ4_LIBRARIES )
endif ()

if (LZ4_FOUND)
  message(STATUS "Found LZ4: ${LZ4_LIBRARY}")
else ()
  message(STATUS "Not Found LZ4: ${LZ4_LIBRARY}")
  if (LZ4_FIND_REQUIRED)
    message(STATUS "Looked for LZ4 libraries named ${LZ4_NAMES}.")
    message(FATAL_ERROR "Could NOT find LZ4 library")
  endif ()
endif ()

mark_as_advanced(
  LZ4_LIBRARY
  LZ4_INCLUDE_DIR
)
