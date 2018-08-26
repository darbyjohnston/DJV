if (WIN32)

    find_package(FFmpeg)

    #message("FFMPEG_FOUND = " ${FFMPEG_FOUND})
    #message("FFMPEG_INCLUDE_DIRS = " ${FFMPEG_INCLUDE_DIRS})
    #message("FFMPEG_LIBRARIES = " ${FFMPEG_LIBRARIES})

    if (FFMPEG_FOUND)

        set(djvImageIoPlugins ${djvImageIoPlugins} djvFFmpegPlugin)
    
        if (djvPackageThirdParty)
        
			install(
				FILES
				$ENV{CMAKE_PREFIX_PATH}/bin/avcodec-58.dll
				$ENV{CMAKE_PREFIX_PATH}/bin/avdevice-58.dll
				$ENV{CMAKE_PREFIX_PATH}/bin/avfilter-7.dll
				$ENV{CMAKE_PREFIX_PATH}/bin/avformat-58.dll
				$ENV{CMAKE_PREFIX_PATH}/bin/avutil-56.dll
				$ENV{CMAKE_PREFIX_PATH}/bin/swresample-3.dll
				$ENV{CMAKE_PREFIX_PATH}/bin/swscale-5.dll
				DESTINATION bin)
				
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
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.58.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.58.18.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.58.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.58.3.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.7.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.7.16.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.58.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.58.12.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.56.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.56.14.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.3.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.3.1.100.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.5.dylib
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.5.1.100.dylib
                    DESTINATION lib)
            
            elseif (UNIX)
            
                install(
                    FILES
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so.58
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavcodec.so.58.18.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so.58
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavdevice.so.58.3.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so.7
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavfilter.so.7.16.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so.58
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavformat.so.58.12.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so.56
                    $ENV{CMAKE_PREFIX_PATH}/lib/libavutil.so.56.14.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so.3
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswresample.so.3.1.100
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so.5
                    $ENV{CMAKE_PREFIX_PATH}/lib/libswscale.so.5.1.100
                    DESTINATION lib)

            endif (APPLE)

        endif (djvPackageThirdParty)

    endif (FFMPEG_FOUND)
    
endif (WIN32)
