set(header
    ColorPicker.h
    FileBrowser.h
    FileBrowserDialog.h
    FileBrowserItemView.h
    FileBrowserPrivate.h
    FileBrowserSettings.h
    GLFWSettingsWidget.h
	IOSettings.h
	IOSettingsWidget.h
    ISettingsWidget.h
    LanguageSettingsWidget.h
	LineGraphWidget.h
    ListViewHeader.h
    PPMSettingsWidget.h
    Render2DSettingsWidget.h
    SceneWidget.h
    SearchBox.h
    StyleSettingsWidget.h
	ThermometerWidget.h
	TimeSettingsWidget.h
    TooltipsSettingsWidget.h
    UIComponentsSystem.h)
set(source
    ColorPicker.cpp
    FileBrowser.cpp
    FileBrowserDialog.cpp
    FileBrowserDrivesWidget.cpp
    FileBrowserItemView.cpp
    FileBrowserPathWidget.cpp
    FileBrowserRecentPathsWidget.cpp
    FileBrowserSettings.cpp
    FileBrowserShortcutsModel.cpp
    FileBrowserShortcutsWidget.cpp
    GLFWSettingsWidget.cpp
	IOSettings.cpp
	IOSettingsWidget.cpp
    ISettingsWidget.cpp
    LanguageSettingsWidget.cpp
	LineGraphWidget.cpp
    ListViewHeader.cpp
    PPMSettingsWidget.cpp
    Render2DSettingsWidget.cpp
    SceneWidget.cpp
    SearchBox.cpp
    StyleSettingsWidget.cpp
	ThermometerWidget.cpp
	TimeSettingsWidget.cpp
    TooltipsSettingsWidget.cpp
    UIComponentsSystem.cpp)

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
target_link_libraries(djvUIComponents djvUI djvScene)
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

