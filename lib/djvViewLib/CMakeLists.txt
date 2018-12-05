set(header
    Application.h
    ColorPickerTool.h
    Context.h
    ContextInline.h
    DebugTool.h
    Enum.h
    FileObject.h
    HelpObject.h
    HistogramTool.h
    IViewObject.h
    ImageObject.h
    ImageView.h
    InfoTool.h
    MagnifyTool.h
    MainWindow.h
    Media.h
    PlaybackObject.h
    TimelineSlider.h
    TimelineWidget.h
    ToolObject.h
    ViewObject.h
    WindowObject.h
    WorkspaceObject.h
    WorkspacePrivate.h
    Workspace.h)
set(mocHeader
    Application.h
    ColorPickerTool.h
    DebugTool.h
    Enum.h
    FileObject.h
    HelpObject.h
    HistogramTool.h
    IViewObject.h
    ImageObject.h
    ImageView.h
    InfoTool.h
    MagnifyTool.h
    MainWindow.h
    PlaybackObject.h
    TimelineSlider.h
    TimelineWidget.h
    ToolObject.h
    ViewObject.h
    WindowObject.h
    WorkspaceObject.h
    WorkspacePrivate.h
    Workspace.h)
set(source
    Application.cpp
    ColorPickerTool.cpp
    Context.cpp
    DebugTool.cpp
    Enum.cpp
    FileObject.cpp
    HelpObject.cpp
    HistogramTool.cpp
    IViewObject.cpp
    ImageObject.cpp
    ImageView.cpp
    InfoTool.cpp
    MagnifyTool.cpp
    MainWindow.cpp
    Media.cpp
    PlaybackObject.cpp
    TimelineSlider.cpp
    TimelineWidget.cpp
    ToolObject.cpp
    ViewObject.cpp
    WindowObject.cpp
    WorkspaceObject.cpp
    WorkspacePrivate.cpp
    Workspace.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

add_library(djvViewLib ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvViewLib djvUIQt)
set_target_properties(
    djvViewLib
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

