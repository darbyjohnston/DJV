set(header
    FileBrowser.h
    FileBrowser.h
    FileBrowserPrivate.h
    ProxyStyle.h
    System.h
    Util.h)
set(mocHeader
    FileBrowser.h
    FileBrowserPrivate.h
    ProxyStyle.h
    Util.h)
set(source
    FileBrowser.cpp
    FileBrowserModel.cpp
    FileBrowserWidget.cpp
    ProxyStyle.cpp
    System.cpp
    Util.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

add_library(djvUIQt ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvUIQt djvUICore)
set_target_properties(
    djvUIQt
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)
