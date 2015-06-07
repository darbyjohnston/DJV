set(header
    djvGlslTestApplication.h
    djvGlslTestBlur.h
    djvGlslTestColor.h
    djvGlslTestContext.h
    djvGlslTestEdge.h
    djvGlslTestExposure.h
    djvGlslTestImageLoad.h
    djvGlslTestLevels.h
    djvGlslTestOp.h
    djvGlslTestOpManager.h
    djvGlslTestPlayback.h
    djvGlslTestPlaybackButtons.h
    djvGlslTestPlaybackWidget.h
    djvGlslTestScale.h
    djvGlslTestSharpen.h
    djvGlslTestUtil.h
    djvGlslTestWidget.h
    djvGlslTestWindow.h)

set(mocHeader
    djvGlslTestApplication.h
    djvGlslTestBlur.h
    djvGlslTestColor.h
    djvGlslTestContext.h
    djvGlslTestExposure.h
    djvGlslTestImageLoad.h
    djvGlslTestLevels.h
    djvGlslTestOp.h
    djvGlslTestOpManager.h
    djvGlslTestPlayback.h
    djvGlslTestPlaybackButtons.h
    djvGlslTestPlaybackWidget.h
    djvGlslTestScale.h
    djvGlslTestSharpen.h
    djvGlslTestWidget.h
    djvGlslTestWindow.h)

set(source
    djvGlslTestApplication.cpp
    djvGlslTestBlur.cpp
    djvGlslTestColor.cpp
    djvGlslTestContext.cpp
    djvGlslTestEdge.cpp
    djvGlslTestExposure.cpp
    djvGlslTestImageLoad.cpp
    djvGlslTestLevels.cpp
    djvGlslTestOp.cpp
    djvGlslTestOpManager.cpp
    djvGlslTestPlayback.cpp
    djvGlslTestPlaybackButtons.cpp
    djvGlslTestPlaybackWidget.cpp
    djvGlslTestScale.cpp
    djvGlslTestSharpen.cpp
    djvGlslTestUtil.cpp
    djvGlslTestWidget.cpp
    djvGlslTestWindow.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvGlslTest
    ${djvGuiIncludeDirs})

add_executable(djvGlslTest ${header} ${source} ${mocSource})

target_link_libraries(djvGlslTest ${djvGuiLibs})

set_target_properties(djvGlslTest PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvGlslTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

