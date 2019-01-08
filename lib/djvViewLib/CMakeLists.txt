set(header
    Application.h
    #ColorPickerTool.h
    #Context.h
    #ContextInline.h
    #DebugTool.h
    Enum.h
    FileSystem.h
    #HelpSystem.h
    #HistogramTool.h
    IToolWidget.h
    IViewSystem.h
    #ImageSystem.h
    ImageView.h
    #InfoTool.h
    #MagnifyTool.h
    MDIWindow.h
    MainWindow.h
    Media.h
    MediaSession.h
    #PlaybackSystem.h
    PlaybackWidget.h
    TimelineSlider.h
    #TimelineWidget.h
    #ToolSystem.h
    ViewLib.h
    #ViewSystem.h
    WindowSystem.h
    )
set(source
    Application.cpp
    #ColorPickerTool.cpp
    #Context.cpp
    #DebugTool.cpp
    Enum.cpp
    FileSystem.cpp
    #HelpSystem.cpp
    #HistogramTool.cpp
    IToolWidget.cpp
    IViewSystem.cpp
    #ImageSystem.cpp
    ImageView.cpp
    #InfoTool.cpp
    #MagnifyTool.cpp
    MDIWindow.cpp
    MainWindow.cpp
    Media.cpp
    MediaSession.cpp
    #PlaybackSystem.cpp
    PlaybackWidget.cpp
    TimelineSlider.cpp
    #TimelineWidget.cpp
    #ToolSystem.cpp
    #ViewSystem.cpp
    WindowSystem.cpp
    )

add_library(djvViewLib ${header} ${source})
target_link_libraries(djvViewLib djvDesktop)
set_target_properties(
    djvViewLib
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

