if (WIN32)

else (WIN32)

    find_package(FFmpeg)

    #message("FFMPEG_FOUND = " ${FFMPEG_FOUND})
    #message("FFMPEG_INCLUDE_DIRS = " ${FFMPEG_INCLUDE_DIRS})
    #message("FFMPEG_LIBRARIES = " ${FFMPEG_LIBRARIES})

    if (FFMPEG_FOUND)
    
        if (djvPackageThirdParty)
        
            if (WIN32)

            elseif (APPLE)
            
                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.54.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.54.92.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.54.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.54.3.103.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.3.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.3.42.103.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.54.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.54.63.104.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.52.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.52.18.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.0.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.0.17.102.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.2.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.2.2.100.dylib
                    DESTINATION lib)
            
            elseif (UNIX)
            
                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so.54
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so.54.92.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so.54
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so.54.3.103
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so.3
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so.3.42.103
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so.54
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so.54.63.104
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so.52
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so.52.18.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so.0
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so.0.17.102
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so.2
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so.2.2.100
                    DESTINATION lib)

            endif (WIN32)

        endif (djvPackageThirdParty)

    endif (FFMPEG_FOUND)
    
endif (WIN32)

if (WIN32)

else (WIN32)

    find_package(Libquicktime)

    #message("LIBQUICKTIME_FOUND = " ${LIBQUICKTIME_FOUND})
    #message("LIBQUICKTIME_INCLUDE_DIRS = " ${LIBQUICKTIME_INCLUDE_DIRS})
    #message("LIBQUICKTIME_LIBRARIES = " ${LIBQUICKTIME_LIBRARIES})

    if (LIBQUICKTIME_FOUND)

        set(djvImageIoPlugins ${djvImageIoPlugins} djvLibquicktimePlugin)

        if (djvPackageThirdParty)
        
            if (WIN32)

            elseif (APPLE)

                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime.0.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libintl.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libintl.8.dylib
                    DESTINATION lib)

                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_audiocodec.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_ffmpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_mjpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_rtjpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_videocodec.so
                    DESTINATION lib/libquicktime)

            elseif (UNIX)
            
                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime.so.0
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime.so.0.0.0
                    DESTINATION lib)

                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_audiocodec.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_ffmpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_mjpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_png.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_rtjpeg.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libquicktime/lqt_videocodec.so
                    DESTINATION lib/libquicktime)

            endif (WIN32)

        endif (djvPackageThirdParty)
    
    endif (LIBQUICKTIME_FOUND)

endif (WIN32)

