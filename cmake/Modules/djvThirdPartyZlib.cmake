if (WIN32)

    add_definitions(-DZLIB_DLL)

endif (WIN32)

find_package(ZLIB)

#message("ZLIB_FOUND = " ${ZLIB_FOUND})
#message("ZLIB_INCLUDE_DIRS = " ${ZLIB_INCLUDE_DIRS})
#message("ZLIB_LIBRARIES = " ${ZLIB_LIBRARIES})

if (ZLIB_FOUND)

    if (djvPackageThirdParty)
    
        if (WIN32)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/bin/zlib.dll
                DESTINATION bin)

        elseif (APPLE)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.1.dylib
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.1.2.8.dylib
                DESTINATION lib)

        elseif (UNIX)

            install(
                FILES
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.so
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.so.1
                $ENV{CMAKE_PREFIX_PATH}/lib/libz.so.1.2.8
                DESTINATION lib)

        endif (WIN32)

    endif (djvPackageThirdParty)

endif (ZLIB_FOUND)

