set(header
    djvAbstractPrefsWidget.h
    djvAbstractToolButton.h
    djvApplication.h
    djvApplicationAboutDialog.h
    djvApplicationInfoDialog.h
    djvApplicationMessageDialog.h
    djvChoiceButton.h
    djvChoiceDialog.h
    djvColorDialog.h
    djvColorSwatch.h
    djvColorWidget.h
    djvDebugLogDialog.h
    djvFileBrowser.h
    djvFileBrowserCache.h
    djvFileBrowserModel.h
    djvFileBrowserModelPrivate.h
    djvFileBrowserPrefs.h
    djvFileBrowserPrefsWidget.h
    djvFileEdit.h
    djvFloatDisplay.h
    djvFloatEdit.h
    djvFloatEditSlider.h
    djvFloatObject.h
    djvFloatSlider.h
	djvFormLayoutAlign.h
    djvGuiExport.h
    djvHelpPrefs.h
	djvHelpPrefsWidget.h
    djvIconLibrary.h
    djvImageIoPrefs.h
    djvImageView.h
    djvIndexShortcut.h
    djvInputDialog.h
    djvIntDisplay.h
    djvIntEdit.h
    djvIntEditSlider.h
    djvIntObject.h
    djvIntSlider.h
    djvMessageDialog.h
    djvMultiChoiceDialog.h
    djvOpenGlPrefs.h
	djvOpenGlPrefsWidget.h
    djvOpenGlWidget.h
    djvOsxMenuHack.h
    djvPixelMaskWidget.h
    djvPixelWidget.h
    djvPrefs.h
    djvPrefsDialog.h
    djvPrefsGroupBox.h
    djvPrefsInline.h
    djvProgressDialog.h
    djvQuestionDialog.h
    djvSearchBox.h
    djvSequencePrefs.h
	djvSequencePrefsWidget.h
    djvShortcut.h
    djvShortcutsModel.h
    djvShortcutsWidget.h
    djvShuttleButton.h
    djvStyle.h
	djvStylePrefsWidget.h
    djvTimePrefs.h
	djvTimePrefsWidget.h
    djvToolButton.h
    djvVector2fEditWidget.h
    djvVector2iEditWidget.h
    djvWindowUtil.h)

set(mocHeader
    djvAbstractPrefsWidget.h
    djvAbstractToolButton.h
    djvApplicationAboutDialog.h
    djvApplication.h
    djvApplicationInfoDialog.h
    djvApplicationMessageDialog.h
    djvChoiceButton.h
    djvChoiceDialog.h
    djvColorDialog.h
    djvColorSwatch.h
    djvColorWidget.h
    djvDebugLogDialog.h
    djvFileBrowser.h
    djvFileBrowserModel.h
    djvFileBrowserModelPrivate.h
    djvFileBrowserPrefs.h
    djvFileBrowserPrefsWidget.h
    djvFileEdit.h
    djvFloatDisplay.h
    djvFloatEdit.h
    djvFloatEditSlider.h
    djvFloatObject.h
    djvFloatSlider.h
    djvFormLayoutAlign.h
    djvHelpPrefs.h
    djvHelpPrefsWidget.h
    djvIconLibrary.h
    djvImageIoPrefs.h
    djvImageView.h
    djvIndexShortcut.h
    djvInputDialog.h
    djvIntDisplay.h
    djvIntEdit.h
    djvIntEditSlider.h
    djvIntObject.h
    djvIntSlider.h
    djvMultiChoiceDialog.h
    djvOpenGlPrefs.h
    djvOpenGlPrefsWidget.h
    djvOpenGlWidget.h
    djvOsxMenuHack.h
    djvPixelMaskWidget.h
    djvPixelWidget.h
    djvPrefs.h
    djvPrefsDialog.h
    djvPrefsGroupBox.h
    djvProgressDialog.h
    djvQuestionDialog.h
    djvSearchBox.h
    djvSequencePrefs.h
    djvSequencePrefsWidget.h
    djvShortcutsModel.h
    djvShortcutsWidget.h
    djvShuttleButton.h
    djvStyle.h
    djvStylePrefsWidget.h
    djvTimePrefs.h
    djvTimePrefsWidget.h
    djvToolButton.h
    djvVector2fEditWidget.h
    djvVector2iEditWidget.h)

