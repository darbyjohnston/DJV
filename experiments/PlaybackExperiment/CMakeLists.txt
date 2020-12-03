set(header
    Application.h
    DrawerWidget.h
    Enum.h
    FFmpeg.h
    FFmpegFunc.h
    IO.h
    IOCache.h
    IOInline.h
    ImageWidget.h
    MainWindow.h
    Media.h
    TimelineWidget.h)
set(source
    Application.cpp
    DrawerWidget.cpp
    FFmpeg.cpp
    FFmpegFunc.cpp
    IO.cpp
    IOCache.cpp
    ImageWidget.cpp
    MainWindow.cpp
    Media.cpp
    TimelineWidget.cpp
    main.cpp)

add_executable(PlaybackExperiment ${header} ${source})
target_link_libraries(PlaybackExperiment djvDesktopApp djvUIComponents FFmpeg)
set_target_properties(
    PlaybackExperiment
    PROPERTIES
    FOLDER experiments
    CXX_STANDARD 11)
