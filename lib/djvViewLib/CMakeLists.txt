set(header
	djvViewAbstractActions.h
	djvViewAbstractGroup.h
	djvViewAbstractMenu.h
	djvViewAbstractPrefs.h
	djvViewAbstractPrefsWidget.h
	djvViewAbstractTool.h
	djvViewAbstractToolBar.h
    djvViewApplication.h
	djvViewColorPickerTool.h
    djvViewDisplayProfile.h
    djvViewDisplayProfileWidget.h
    djvViewFileActions.h
    djvViewFileCache.h
    djvViewFileGroup.h
    djvViewFileMenu.h
    djvViewFilePrefs.h
    djvViewFilePrefsWidget.h
    djvViewFileSave.h
    djvViewFileToolBar.h
	djvViewHelpActions.h
	djvViewHelpGroup.h
	djvViewHelpMenu.h
	djvViewHistogramTool.h
    djvViewHudInfo.h
    djvViewImageActions.h
    djvViewImageGroup.h
    djvViewImageMenu.h
    djvViewImagePrefs.h
    djvViewImagePrefsWidget.h
    djvViewImageToolBar.h
    djvViewImageView.h
	djvViewInfoTool.h
    djvViewInputPrefs.h
    djvViewInputPrefsWidget.h
    djvViewLibExport.h
	djvViewMagnifyTool.h
    djvViewMainWindow.h
    djvViewMiscWidget.h
    djvViewPlaybackActions.h
    djvViewPlaybackGroup.h
    djvViewPlaybackMenu.h
    djvViewPlaybackPrefs.h
    djvViewPlaybackPrefsWidget.h
    djvViewPlaybackToolBar.h
    djvViewPlaybackWidgets.h
	djvViewShortcutPrefs.h
	djvViewShortcutPrefsWidget.h
	djvViewToolActions.h
	djvViewToolGroup.h
	djvViewToolMenu.h
	djvViewToolToolBar.h
    djvViewUtil.h
	djvViewViewActions.h
	djvViewViewGroup.h
	djvViewViewMenu.h
	djvViewViewPrefs.h
	djvViewViewPrefsWidget.h
	djvViewViewToolBar.h
	djvViewWindowActions.h
	djvViewWindowGroup.h
	djvViewWindowMenu.h
	djvViewWindowPrefs.h
	djvViewWindowPrefsWidget.h
	djvViewWindowToolBar.h)

set(mocHeader
    djvViewAbstractActions.h
    djvViewAbstractGroup.h
    djvViewAbstractMenu.h
    djvViewAbstractPrefs.h
    djvViewAbstractPrefsWidget.h
    djvViewAbstractToolBar.h
    djvViewAbstractTool.h
    djvViewApplication.h
    djvViewColorPickerTool.h
    djvViewDisplayProfileWidget.h
    djvViewFileActions.h
    djvViewFileCache.h
    djvViewFileGroup.h
    djvViewFileMenu.h
    djvViewFilePrefs.h
    djvViewFilePrefsWidget.h
    djvViewFileSave.h
    djvViewFileToolBar.h
    djvViewHelpActions.h
    djvViewHelpGroup.h
    djvViewHelpMenu.h
    djvViewHistogramTool.h
    djvViewImageActions.h
    djvViewImageGroup.h
    djvViewImageMenu.h
    djvViewImagePrefs.h
    djvViewImagePrefsWidget.h
    djvViewImageToolBar.h
    djvViewImageView.h
    djvViewInfoTool.h
    djvViewInputPrefs.h
    djvViewInputPrefsWidget.h
    djvViewMagnifyTool.h
    djvViewMainWindow.h
    djvViewMiscWidget.h
    djvViewPlaybackActions.h
    djvViewPlaybackGroup.h
    djvViewPlaybackMenu.h
    djvViewPlaybackPrefs.h
    djvViewPlaybackPrefsWidget.h
    djvViewPlaybackToolBar.h
    djvViewPlaybackWidgets.h
    djvViewShortcutPrefs.h
    djvViewShortcutPrefsWidget.h
    djvViewToolActions.h
    djvViewToolGroup.h
    djvViewToolMenu.h
    djvViewToolToolBar.h
    djvViewUtil.h
    djvViewViewActions.h
    djvViewViewGroup.h
    djvViewViewMenu.h
    djvViewViewPrefs.h
    djvViewViewPrefsWidget.h
    djvViewViewToolBar.h
    djvViewWindowActions.h
    djvViewWindowGroup.h
    djvViewWindowMenu.h
    djvViewWindowPrefs.h
    djvViewWindowPrefsWidget.h
    djvViewWindowToolBar.h)