set(source
    djvAbstractPrefsWidget.cpp
    djvAbstractToolButton.cpp
    djvApplication.cpp
    djvApplicationAboutDialog.cpp
    djvApplicationInfoDialog.cpp
    djvApplicationMessageDialog.cpp
    djvChoiceButton.cpp
    djvChoiceDialog.cpp
    djvColorDialog.cpp
    djvColorSwatch.cpp
    djvColorWidget.cpp
    djvDebugLogDialog.cpp
    djvFileBrowser.cpp
    djvFileBrowserCache.cpp
    djvFileBrowserModel.cpp
    djvFileBrowserModelPrivate.cpp
    djvFileBrowserPrefs.cpp
    djvFileBrowserPrefsWidget.cpp
    djvFileEdit.cpp
    djvFloatDisplay.cpp
    djvFloatEdit.cpp
    djvFloatEditSlider.cpp
    djvFloatObject.cpp
    djvFloatSlider.cpp
	djvFormLayoutAlign.cpp
    djvHelpPrefs.cpp
	djvHelpPrefsWidget.cpp
    djvIconLibrary.cpp
    djvImageIoPrefs.cpp
    djvImageView.cpp
    djvIndexShortcut.cpp
    djvInputDialog.cpp
    djvIntDisplay.cpp
    djvIntEdit.cpp
    djvIntEditSlider.cpp
    djvIntObject.cpp
    djvIntSlider.cpp
    djvMessageDialog.cpp
    djvMultiChoiceDialog.cpp
    djvOpenGlPrefs.cpp
	djvOpenGlPrefsWidget.cpp
    djvOpenGlWidget.cpp
    djvOsxMenuHack.cpp
    djvPixelMaskWidget.cpp
    djvPixelWidget.cpp
    djvPrefs.cpp
    djvPrefsDialog.cpp
    djvPrefsGroupBox.cpp
    djvProgressDialog.cpp
    djvQuestionDialog.cpp
    djvSearchBox.cpp
    djvSequencePrefs.cpp
	djvSequencePrefsWidget.cpp
    djvShortcut.cpp
    djvShortcutsModel.cpp
    djvShortcutsWidget.cpp
    djvShuttleButton.cpp
    djvStyle.cpp
	djvStylePrefsWidget.cpp
    djvTimePrefs.cpp
	djvTimePrefsWidget.cpp
    djvToolButton.cpp
    djvVector2fEditWidget.cpp
    djvVector2iEditWidget.cpp
    djvWindowUtil.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
    QT5_ADD_RESOURCES(rccSource djvGui.qrc)
    QT5_CREATE_TRANSLATION(qmSource ${source}
        djvGui_en_US.ts
        djvGui_cs_CZ.ts)
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
    QT4_ADD_RESOURCES(rccSource djvGui.qrc)
    QT4_CREATE_TRANSLATION(qmSource ${source}
        djvGui_en_US.ts
        djvGui_cs_CZ.ts)
endif()

include_directories(${djvGuiIncludeDirs})

add_library(djvGui ${header} ${source} ${mocSource} ${rccSource} ${qmSource})

target_link_libraries(djvGui ${djvGuiDeps})

set_target_properties(djvGui PROPERTIES FOLDER lib)

if (djvInstallRpath)

    set_target_properties(
        djvGui PROPERTIES
        INSTALL_RPATH "${djvInstallRpath}")

endif (djvInstallRpath)

install(
    FILES ${header}
    DESTINATION include)

install(
    FILES ${qmSource}
    DESTINATION translations)

install(
    TARGETS djvGui
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

