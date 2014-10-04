# Find the QuickTime library.
#
# This module defines the following variables:
#
# * QUICKTIME_INCLUDE_DIRS - Include directories for QuickTime.
# * QUICKTIME_LIBRARIES    - Libraries to link against QuickTime.
# * QUICKTIME_FOUND        - True if QuickTime has been found and can be used.

if (WIN32)

    if (EXISTS "C:\\Program Files (x86)\\QuickTime SDK\\")

        set(QUICKTIME_INCLUDE_DIR "C:\\Program Files (x86)\\QuickTime SDK\\CIncludes")

        set(QUICKTIME_LIBRARY
            "C:\\Program Files (x86)\\QuickTime SDK\\Libraries\\QTMLClient.lib")
    
    endif (EXISTS "C:\\Program Files (x86)\\QuickTime SDK\\")

    set(QUICKTIME_LIBRARIES ${QUICKTIME_LIBRARY})

    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args(
        QUICKTIME
        REQUIRED_VARS
            QUICKTIME_INCLUDE_DIR
            QUICKTIME_LIBRARY)

    mark_as_advanced(QUICKTIME_INCLUDE_DIR QUICKTIME_LIBRARY)

elseif (APPLE)

    if (EXISTS /System/Library/Frameworks/QuickTime.framework/)
    
        find_library(QUICKTIME_LIBRARY QuickTime)
        find_library(QUICKTIME_CORE_AUDIO_LIBRARY CoreAudio)
        find_library(QUICKTIME_CARBON_LIBRARY Carbon)

    endif (EXISTS /System/Library/Frameworks/QuickTime.framework/)

    set(QUICKTIME_LIBRARIES
        ${QUICKTIME_LIBRARY}
        ${QUICKTIME_CORE_AUDIO_LIBRARY}
        ${QUICKTIME_CARBON_LIBRARY})
    
    include(FindPackageHandleStandardArgs)

    find_package_handle_standard_args(
        QUICKTIME
        REQUIRED_VARS
            QUICKTIME_LIBRARY
            QUICKTIME_CORE_AUDIO_LIBRARY
            QUICKTIME_CARBON_LIBRARY)

    mark_as_advanced(
        QUICKTIME_INCLUDE_DIR
        QUICKTIME_LIBRARY
        QUICKTIME_CORE_AUDIO_LIBRARY
        QUICKTIME_CARBON_LIBRARY)

endif (WIN32)
