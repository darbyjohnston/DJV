find_package(JPEG)
find_package(TIFF)

set(header
    AVIOSettings.h
    ActionButton.h
    ColorPicker.h
    FileBrowser.h
    FileBrowserDialog.h
    FileBrowserItemView.h
    FileBrowserPrivate.h
    FileBrowserSettings.h
    GeneralSettingsWidget.h
    ISettingsWidget.h
    ListViewHeader.h
    PPMSettingsWidget.h
    SearchBox.h
    UIComponentsSystem.h)
set(source
    AVIOSettings.cpp
    ActionButton.cpp
    ColorPicker.cpp
    FileBrowser.cpp
    FileBrowserDialog.cpp
    FileBrowserDrivesWidget.cpp
    FileBrowserItemView.cpp
    FileBrowserPathWidget.cpp
    FileBrowserSettings.cpp
    FileBrowserShortcutsModel.cpp
    FileBrowserShortcutsWidget.cpp
    GeneralSettingsWidget.cpp
    ISettingsWidget.cpp
    ListViewHeader.cpp
    PPMSettingsWidget.cpp
    SearchBox.cpp
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
