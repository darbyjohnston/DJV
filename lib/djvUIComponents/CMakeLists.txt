find_package(JPEG)
find_package(TIFF)

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
    FileBrowserShortcutsModel.cpp
    FileBrowserShortcutsWidget.cpp
    ISettingsWidget.cpp
    LanguageSettingsWidget.cpp
    PPMSettingsWidget.cpp
    PaletteSettingsWidget.cpp
    UIComponentsSystem.cpp)

if(JPEG_FOUND)
    set(header
        ${header}
        JPEGSettingsWidget.h)
    set(source
        ${source}
        JPEGSettingsWidget.cpp)
endif()
if(TIFF_FOUND)
    set(header
        ${header}
        TIFFSettingsWidget.h)
    set(source
        ${source}
        TIFFSettingsWidget.cpp)
endif()

add_library(djvUIComponents ${header} ${source})
target_link_libraries(djvUIComponents djvUI)
set_target_properties(
    djvUIComponents
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)
