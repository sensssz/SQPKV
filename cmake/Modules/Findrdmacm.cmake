# - Find rdmacm
# Find the rdma/verbs library and includes
#
#  RDMA_INCLUDE_DIR - where to find rdma/rdma_cma.h, etc.
#  RDMA_LIBRARIES   - List of libraries when using rdmacm.
#  RDMA_FOUND       - True if rdma found.

find_path(RDMA_INCLUDE_DIR rdma.h NO_DEFAULT_PATH PATHS
  ${SQPKV_DEPENDENCY_INCLUDE_DIR}
  /usr/include
  /opt/local/include
  /usr/local/include
)

set(RDMA_NAMES ${RDMA_NAMES} rdmacm)
find_library(RDMA_LIBRARY NAMES ${RDMA_NAMES} NO_DEFAULT_PATH PATHS
  ${SQPKV_DEPENDENCY_LIB_DIR}
  /usr/local/lib
  /opt/local/lib
  /usr/lib
  /usr/lib64
)

set(IBVERBS_NAMES ${IBVERBS_NAMES} ibverbs)
find_library(VERBS_LIBRARY NAMES ${IBVERBS_NAMES} NO_DEFAULT_PATH PATHS
  ${SQPKV_DEPENDENCY_LIB_DIR}
  /usr/local/lib
  /opt/local/lib
  /usr/lib
  /usr/lib64
)

if (RDMA_INCLUDE_DIR AND RDMA_LIBRARY AND VERBS_LIBRARY)
  set(RDMA_FOUND TRUE)
  set( RDMA_LIBRARIES ${RDMA_LIBRARY} )
else ()
  set(RDMA_FOUND FALSE)
  set( RDMA_LIBRARIES )
endif ()

if (RDMA_FOUND)
  message(STATUS "Found rdma: ${RDMA_LIBRARY}")
else ()
  message(STATUS "Not Found rdma: ${RDMA_LIBRARY}")
  if (RDMA_FIND_REQUIRED)
    message(STATUS "Looked for rdma libraries named ${RDMA_NAMES}.")
    message(FATAL_ERROR "Could NOT find rdma library")
  endif ()
endif ()

mark_as_advanced(
  RDMA_LIBRARY
  VERBS_LIBRARY
  RDMA_INCLUDE_DIR
)
