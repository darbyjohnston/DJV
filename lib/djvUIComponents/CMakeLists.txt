find_package(JPEG)
find_package(TIFF)

set(header
    ColorPicker.h
    DisplaySettingsWidget.h
    FileBrowser.h
    FileBrowserDialog.h
    FileBrowserItemView.h
    FileBrowserPrivate.h
    FileBrowserSettings.h
    ISettingsWidget.h
    LanguageSettingsWidget.h
	LineGraphWidget.h
    ListViewHeader.h
    PPMSettingsWidget.h
    SearchBox.h
	ThermometerWidget.h
	TimeSettingsWidget.h
    TooltipsSettingsWidget.h
    UIComponentsSystem.h)
set(source
    ColorPicker.cpp
    DisplaySettingsWidget.cpp
    FileBrowser.cpp
    FileBrowserDialog.cpp
    FileBrowserDrivesWidget.cpp
    FileBrowserItemView.cpp
    FileBrowserPathWidget.cpp
    FileBrowserSettings.cpp
    FileBrowserShortcutsModel.cpp
    FileBrowserShortcutsWidget.cpp
    ISettingsWidget.cpp
    LanguageSettingsWidget.cpp
	LineGraphWidget.cpp
    ListViewHeader.cpp
    PPMSettingsWidget.cpp
    SearchBox.cpp
	ThermometerWidget.cpp
	TimeSettingsWidget.cpp
    TooltipsSettingsWidget.cpp
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
