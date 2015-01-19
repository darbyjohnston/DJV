if (WIN32)

    add_definitions(-DPNG_USE_DLL)

endif (WIN32)

find_package(PNG)

#message("PNG_FOUND = " ${PNG_FOUND})
#message("PNG_INCLUDE_DIR = " ${PNG_INCLUDE_DIR})
#message("PNG_LIBRARIES = " ${PNG_LIBRARIES})

if (PNG_FOUND)

    set(djvImageIoPlugins ${djvImageIoPlugins} djvPngPlugin)
    
    if (djvPackageThirdParty)
    
        if (WIN32)

			install(
				FILES
				$ENV{CMAKE_PREFIX_PATH}/bin/png.dll
				DESTINATION bin)

        elseif (APPLE)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libpng16.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libpng16.16.dylib
                DESTINATION lib)

        elseif (UNIX)
        
            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libpng16.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libpng16.so.16
                $ENV{CMAKE_PREFIX_PATH}/lib/libpng16.so.16.8.0
                DESTINATION lib)

        endif (WIN32)

    endif (djvPackageThirdParty)
    
endif (PNG_FOUND)

