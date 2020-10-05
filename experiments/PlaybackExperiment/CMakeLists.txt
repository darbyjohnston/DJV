set(header
    Application.h
    Enum.h
    FFmpeg.h
    IO.h
    IOInline.h
    ImageWidget.h
    Media.h
    PlaybackWidget.h
    TimelineWidget.h
    Window.h)
set(source
    Application.cpp
    FFmpeg.cpp
    IO.cpp
    ImageWidget.cpp
    Media.cpp
    PlaybackWidget.cpp
    TimelineWidget.cpp
    Window.cpp
    main.cpp)

add_executable(PlaybackExperiment ${header} ${source})
target_link_libraries(PlaybackExperiment djvDesktopApp djvUIComponents FFmpeg)
set_target_properties(
    PlaybackExperiment
    PROPERTIES
    FOLDER experiments
    CXX_STANDARD 11)
