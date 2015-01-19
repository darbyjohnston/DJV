if (WIN32)

    if (djvBuildArch EQUAL 32)

        find_package(QuickTime)

        #message("QUICKTIME_FOUND = " ${QUICKTIME_FOUND})
        #message("QUICKTIME_INCLUDE_DIRS = " ${QUICKTIME_INCLUDE_DIRS})
        #message("QUICKTIME_LIBRARIES = " ${QUICKTIME_LIBRARIES})
    
        if (QUICKTIME_FOUND)

            set(djvImageIoPlugins ${djvImageIoPlugins} djvQuickTimePlugin)

        endif (QUICKTIME_FOUND)

    endif (djvBuildArch EQUAL 32)

endif (WIN32)

