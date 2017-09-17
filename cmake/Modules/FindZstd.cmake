# - Find Zstd 
# Find the Zstandard compression library and includes
#
#  ZSTD_INCLUDE_DIR - where to find zstd.h, etc.
#  ZSTD_LIBRARIES   - List of libraries when using zstd.
#  ZSTD_FOUND       - True if zstd found.

find_path(ZSTD_INCLUDE_DIR zstd.h)

set(ZSTD_NAMES ${ZSTD_NAMES} zstd)
find_library(ZSTD_LIBRARY NAMES ${ZSTD_NAMES})

if (ZSTD_INCLUDE_DIR AND ZSTD_LIBRARY)
  set(ZSTD_FOUND TRUE)
  set( ZSTD_LIBRARIES ${ZSTD_LIBRARY} )
else ()
  set(ZSTD_FOUND FALSE)
  set( ZSTD_LIBRARIES )
endif ()

if (ZSTD_FOUND)
  message(STATUS "Found Zstd: ${ZSTD_LIBRARY}")
else ()
  message(STATUS "Not Found Zstd: ${ZSTD_LIBRARY}")
  if (ZSTD_FIND_REQUIRED)
    message(STATUS "Looked for Zstd libraries named ${ZSTD_NAMES}.")
    message(FATAL_ERROR "Could NOT find Zstd library")
  endif ()
endif ()

mark_as_advanced(
  ZSTD_LIBRARY
  ZSTD_INCLUDE_DIR
)
