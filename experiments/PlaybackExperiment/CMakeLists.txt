set(header
    Application.h
    FFmpeg.h
    IO.h
    IOInline.h
    ImageWidget.h
    Media.h
    TimelineWidget.h
    Window.h)
set(source
    Application.cpp
    FFmpeg.cpp
    IO.cpp
    ImageWidget.cpp
    Media.cpp
    TimelineWidget.cpp
    Window.cpp
    main.cpp)

add_executable(PlaybackExperiment ${header} ${source})
target_link_libraries(PlaybackExperiment djvDesktopApp FFmpeg)
set_target_properties(
    PlaybackExperiment
    PROPERTIES
    FOLDER experiments
    CXX_STANDARD 11)
