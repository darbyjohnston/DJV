# Find the FSeq library.
#
# This module defines the following variables:
#
# * FSeq_FOUND
#
# This module defines the following imported targets:
#
# * FSeq::FSeq
#
# This module defines the following interfaces:
#
# * FSeq
	
find_path(FSeq_INCLUDE_DIR NAMES fseq.h)

if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    find_library(FSeq_LIBRARY NAMES fseq)
else()
    find_library(FSeq_LIBRARY NAMES fseq)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    FSeq
    REQUIRED_VARS FSeq_INCLUDE_DIR FSeq_LIBRARY)
mark_as_advanced(FSeq_INCLUDE_DIR FSeq_LIBRARY)

if(FSeq_FOUND AND NOT TARGET FSeq::FSeq)
    add_library(FSeq::FSeq UNKNOWN IMPORTED)
    set_target_properties(FSeq::FSeq PROPERTIES
        IMPORTED_LOCATION "${FSeq_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FSeq_INCLUDE_DIR}")
endif()
if(FSeq_FOUND AND NOT TARGET FSeq)
    add_library(FSeq INTERFACE)
    target_link_libraries(FSeq INTERFACE FSeq::FSeq)
endif()