set(source
	djvViewAbstractActions.cpp
	djvViewAbstractGroup.cpp
	djvViewAbstractMenu.cpp
	djvViewAbstractPrefs.cpp
	djvViewAbstractPrefsWidget.cpp
	djvViewAbstractTool.cpp
	djvViewAbstractToolBar.cpp
    djvViewApplication.cpp
	djvViewColorPickerTool.cpp
    djvViewDisplayProfile.cpp
    djvViewDisplayProfileWidget.cpp
    djvViewFileActions.cpp
    djvViewFileCache.cpp
    djvViewFileGroup.cpp
    djvViewFileMenu.cpp
    djvViewFilePrefs.cpp
    djvViewFilePrefsWidget.cpp
    djvViewFileSave.cpp
    djvViewFileToolBar.cpp
	djvViewHelpActions.cpp
	djvViewHelpGroup.cpp
	djvViewHelpMenu.cpp
	djvViewHistogramTool.cpp
    djvViewHudInfo.cpp
    djvViewImageActions.cpp
    djvViewImageGroup.cpp
    djvViewImageMenu.cpp
    djvViewImagePrefs.cpp
    djvViewImagePrefsWidget.cpp
    djvViewImageToolBar.cpp
    djvViewImageView.cpp
	djvViewInfoTool.cpp
    djvViewInputPrefs.cpp
    djvViewInputPrefsWidget.cpp
	djvViewMagnifyTool.cpp
    djvViewMainWindow.cpp
    djvViewMiscWidget.cpp
    djvViewPlaybackActions.cpp
    djvViewPlaybackGroup.cpp
    djvViewPlaybackMenu.cpp
    djvViewPlaybackPrefs.cpp
    djvViewPlaybackPrefsWidget.cpp
    djvViewPlaybackToolBar.cpp
    djvViewPlaybackWidgets.cpp
	djvViewShortcutPrefs.cpp
	djvViewShortcutPrefsWidget.cpp
	djvViewToolActions.cpp
	djvViewToolGroup.cpp
	djvViewToolMenu.cpp
	djvViewToolToolBar.cpp
    djvViewUtil.cpp
	djvViewViewActions.cpp
	djvViewViewGroup.cpp
	djvViewViewMenu.cpp
	djvViewViewPrefs.cpp
	djvViewViewPrefsWidget.cpp
	djvViewViewToolBar.cpp
	djvViewWindowActions.cpp
	djvViewWindowGroup.cpp
	djvViewWindowMenu.cpp
	djvViewWindowPrefs.cpp
	djvViewWindowPrefsWidget.cpp
	djvViewWindowToolBar.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_ADD_RESOURCES(rccSource djvViewLib.qrc)
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djvViewLib_en_US.ts
        djvViewLib_cs_CZ.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_ADD_RESOURCES(rccSource djvViewLib.qrc)
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djvViewLib_en_US.ts
        djvViewLib_cs_CZ.ts)
endif()

include_directories(${CMAKE_SOURCE_DIR}/lib/djvViewLib ${djvGuiIncludeDirs})

add_library(djvViewLib ${header} ${source} ${mocSource} ${rccSource} ${qmSource})

target_link_libraries(djvViewLib ${djvGuiLibs})

set_target_properties(djvViewLib PROPERTIES FOLDER lib)

if (djvInstallRpath)

    set_target_properties(
        djvViewLib PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${header}
    DESTINATION include)

install(
    FILES ${qmSource}
    DESTINATION translations)

install(
    TARGETS djvViewLib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

