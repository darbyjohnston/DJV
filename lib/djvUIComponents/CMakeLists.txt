set(header
    ColorPicker.h
    FileBrowser.h
    FileBrowserDialog.h
    FileBrowserItemView.h
    FileBrowserPrivate.h
    FileBrowserSettings.h
    FileBrowserSettingsWidget.h
    GLFWSettingsWidget.h
	IOSettings.h
	IOSettingsWidget.h
    LanguageSettingsWidget.h
	LineGraphWidget.h
    ListViewHeader.h
    MouseSettingsWidget.h
    Namespace.h
    PPMSettingsWidget.h
    Render2DSettingsWidget.h
    SceneWidget.h
    SceneWidgetFunc.h
    SearchBox.h
    SettingsIWidget.h
    ShortcutsWidget.h
    StyleSettingsWidget.h
	ThermometerWidget.h
	TimeSettingsWidget.h
    UIComponentsSystem.h
    UISettingsWidget.h)
set(source
    ColorPicker.cpp
    FileBrowser.cpp
    FileBrowserDialog.cpp
    FileBrowserDrivesWidget.cpp
    FileBrowserFunc.cpp
    FileBrowserItemView.cpp
    FileBrowserPathWidget.cpp
    FileBrowserPathsWidget.cpp
    FileBrowserRecentPathsWidget.cpp
    FileBrowserSettings.cpp
    FileBrowserSettingsWidget.cpp
    FileBrowserShortcutsModel.cpp
    FileBrowserShortcutsWidget.cpp
    FileBrowserThumbnailWidget.cpp
    GLFWSettingsWidget.cpp
	IOSettings.cpp
	IOSettingsWidget.cpp
    LanguageSettingsWidget.cpp
	LineGraphWidget.cpp
    ListViewHeader.cpp
    MouseSettingsWidget.cpp
    PPMSettingsWidget.cpp
    Render2DSettingsWidget.cpp
    SceneWidget.cpp
    SceneWidgetFunc.cpp
    SearchBox.cpp
    SettingsIWidget.cpp
    ShortcutsWidget.cpp
    StyleSettingsWidget.cpp
	ThermometerWidget.cpp
	TimeSettingsWidget.cpp
    UIComponentsSystem.cpp
    UISettingsWidget.cpp)

if(FFmpeg_FOUND)
    set(header
        ${header}
        FFmpegSettingsWidget.h)
    set(source
        ${source}
        FFmpegSettingsWidget.cpp)
endif()
if(JPEG_FOUND)
    set(header
        ${header}
        JPEGSettingsWidget.h)
    set(source
        ${source}
        JPEGSettingsWidget.cpp)
endif()
if(OpenEXR_FOUND)
    set(header
        ${header}
    	OpenEXRSettingsWidget.h)
    set(source
        ${source}
    	OpenEXRSettingsWidget.cpp)
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
target_link_libraries(djvUIComponents djvUI djvScene3D)
set_target_properties(
    djvUIComponents
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvUIComponents
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvUIComponents)

