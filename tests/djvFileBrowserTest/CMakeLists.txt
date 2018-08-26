set(header
    djvFileBrowserTestAbstractWorker.h
    djvFileBrowserTestActionData.h
    djvFileBrowserTestActions.h
    djvFileBrowserTestApplication.h
    djvFileBrowserTestContactSheetDelegate.h
    djvFileBrowserTestControls.h
    djvFileBrowserTestDir.h
    djvFileBrowserTestInfo.h
    djvFileBrowserTestItem.h
    djvFileBrowserTestModel.h
    djvFileBrowserTestThumbnail.h
    djvFileBrowserTestUtil.h
    djvFileBrowserTestView.h
    djvFileBrowserTestWidget.h)

set(mocHeader
    djvFileBrowserTestAbstractWorker.h
    djvFileBrowserTestActionData.h
    djvFileBrowserTestActions.h
    djvFileBrowserTestApplication.h
    djvFileBrowserTestContactSheetDelegate.h
    djvFileBrowserTestControls.h
    djvFileBrowserTestDir.h
    djvFileBrowserTestInfo.h
    djvFileBrowserTestItem.h
    djvFileBrowserTestModel.h
    djvFileBrowserTestThumbnail.h
    djvFileBrowserTestUtil.h
    djvFileBrowserTestView.h
    djvFileBrowserTestWidget.h)

set(source
    djvFileBrowserTestAbstractWorker.cpp
    djvFileBrowserTestActionData.cpp
    djvFileBrowserTestActions.cpp
    djvFileBrowserTestApplication.cpp
    djvFileBrowserTestContactSheetDelegate.cpp
    djvFileBrowserTestControls.cpp
    djvFileBrowserTestDir.cpp
    djvFileBrowserTestInfo.cpp
    djvFileBrowserTestItem.cpp
    djvFileBrowserTestModel.cpp
    djvFileBrowserTestThumbnail.cpp
    djvFileBrowserTestUtil.cpp
    djvFileBrowserTestView.cpp
    djvFileBrowserTestWidget.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvFileBrowserTest
    ${djvGuiIncludeDirs})

add_executable(djvFileBrowserTest ${header} ${source} ${mocSource})

target_link_libraries(djvFileBrowserTest ${djvGuiLibs})

set_target_properties(djvFileBrowserTest PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvFileBrowserTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

