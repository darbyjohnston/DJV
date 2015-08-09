set(header
    djvFileBrowserTestAbstractWorker.h
    djvFileBrowserTestApplication.h
    djvFileBrowserTestDirWorker.h
    djvFileBrowserTestIconDelegate.h
    djvFileBrowserTestInfoWorker.h
    djvFileBrowserTestModel.h
    djvFileBrowserTestThumbnailWorker.h
    djvFileBrowserTestUtil.h
    djvFileBrowserTestWidget.h)

set(mocHeader
    djvFileBrowserTestAbstractWorker.h
    djvFileBrowserTestApplication.h
    djvFileBrowserTestDirWorker.h
    djvFileBrowserTestIconDelegate.h
    djvFileBrowserTestInfoWorker.h
    djvFileBrowserTestModel.h
    djvFileBrowserTestThumbnailWorker.h
    djvFileBrowserTestUtil.h
    djvFileBrowserTestWidget.h)

set(source
    djvFileBrowserTestAbstractWorker.cpp
    djvFileBrowserTestApplication.cpp
    djvFileBrowserTestDirWorker.cpp
    djvFileBrowserTestIconDelegate.cpp
    djvFileBrowserTestInfoWorker.cpp
    djvFileBrowserTestModel.cpp
    djvFileBrowserTestThumbnailWorker.cpp
    djvFileBrowserTestUtil.cpp
    djvFileBrowserTestWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvFileBrowserTest
    ${djvGuiIncludeDirs})

add_executable(djvFileBrowserTest ${header} ${source} ${mocSource})

target_link_libraries(djvFileBrowserTest ${djvGuiLibs})

set_target_properties(djvFileBrowserTest PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvFileBrowserTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

