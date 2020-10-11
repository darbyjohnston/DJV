# Find the OpenCV library.
#
# This module defines the following variables:
#
# * OpenCV_FOUND
#
# This module defines the following imported targets:
#
# * OpenCV::calib3d
# * OpenCV::core
# * OpenCV::dnn
# * OpenCV::features2d
# * OpenCV::flann
# * OpenCV::gapi
# * OpenCV::imgproc
# * OpenCV::ml
# * OpenCV::objdetect
# * OpenCV::photo
# * OpenCV::stitching
#
# This module defines the following interfaces:
#
# * OpenCV

find_path(OpenCV_INCLUDE_DIR NAMES opencv2/cvconfig.h PATH_SUFFIXES opencv4)

if(WIN32)
    set(OpenCV_VERSION 440)
    set(OpenCV_LIBRARY_SUFFIXES x64/vc16/staticlib)
    if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
        set(OpenCV_LIBRARY_SUFFIX d)
    endif()
endif()
find_library(OpenCV_LIBCALIB3D
    NAMES opencv_calib3d${OpenCV_VERSION}${OpenCV_LIBRARY_SUFFIX}
    PATH_SUFFIXES ${OpenCV_LIBRARY_SUFFIXES})
find_library(OpenCV_LIBCORE
    NAMES opencv_core${OpenCV_VERSION}${OpenCV_LIBRARY_SUFFIX}
    PATH_SUFFIXES ${OpenCV_LIBRARY_SUFFIXES})
find_library(OpenCV_LIBDNN
    NAMES opencv_dnn${OpenCV_VERSION}${OpenCV_LIBRARY_SUFFIX}
    PATH_SUFFIXES ${OpenCV_LIBRARY_SUFFIXES})
find_library(OpenCV_LIBFEATURES2D
    NAMES opencv_features2d${OpenCV_VERSION}${OpenCV_LIBRARY_SUFFIX}
    PATH_SUFFIXES ${OpenCV_LIBRARY_SUFFIXES})
find_library(OpenCV_LIBFLANN
    NAMES opencv_flann${OpenCV_VERSION}${OpenCV_LIBRARY_SUFFIX}
    PATH_SUFFIXES ${OpenCV_LIBRARY_SUFFIXES})
find_library(OpenCV_LIBGAPI
    NAMES opencv_gapi${OpenCV_VERSION}${OpenCV_LIBRARY_SUFFIX}
    PATH_SUFFIXES ${OpenCV_LIBRARY_SUFFIXES})
find_library(OpenCV_LIBIMGPROC
    NAMES opencv_imgproc${OpenCV_VERSION}${OpenCV_LIBRARY_SUFFIX}
    PATH_SUFFIXES ${OpenCV_LIBRARY_SUFFIXES})
find_library(OpenCV_LIBML
    NAMES opencv_ml${OpenCV_VERSION}${OpenCV_LIBRARY_SUFFIX}
    PATH_SUFFIXES ${OpenCV_LIBRARY_SUFFIXES})
find_library(OpenCV_LIBOBJDETECT
    NAMES opencv_objdetect${OpenCV_VERSION}${OpenCV_LIBRARY_SUFFIX}
    PATH_SUFFIXES ${OpenCV_LIBRARY_SUFFIXES})
find_library(OpenCV_LIBPHOTO
    NAMES opencv_photo${OpenCV_VERSION}${OpenCV_LIBRARY_SUFFIX}
    PATH_SUFFIXES ${OpenCV_LIBRARY_SUFFIXES})
