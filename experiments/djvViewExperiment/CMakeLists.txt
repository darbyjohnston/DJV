find_package(GLM REQUIRED)

set(header
    Application.h
    Context.h
    ContextInline.h
    FileBrowser.h
    FileBrowserPrivate.h
    FileUtil.h
    ICommand.h
    ISystem.h
    IUISystem.h
    ImageView.h
    MainWindow.h
    Project.h
    ProjectTabWidget.h
    UndoStack.h
    Util.h
    WindowSystem.h)
set(mocHeader
    Application.h
    Context.h
    FileBrowser.h
    FileBrowserPrivate.h
    ICommand.h
    ISystem.h
    IUISystem.h
    ImageView.h
    MainWindow.h
    Project.h
    ProjectTabWidget.h
    UndoStack.h
    Util.h
    WindowSystem.h)
set(source
    Application.cpp
    Context.cpp
    FileBrowser.cpp
    FileBrowserModel.cpp
    FileBrowserWidget.cpp
    FileUtil.cpp
    ICommand.cpp
    ISystem.cpp
    IUISystem.cpp
    ImageView.cpp
    MainWindow.cpp
    Project.cpp
    ProjectTabWidget.cpp
    Util.cpp
    UndoStack.cpp
    WindowSystem.cpp
    main.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
add_executable(djvViewExperiment ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvViewExperiment
    Qt5
    GLM
    ${CMAKE_DL_LIBS})
set_target_properties(djvViewExperiment PROPERTIES FOLDER experiments CXX_STANDARD 11)

