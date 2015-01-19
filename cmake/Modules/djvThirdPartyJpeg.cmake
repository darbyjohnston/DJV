find_package(JPEG)

#message("JPEG_FOUND = " ${JPEG_FOUND})
#message("JPEG_INCLUDE_DIR = " ${JPEG_INCLUDE_DIR})
#message("JPEG_LIBRARIES = " ${JPEG_LIBRARIES})

if (JPEG_FOUND)

    set(djvImageIoPlugins ${djvImageIoPlugins} djvJpegPlugin)
    
    if (djvPackageThirdParty)
    
        if (WIN32)

			#install(
			#	FILES
			#	$ENV{CMAKE_PREFIX_PATH}/bin/jpeg.dll
			#	DESTINATION bin)

        elseif (APPLE)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libjpeg.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libjpeg.9.dylib
                DESTINATION lib)
        
        elseif (UNIX)
        
            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libjpeg.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libjpeg.so.9
                $ENV{CMAKE_PREFIX_PATH}/lib/libjpeg.so.9.0.0
                DESTINATION lib)

        endif (WIN32)

    endif (djvPackageThirdParty)

endif (JPEG_FOUND)

