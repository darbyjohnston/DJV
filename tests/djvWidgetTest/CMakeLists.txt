set(header
    djvApplicationAboutDialogTest.h
    djvApplicationInfoDialogTest.h
    djvApplicationMessageDialogTest.h
    djvChoiceDialogTest.h
    djvColorWidgetTest.h
    djvColorDialogTest.h
    djvInputDialogTest.h
    djvIntSliderTest.h
    djvFileBrowserTest.h
    djvFileEditTest.h
    djvIconLibraryTest.h
    djvMessageDialogTest.h
    djvMultiChoiceDialogTest.h
    djvNumWidgetTest.h
    djvPixelWidgetTest.h
    djvPrefsDialogTest.h
    djvProgressDialogTest.h
    djvQuestionDialogTest.h
    djvSearchBoxTest.h
    djvShortcutsWidgetTest.h
    djvToolButtonTest.h
    djvVectorWidgetTest.h
    djvWidgetTest.h)

set(mocHeader
    djvApplicationAboutDialogTest.h
    djvApplicationInfoDialogTest.h
    djvApplicationMessageDialogTest.h
    djvChoiceDialogTest.h
    djvColorDialogTest.h
    djvColorWidgetTest.h
    djvFileBrowserTest.h
    djvFileEditTest.h
    djvIconLibraryTest.h
    djvInputDialogTest.h
    djvIntSliderTest.h
    djvMessageDialogTest.h
    djvMultiChoiceDialogTest.h
    djvNumWidgetTest.h
    djvPixelWidgetTest.h
    djvPrefsDialogTest.h
    djvProgressDialogTest.h
    djvQuestionDialogTest.h
    djvSearchBoxTest.h
    djvShortcutsWidgetTest.h
    djvToolButtonTest.h
    djvVectorWidgetTest.h
    djvWidgetTest.h)

set(source
    djvApplicationAboutDialogTest.cpp
    djvApplicationInfoDialogTest.cpp
    djvApplicationMessageDialogTest.cpp
    djvChoiceDialogTest.cpp
    djvColorWidgetTest.cpp
    djvColorDialogTest.cpp
    djvFileBrowserTest.cpp
    djvFileEditTest.cpp
    djvIconLibraryTest.cpp
    djvInputDialogTest.cpp
    djvIntSliderTest.cpp
    djvMessageDialogTest.cpp
    djvMultiChoiceDialogTest.cpp
    djvNumWidgetTest.cpp
    djvPixelWidgetTest.cpp
    djvPrefsDialogTest.cpp
    djvProgressDialogTest.cpp
    djvQuestionDialogTest.cpp
    djvSearchBoxTest.cpp
    djvShortcutsWidgetTest.cpp
    djvToolButtonTest.cpp
    djvVectorWidgetTest.cpp
    djvWidgetTest.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvWidgetTest
    ${djvGuiIncludeDirs})

add_executable(djvWidgetTest ${header} ${source} ${mocSource})

target_link_libraries(djvWidgetTest ${djvGuiLibs})

set_target_properties(djvWidgetTest PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvWidgetTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

