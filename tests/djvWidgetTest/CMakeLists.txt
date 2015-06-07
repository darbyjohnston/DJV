set(header
    djvAboutDialogTest.h
    djvChoiceDialogTest.h
    djvColorWidgetTest.h
    djvColorDialogTest.h
    djvFileBrowserTest.h
    djvFileEditTest.h
    djvIconLibraryTest.h
    djvInfoDialogTest.h
    djvInputDialogTest.h
    djvIntSliderTest.h
    djvIconLibraryTest.h
    djvMessageDialogTest.h
    djvMessagesDialogTest.h
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
    djvAboutDialogTest.h
    djvChoiceDialogTest.h
    djvColorDialogTest.h
    djvColorWidgetTest.h
    djvFileBrowserTest.h
    djvFileEditTest.h
    djvIconLibraryTest.h
    djvInfoDialogTest.h
    djvInputDialogTest.h
    djvIntSliderTest.h
    djvMessageDialogTest.h
    djvMessagesDialogTest.h
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
    djvAboutDialogTest.cpp
    djvChoiceDialogTest.cpp
    djvColorWidgetTest.cpp
    djvColorDialogTest.cpp
    djvFileBrowserTest.cpp
    djvFileEditTest.cpp
    djvIconLibraryTest.cpp
    djvInfoDialogTest.cpp
    djvInputDialogTest.cpp
    djvIntSliderTest.cpp
    djvMessageDialogTest.cpp
    djvMessagesDialogTest.cpp
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

