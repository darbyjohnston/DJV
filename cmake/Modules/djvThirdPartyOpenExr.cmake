find_package(OpenEXR)

#message("OPENEXR_FOUND = " ${OPENEXR_FOUND})
#message("OPENEXR_INCLUDE_DIR = " ${OPENEXR_INCLUDE_DIR})
#message("OPENEXR_LIBRARIES = " ${OPENEXR_LIBRARIES})

if (OPENEXR_FOUND)

    set(djvImageIoPlugins ${djvImageIoPlugins} djvOpenExrPlugin)
    
    if (djvPackageThirdParty)
    
        if (WIN32)

			install(
				FILES
				$ENV{CMAKE_PREFIX_PATH}/bin/IlmImf.dll
				DESTINATION bin)

        elseif (APPLE)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImf.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImf-2_2.22.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImfUtil.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImfUtil-2_2.22.dylib
                DESTINATION lib)

        elseif (UNIX)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImf.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImf-2_2.so.22
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImf-2_2.so.22.0.0
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImfUtil.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImfUtil-2_2.so.22
                $ENV{CMAKE_PREFIX_PATH}/lib/libIlmImfUtil-2_2.so.22.0.0
                DESTINATION lib)

        endif (WIN32)

    endif (djvPackageThirdParty)
            
endif (OPENEXR_FOUND)

