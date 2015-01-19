find_package(TIFF)

#message("TIFF_FOUND = " ${TIFF_FOUND})
#message("TIFF_INCLUDE_DIR = " ${TIFF_INCLUDE_DIR})
#message("TIFF_LIBRARIES = " ${TIFF_LIBRARIES})

if (TIFF_FOUND)

    set(djvImageIoPlugins ${djvImageIoPlugins} djvTiffPlugin)
    
    if (djvPackageThirdParty)
    
        if (WIN32)

			install(
				FILES
				$ENV{CMAKE_PREFIX_PATH}/bin/tiff.dll
				DESTINATION bin)

        elseif (APPLE)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libtiff.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libtiff.5.dylib
                DESTINATION lib)

        elseif (UNIX)
        
            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libtiff.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libtiff.so.5
                $ENV{CMAKE_PREFIX_PATH}/lib/libtiff.so.5.2.0
                DESTINATION lib)

        endif (WIN32)

    endif (djvPackageThirdParty)

endif (TIFF_FOUND)

