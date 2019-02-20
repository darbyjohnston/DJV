set(header
    AVIOSettings.h
    ActionButton.h
    DisplaySettingsWidget.h
    FileBrowser.h
    FileBrowserDialog.h
    FileBrowserItemView.h
    FileBrowserPrivate.h
    FileBrowserSettings.h
    ISettingsWidget.h
    JPEGSettingsWidget.h
    LanguageSettingsWidget.h
    PPMSettingsWidget.h
    PaletteSettingsWidget.h
    UIComponentsSystem.h)
set(source
    AVIOSettings.cpp
    ActionButton.cpp
    DisplaySettingsWidget.cpp
    FileBrowser.cpp
    FileBrowserDialog.cpp
    FileBrowserItemView.cpp
    FileBrowserPathWidget.cpp
    FileBrowserSettings.cpp
    FileBrowserShortcutsWidget.cpp
    ISettingsWidget.cpp
    JPEGSettingsWidget.cpp
    LanguageSettingsWidget.cpp
    PPMSettingsWidget.cpp
    PaletteSettingsWidget.cpp
    UIComponentsSystem.cpp)

add_library(djvUIComponents ${header} ${source})
target_link_libraries(djvUIComponents djvUI)
set_target_properties(
    djvUIComponents
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)
