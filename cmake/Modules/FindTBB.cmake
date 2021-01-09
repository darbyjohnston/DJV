# Find the TBB library.
#
# This module defines the following variables:
#
# * TBB_FOUND
# * TBB_INCLUDE_DIRS
# * TBB_LIBRARIES
#
# This module defines the following imported targets:
#
# * TBB::tbb
# * TBB::tbbmalloc
# * TBB::tbbmalloc_proxy
#
# This module defines the following interfaces:
#
# * TBB

find_path(TBB_INCLUDE_DIR NAMES tbb/tbb.h)
set(TBB_INCLUDE_DIRS ${TBB_INCLUDE_DIR})

find_library(TBB_LIBRARY NAMES tbb)
find_library(TBB_tbbmalloc_LIBRARY NAMES tbbmalloc)
find_library(TBB_tbbmalloc_proxy_LIBRARY NAMES tbbmalloc_proxy)
set(TBB_LIBRARIES ${TBB_LIBRARY} ${TBB_tbbmalloc_LIBRARY} ${TBB_tbbmalloc_proxy_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    TBB
    REQUIRED_VARS TBB_INCLUDE_DIR TBB_LIBRARY)
mark_as_advanced(
    TBB_INCLUDE_DIR
    TBB_LIBRARY
    TBB_tbbmalloc_LIBRARY
    TBB_tbbmalloc_proxy_LIBRARY)

set(TBB_COMPILE_DEFINITIONS TBB_FOUND)
list(APPEND TBB_COMPILE_DEFINITIONS __TBB_NO_IMPLICIT_LINKAGE)
list(APPEND TBB_COMPILE_DEFINITIONS __TBBMALLOC_NO_IMPLICIT_LINKAGE)

if(TBB_FOUND AND NOT TARGET TBB::tbb)
    add_library(TBB::tbb UNKNOWN IMPORTED)
    set_target_properties(TBB::tbb PROPERTIES
        IMPORTED_LOCATION "${TBB_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${TBB_COMPILE_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${TBB_INCLUDE_DIR}")
endif()
if(TBB_FOUND AND NOT TARGET TBB::tbbmalloc)
    add_library(TBB::tbbmalloc UNKNOWN IMPORTED)
    set_target_properties(TBB::tbbmalloc PROPERTIES
        IMPORTED_LOCATION "${TBB_tbbmalloc_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${TBB_COMPILE_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${TBB_INCLUDE_DIR}")
endif()
if(TBB_FOUND AND NOT TARGET TBB::tbbmalloc_proxy)
    add_library(TBB::tbbmalloc_proxy UNKNOWN IMPORTED)
    set_target_properties(TBB::tbbmalloc_proxy PROPERTIES
        IMPORTED_LOCATION "${TBB_tbbmalloc_proxy_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS "${TBB_COMPILE_DEFINITIONS}"
        INTERFACE_INCLUDE_DIRECTORIES "${TBB_INCLUDE_DIR}")
endif()
if(TBB_FOUND AND NOT TARGET TBB)
    add_library(TBB INTERFACE)
    target_link_libraries(TBB INTERFACE TBB::tbb)
    target_link_libraries(TBB INTERFACE TBB::tbbmalloc)
    target_link_libraries(TBB INTERFACE TBB::tbbmalloc_proxy)
endif()
