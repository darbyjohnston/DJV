set(header
    Context.h
    FileBrowser.h
    FileBrowser.h
    FileBrowserPrivate.h
    ProxyStyle.h)
set(mocHeader
    Context.h
    FileBrowser.h
    FileBrowserPrivate.h
    ProxyStyle.h)
set(source
    Context.cpp
    FileBrowser.cpp
    FileBrowserModel.cpp
    FileBrowserWidget.cpp
    ProxyStyle.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
add_library(djvViewExperimentUI ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvViewExperimentUI djvViewExperimentAV)
set_target_properties(
    djvViewExperimentUI
    PROPERTIES
    FOLDER experiments/djvViewExperiment
    CXX_STANDARD 11)