find_library(OpenCV_LIBSTITCHING
    NAMES opencv_stitching${OpenCV_VERSION}${OpenCV_LIBRARY_SUFFIX}
    PATH_SUFFIXES ${OpenCV_LIBRARY_SUFFIXES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    OpenCV
    REQUIRED_VARS
        OpenCV_INCLUDE_DIR
        OpenCV_LIBCALIB3D
        OpenCV_LIBCORE
        OpenCV_LIBDNN
        OpenCV_LIBFEATURES2D
        OpenCV_LIBFLANN
        OpenCV_LIBGAPI
        OpenCV_LIBIMGPROC
        OpenCV_LIBML
        OpenCV_LIBOBJDETECT
        OpenCV_LIBPHOTO
        OpenCV_LIBSTITCHING)
mark_as_advanced(
    OpenCV_INCLUDE_DIR
    OpenCV_LIBCALIB3D
    OpenCV_LIBCORE
    OpenCV_LIBDNN
    OpenCV_LIBFEATURES2D
    OpenCV_LIBFLANN
    OpenCV_LIBGAPI
    OpenCV_LIBIMGPROC
    OpenCV_LIBML
    OpenCV_LIBOBJDETECT
    OpenCV_LIBPHOTO
    OpenCV_LIBSTITCHING)

if(OpenCV_FOUND AND NOT TARGET OpenCV::core)
    add_library(OpenCV::core UNKNOWN IMPORTED)
    find_package(ZLIB REQUIRED)
    set_target_properties(OpenCV::core PROPERTIES
        IMPORTED_LOCATION "${OpenCV_LIBCORE}"
        INTERFACE_COMPILE_DEFINITIONS OpenCV_FOUND
        INTERFACE_INCLUDE_DIRECTORIES "${OpenCV_INCLUDE_DIR}")
    if(WIN32)
	    set_property(TARGET OpenCV::core PROPERTY INTERFACE_LINK_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/x64/vc16/staticlib)
        if(CMAKE_BUILD_TYPE MATCHES "^Debug$")
    	    set_property(TARGET OpenCV::core APPEND PROPERTY INTERFACE_LINK_LIBRARIES "ade;ippiwd;ippicvmt;ittnotifyd;libprotobufd;quircd")
        else()
            set_property(TARGET OpenCV::core APPEND PROPERTY INTERFACE_LINK_LIBRARIES "ade;ippiw;ippicvmt;ittnotify;libprotobuf;quirc")
        endif()
    else()
	    set_property(TARGET OpenCV::core PROPERTY INTERFACE_LINK_DIRECTORIES ${CMAKE_INSTALL_PREFIX}/lib/opencv4/3rdparty)
        set_property(TARGET OpenCV::core APPEND PROPERTY INTERFACE_LINK_LIBRARIES "ade;ippiw;ippicv;ittnotify;libprotobuf;quirc")
    endif()
    set_property(TARGET OpenCV::core APPEND PROPERTY INTERFACE_LINK_LIBRARIES ZLIB)
endif()
if(OpenCV_FOUND AND NOT TARGET OpenCV::calib3d)
    add_library(OpenCV::calib3d UNKNOWN IMPORTED)
    set_target_properties(OpenCV::calib3d PROPERTIES
        IMPORTED_LOCATION "${OpenCV_LIBCALIB3D}"
        INTERFACE_LINK_LIBRARIES OpenCV::core)
endif()
if(OpenCV_FOUND AND NOT TARGET OpenCV::dnn)
    add_library(OpenCV::dnn UNKNOWN IMPORTED)
    set_target_properties(OpenCV::dnn PROPERTIES
        IMPORTED_LOCATION "${OpenCV_LIBDNN}"
        INTERFACE_LINK_LIBRARIES OpenCV::core)
endif()
if(OpenCV_FOUND AND NOT TARGET OpenCV::features2d)
    add_library(OpenCV::features2d UNKNOWN IMPORTED)
    set_target_properties(OpenCV::features2d PROPERTIES
        IMPORTED_LOCATION "${OpenCV_LIBFEATURES2D}"
        INTERFACE_LINK_LIBRARIES OpenCV::core)
endif()
if(OpenCV_FOUND AND NOT TARGET OpenCV::flann)
    add_library(OpenCV::flann UNKNOWN IMPORTED)
    set_target_properties(OpenCV::flann PROPERTIES
        IMPORTED_LOCATION "${OpenCV_LIBFLANN}"
        INTERFACE_LINK_LIBRARIES OpenCV::core)
endif()
if(OpenCV_FOUND AND NOT TARGET OpenCV::gapi)
    add_library(OpenCV::gapi UNKNOWN IMPORTED)
    set_target_properties(OpenCV::gapi PROPERTIES
        IMPORTED_LOCATION "${OpenCV_LIBGAPI}"
        INTERFACE_LINK_LIBRARIES OpenCV::core)
endif()
if(OpenCV_FOUND AND NOT TARGET OpenCV::imgproc)
    add_library(OpenCV::imgproc UNKNOWN IMPORTED)
    set_target_properties(OpenCV::imgproc PROPERTIES
        IMPORTED_LOCATION "${OpenCV_LIBIMGPROC}"
        INTERFACE_LINK_LIBRARIES OpenCV::core)
endif()
if(OpenCV_FOUND AND NOT TARGET OpenCV::ml)
    add_library(OpenCV::ml UNKNOWN IMPORTED)
    set_target_properties(OpenCV::ml PROPERTIES
        IMPORTED_LOCATION "${OpenCV_LIBML}"
        INTERFACE_LINK_LIBRARIES OpenCV::core)
endif()
if(OpenCV_FOUND AND NOT TARGET OpenCV::objdetect)
    add_library(OpenCV::objdetect UNKNOWN IMPORTED)
    set_target_properties(OpenCV::objdetect PROPERTIES
        IMPORTED_LOCATION "${OpenCV_LIBOBJDETECT}"
        INTERFACE_LINK_LIBRARIES OpenCV::core)
endif()
if(OpenCV_FOUND AND NOT TARGET OpenCV::photo)
    add_library(OpenCV::photo UNKNOWN IMPORTED)
    set_target_properties(OpenCV::photo PROPERTIES
        IMPORTED_LOCATION "${OpenCV_LIBPHOTO}"
        INTERFACE_LINK_LIBRARIES OpenCV::core)
endif()
if(OpenCV_FOUND AND NOT TARGET OpenCV::stitching)
    add_library(OpenCV::stitching UNKNOWN IMPORTED)
    set_target_properties(OpenCV::stitching PROPERTIES
        IMPORTED_LOCATION "${OpenCV_LIBSTITCHING}"
        INTERFACE_LINK_LIBRARIES OpenCV::core)
endif()
if(OpenCV_FOUND AND NOT TARGET OpenCV)
    add_library(OpenCV INTERFACE)
    target_link_libraries(OpenCV INTERFACE OpenCV::core)
    target_link_libraries(OpenCV INTERFACE OpenCV::calib3d)
    target_link_libraries(OpenCV INTERFACE OpenCV::dnn)
    target_link_libraries(OpenCV INTERFACE OpenCV::features2d)
    target_link_libraries(OpenCV INTERFACE OpenCV::flann)
    target_link_libraries(OpenCV INTERFACE OpenCV::gapi)
    target_link_libraries(OpenCV INTERFACE OpenCV::imgproc)
    target_link_libraries(OpenCV INTERFACE OpenCV::ml)
    target_link_libraries(OpenCV INTERFACE OpenCV::objdetect)
    target_link_libraries(OpenCV INTERFACE OpenCV::photo)
    target_link_libraries(OpenCV INTERFACE OpenCV::stitching)
endif()

