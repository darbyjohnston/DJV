set(header
    Application.h
    Context.h
    ImageView.h
    InfoWidget.h
    IO.h
    MainWindow.h
    PlaybackSystem.h
    TimelineSlider.h
    TimelineWidget.h
    Util.h)
set(mocHeader
    Application.h
    Context.h
    ImageView.h
    InfoWidget.h
    IO.h
    MainWindow.h
    PlaybackSystem.h
    TimelineSlider.h
    TimelineWidget.h
    Util.h)
set(source
    Application.cpp
    Context.cpp
    ImageView.cpp
    InfoWidget.cpp
    IO.cpp
    MainWindow.cpp
    PlaybackSystem.cpp
    TimelineSlider.cpp
    TimelineWidget.cpp
    Util.cpp
    main.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
add_executable(djvAudioExperiment2 ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvAudioExperiment2 djvUI)
set_target_properties(djvAudioExperiment2 PROPERTIES FOLDER experiments CXX_STANDARD 11)
