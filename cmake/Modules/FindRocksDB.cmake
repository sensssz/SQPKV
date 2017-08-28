# - Find RocksDB 
# Find the rocksdb compression library and includes
#
#  ROCKSDB_INCLUDE_DIR - where to find rocksdb/db.h, etc.
#  ROCKSDB_LIBRARIES   - List of libraries when using rocksdb.
#  ROCKSDB_FOUND       - True if rocksdb found.

find_path(ROCKSDB_INCLUDE_DIR rocksdb/db.h NO_DEFAULT_PATH PATHS
  ${SQPKV_DEPENDENCY_INCLUDE_DIR}
  /usr/include
  /opt/local/include
  /usr/local/include
)

set(ROCKSDB_NAMES ${ROCKSDB_NAMES} rocksdb)
find_library(ROCKSDB_LIBRARY NAMES ${ROCKSDB_NAMES} NO_DEFAULT_PATH PATHS
  ${SQPKV_DEPENDENCY_LIB_DIR}
  /usr/local/lib
  /opt/local/lib
  /usr/lib
)

if (ROCKSDB_INCLUDE_DIR AND ROCKSDB_LIBRARY)
  set(ROCKSDB_FOUND TRUE)
  set( ROCKSDB_LIBRARIES ${ROCKSDB_LIBRARY} )
else ()
  set(ROCKSDB_FOUND FALSE)
  set( ROCKSDB_LIBRARIES )
endif ()

if (ROCKSDB_FOUND)
  message(STATUS "Found RocksDB: ${ROCKSDB_LIBRARY}")
else ()
  message(STATUS "Not Found RocksDB: ${ROCKSDB_LIBRARY}")
  if (ROCKSDB_FIND_REQUIRED)
    message(STATUS "Looked for RocksDB libraries named ${ROCKSDB_NAMES}.")
    message(FATAL_ERROR "Could NOT find RocksDB library")
  endif ()
endif ()

mark_as_advanced(
  ROCKSDB_LIBRARY
  ROCKSDB_INCLUDE_DIR
)
