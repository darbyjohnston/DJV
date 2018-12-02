set(header
    Context.h
    FileBrowser.h
    FileBrowser.h
    FileBrowserPrivate.h
    ISettings.h
    ISettingsInline.h
    ProxyStyle.h
    SettingsSystem.h
    Style.h
    StyleInline.h
    StyleSettings.h)
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
    ISettings.cpp
    ProxyStyle.cpp
    SettingsSystem.cpp
    Style.cpp
    StyleSettings.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

add_library(djvUI ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvUI djvAV)
set_target_properties(
    djvUI
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)
