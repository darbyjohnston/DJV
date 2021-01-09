# Find the FSeq library.
#
# This module defines the following variables:
#
# * FSeq_FOUND
# * FSeq_INCLUDE_DIRS
# * FSeq_LIBRARIES
#
# This module defines the following imported targets:
#
# * FSeq::fseq
#
# This module defines the following interfaces:
#
# * FSeq
	
find_path(FSeq_INCLUDE_DIR NAMES fseq.h)
set(FSeq_INCLUDE_DIRS ${FSeq_INCLUDE_DIR})

find_library(FSeq_LIBRARY NAMES fseq)
set(FSeq_LIBRARIES ${FSeq_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    FSeq
    REQUIRED_VARS FSeq_INCLUDE_DIR FSeq_LIBRARY)
mark_as_advanced(FSeq_INCLUDE_DIR FSeq_LIBRARY)

if(FSeq_FOUND AND NOT TARGET FSeq::fseq)
    add_library(FSeq::fseq UNKNOWN IMPORTED)
    set_target_properties(FSeq::fseq PROPERTIES
        IMPORTED_LOCATION "${FSeq_LIBRARY}"
        INTERFACE_COMPILE_DEFINITIONS FSeq_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${FSeq_INCLUDE_DIR}")
endif()
if(FSeq_FOUND AND NOT TARGET FSeq)
    add_library(FSeq INTERFACE)
    target_link_libraries(FSeq INTERFACE FSeq::fseq)
endif()
