# Find the OpenEXR library.
#
# This module defines the following variables:
#
# * OPENEXR_FOUND
# * OPENEXR_INCLUDE_DIRS
# * OPENEXR_LIBRARIES
#
# This module defines the following imported targets:
#
# * OpenEXR::IlmImf
#
# This module defines the following interfaces:
#
# * OpenEXR

find_package(ZLIB REQUIRED)
find_package(IlmBase REQUIRED)

set(OPENEXR_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/OpenEXR)
set(OPENEXR_INCLUDE_DIRS
    ${OPENEXR_INCLUDE_DIR}
    ${ILMBASE_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS})

if(WIN32)
    if(OPENEXR_SHARED_LIBS)
        set(OPENEXR_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/IlmImf.lib)
    else()
        set(OPENEXR_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/IlmImf_s.lib)
    endif()
elseif(APPLE)
    if(OPENEXR_SHARED_LIBS)
        set(OPENEXR_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmImf.dylib)
    else()
        set(OPENEXR_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmImf.a)
    endif()
else()
    if(OPENEXR_SHARED_LIBS)
        set(OPENEXR_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmImf.so)
    else()
        set(OPENEXR_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libIlmImf.a)
    endif()
endif()
set(OPENEXR_LIBRARIES
    ${OPENEXR_LIBRARY}
    ${ILMBASE_LIBRARIES}
    ${ZLIB_LIBRARIES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OPENEXR
    REQUIRED_VARS OPENEXR_INCLUDE_DIR OPENEXR_LIBRARY)
mark_as_advanced(OPENEXR_INCLUDE_DIR OPENEXR_LIBRARY)

if(OPENEXR_FOUND AND NOT TARGET OpenEXR::IlmImf)
    add_library(OpenEXR::IlmImf UNKNOWN IMPORTED)
    set_target_properties(OpenEXR::IlmImf PROPERTIES
        IMPORTED_LOCATION "${OPENEXR_LIBRARY}"
        IMPORTED_LINK_INTERFACE_LIBRARIES ZLIB 
        INTERFACE_INCLUDE_DIRECTORIES "${OPENEXR_INCLUDE_DIRS}")
endif()
if(OPENEXR_FOUND AND NOT TARGET OpenEXR)
    add_library(OpenEXR INTERFACE)
    target_link_libraries(OpenEXR INTERFACE OpenEXR::IlmImf)
endif()


