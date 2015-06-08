set(header
    djvGlslTestAbstractOp.h
    djvGlslTestApplication.h
    djvGlslTestBlurOp.h
    djvGlslTestColorOp.h
    djvGlslTestContext.h
    djvGlslTestEdgeOp.h
    djvGlslTestExposureOp.h
    djvGlslTestImageLoad.h
    djvGlslTestLevelsOp.h
    djvGlslTestMainWindow.h
    djvGlslTestNullOp.h
    djvGlslTestOpDockWidget.h
    djvGlslTestOpListDockWidget.h
    djvGlslTestOpsManager.h
    djvGlslTestPlayback.h
    djvGlslTestPlaybackButtons.h
    djvGlslTestPlaybackToolBar.h
    djvGlslTestScaleOp.h
    djvGlslTestSharpenOp.h
    djvGlslTestUtil.h
    djvGlslTestViewWidget.h)

set(mocHeader
    djvGlslTestAbstractOp.h
    djvGlslTestApplication.h
    djvGlslTestBlurOp.h
    djvGlslTestColorOp.h
    djvGlslTestContext.h
    djvGlslTestEdgeOp.h
    djvGlslTestExposureOp.h
    djvGlslTestImageLoad.h
    djvGlslTestLevelsOp.h
    djvGlslTestMainWindow.h
    djvGlslTestNullOp.h
    djvGlslTestOpDockWidget.h
    djvGlslTestOpListDockWidget.h
    djvGlslTestOpsManager.h
    djvGlslTestPlayback.h
    djvGlslTestPlaybackButtons.h
    djvGlslTestPlaybackToolBar.h
    djvGlslTestScaleOp.h
    djvGlslTestSharpenOp.h
    djvGlslTestViewWidget.h)

set(source
    djvGlslTestAbstractOp.cpp
    djvGlslTestApplication.cpp
    djvGlslTestBlurOp.cpp
    djvGlslTestColorOp.cpp
    djvGlslTestContext.cpp
    djvGlslTestEdgeOp.cpp
    djvGlslTestExposureOp.cpp
    djvGlslTestImageLoad.cpp
    djvGlslTestLevelsOp.cpp
    djvGlslTestMainWindow.cpp
    djvGlslTestNullOp.cpp
    djvGlslTestOpDockWidget.cpp
    djvGlslTestOpListDockWidget.cpp
    djvGlslTestOpsManager.cpp
    djvGlslTestPlayback.cpp
    djvGlslTestPlaybackButtons.cpp
    djvGlslTestPlaybackToolBar.cpp
    djvGlslTestScaleOp.cpp
    djvGlslTestSharpenOp.cpp
    djvGlslTestUtil.cpp
    djvGlslTestViewWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvGlslTest
    ${CMAKE_SOURCE_DIR}/lib/djvViewLib
    ${djvGuiIncludeDirs})

add_executable(djvGlslTest ${header} ${source} ${mocSource})

target_link_libraries(djvGlslTest djvViewLib ${djvGuiLibs})

set_target_properties(djvGlslTest PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvGlslTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

