if (WIN32)

    find_package(FFmpeg)

    #message("FFMPEG_FOUND = " ${FFMPEG_FOUND})
    #message("FFMPEG_INCLUDE_DIRS = " ${FFMPEG_INCLUDE_DIRS})
    #message("FFMPEG_LIBRARIES = " ${FFMPEG_LIBRARIES})

    if (FFMPEG_FOUND)

        set(djvImageIoPlugins ${djvImageIoPlugins} djvFFmpegPlugin)
    
        if (djvPackageThirdParty)
        
        endif (djvPackageThirdParty)

    endif (FFMPEG_FOUND)

else (WIN32)

    find_package(FFmpeg)

    #message("FFMPEG_FOUND = " ${FFMPEG_FOUND})
    #message("FFMPEG_INCLUDE_DIRS = " ${FFMPEG_INCLUDE_DIRS})
    #message("FFMPEG_LIBRARIES = " ${FFMPEG_LIBRARIES})

    if (FFMPEG_FOUND)

        set(djvImageIoPlugins ${djvImageIoPlugins} djvFFmpegPlugin)
    
        if (djvPackageThirdParty)
        
            if (APPLE)
            
                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.56.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.56.26.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.56.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.56.4.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.5.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.5.11.102.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.56.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.56.25.101.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.54.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.54.20.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.1.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.1.1.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.3.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.3.1.101.dylib
                    DESTINATION lib)
            
            elseif (UNIX)
            
                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so.56
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so.56.26.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so.56
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so.56.4.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so.5
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so.5.11.102
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so.56
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so.56.25.101
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so.54
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so.54.20.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so.1
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so.1.1.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so.3
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so.3.1.101
                    DESTINATION lib)

            endif (APPLE)

        endif (djvPackageThirdParty)

    endif (FFMPEG_FOUND)
    
endif (WIN32)
