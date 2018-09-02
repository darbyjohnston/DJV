set(header
    djvAbstractWidgetTest.h
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
    djvMessagesDialogTest.h
    djvMultiChoiceDialogTest.h
    djvNoticeDialogTest.h
    djvNumWidgetTest.h
    djvPixelWidgetTest.h
    djvPrefsDialogTest.h
    djvProgressDialogTest.h
    djvQuestionDialogTest.h
    djvSearchBoxTest.h
    djvShortcutsWidgetTest.h
    djvToolButtonTest.h
    djvVectorWidgetTest.h
    djvViewWidgetTest.h
    djvWidgetTestApplication.h
    djvWidgetTestModel.h
    djvWidgetTestManager.h
    djvWidgetTestWindow.h)

set(mocHeader
    djvAbstractWidgetTest.h
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
    djvMessagesDialogTest.h
    djvMultiChoiceDialogTest.h
    djvNoticeDialogTest.h
    djvNumWidgetTest.h
    djvPixelWidgetTest.h
    djvPrefsDialogTest.h
    djvProgressDialogTest.h
    djvQuestionDialogTest.h
    djvSearchBoxTest.h
    djvShortcutsWidgetTest.h
    djvToolButtonTest.h
    djvVectorWidgetTest.h
    djvViewWidgetTest.h
    djvWidgetTestApplication.h
    djvWidgetTestModel.h
    djvWidgetTestManager.h
    djvWidgetTestWindow.h)

set(source
    djvAbstractWidgetTest.cpp
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
    djvMessagesDialogTest.cpp
    djvMultiChoiceDialogTest.cpp
    djvNoticeDialogTest.cpp
    djvNumWidgetTest.cpp
    djvPixelWidgetTest.cpp
    djvPrefsDialogTest.cpp
    djvProgressDialogTest.cpp
    djvQuestionDialogTest.cpp
    djvSearchBoxTest.cpp
    djvShortcutsWidgetTest.cpp
    djvToolButtonTest.cpp
    djvVectorWidgetTest.cpp
    djvViewWidgetTest.cpp
    djvWidgetTestApplication.cpp
    djvWidgetTestModel.cpp
    djvWidgetTestManager.cpp
    djvWidgetTestWindow.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvWidgetTest
    ${CMAKE_SOURCE_DIR}/lib/djvViewLib
    ${djvGuiIncludeDirs})

add_executable(djvWidgetTest ${header} ${source} ${mocSource})

target_link_libraries(djvWidgetTest ${djvGuiLibs} djvViewLib)

set_target_properties(djvWidgetTest PROPERTIES FOLDER tests CXX_STANDARD 11)

if (djvImageIoPlugins)

    add_dependencies(djvWidgetTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

