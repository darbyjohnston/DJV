# Find the libjpeg library.
#
# This module defines the following variables:
#
# * JPEG_FOUND
# * JPEG_INCLUDE_DIRS
# * JPEG_LIBRARIES
#
# This module defines the following imported targets:
#
# * JPEG::JPEG
#
# This module defines the following interfaces:
#
# * JPEG

find_path(JPEG_INCLUDE_DIR
    NAMES jpeglib.h)
set(JPEG_INCLUDE_DIRS ${JPEG_INCLUDE_DIR})

if(NOT djvThirdPartyBuild)
    find_library(JPEG_LIBRARY NAMES jpeg)
else()
    if(WIN32)
        set(JPEG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/jpeg.lib)
    elseif(APPLE)
        if(JPEG_SHARED_LIBS)
            set(JPEG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libjpeg.dylib)
            if(djvThirdPartyPackage)
                install(
                    FILES
                    ${JPEG_LIBRARY}
                    DESTINATION lib)
            endif()
        else()
            set(JPEG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libjpeg.a)
        endif()
    else()
        if(JPEG_SHARED_LIBS)
            set(JPEG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libjpeg.so)
            if(djvThirdPartyPackage)
                install(
                    FILES
                    ${JPEG_LIBRARY}
                    DESTINATION lib)
            endif()
        else()
            set(JPEG_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libjpeg.a)
        endif()
    endif()
endif()
set(JPEG_LIBRARIES ${JPEG_LIBRARY})

if(NOT JPEG_SHARED_LIBS)
    add_definitions(-DJPEG_STATIC)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    JPEG
    REQUIRED_VARS JPEG_INCLUDE_DIR JPEG_LIBRARY)
mark_as_advanced(JPEG_INCLUDE_DIR JPEG_LIBRARY)

if(JPEG_FOUND AND NOT TARGET JPEG::JPEG)
    add_library(JPEG::JPEG UNKNOWN IMPORTED)
    set_target_properties(JPEG::JPEG PROPERTIES
        IMPORTED_LOCATION "${JPEG_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${JPEG_INCLUDE_DIRS}"
        INTERFACE_COMPILE_DEFINITIONS JPEG_FOUND)
endif()
if(JPEG_FOUND AND NOT TARGET JPEG)
    add_library(JPEG INTERFACE)
    target_link_libraries(JPEG INTERFACE JPEG::JPEG)
endif()

