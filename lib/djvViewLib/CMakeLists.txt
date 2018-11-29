set(header
    Application.h
    ColorPickerTool.h
    Context.h
    Enum.h
    FileSystem.h
    HelpSystem.h
    HistogramTool.h
    IViewSystem.h
    ImageSystem.h
    ImageView.h
    InfoTool.h
    MagnifyTool.h
    MainWindow.h
    MediaPlayer.h
    PlaybackSystem.h
    Project.h
    ToolSystem.h
    ViewSystem.h
    WindowSystem.h
    WorkspacePrivate.h
    WorkspaceSystem.h
    Workspace.h)
set(mocHeader
    Application.h
    ColorPickerTool.h
    Context.h
    Enum.h
    FileSystem.h
    HelpSystem.h
    HistogramTool.h
    IViewSystem.h
    ImageSystem.h
    ImageView.h
    InfoTool.h
    MagnifyTool.h
    MainWindow.h
    MediaPlayer.h
    PlaybackSystem.h
    Project.h
    ToolSystem.h
    ViewSystem.h
    WindowSystem.h
    WorkspaceSystem.h
    WorkspacePrivate.h
    Workspace.h)
set(source
    Application.cpp
    ColorPickerTool.cpp
    Context.cpp
    Enum.cpp
    FileSystem.cpp
    HelpSystem.cpp
    HistogramTool.cpp
    IViewSystem.cpp
    ImageSystem.cpp
    ImageView.cpp
    InfoTool.cpp
    MagnifyTool.cpp
    MainWindow.cpp
    MediaPlayer.cpp
    PlaybackSystem.cpp
    Project.cpp
    ToolSystem.cpp
    ViewSystem.cpp
    WindowSystem.cpp
    WorkspaceSystem.cpp
    WorkspacePrivate.cpp
    Workspace.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
add_library(djvViewLib ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvViewLib djvUI)
set_target_properties(
    djvViewLib
    PROPERTIES
    FOLDER lib
    XX_STANDARD 11)

