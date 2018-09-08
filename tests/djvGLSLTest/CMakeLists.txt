set(header
    djvGLSLTestAbstractOp.h
    djvGLSLTestApplication.h
    djvGLSLTestBlurOp.h
    djvGLSLTestColorOp.h
    djvGLSLTestContext.h
    djvGLSLTestEdgeOp.h
    djvGLSLTestExposureOp.h
    djvGLSLTestImageLoad.h
    djvGLSLTestLevelsOp.h
    djvGLSLTestMainWindow.h
    djvGLSLTestNullOp.h
    djvGLSLTestOpDockWidget.h
    djvGLSLTestOpListDockWidget.h
    djvGLSLTestOpsManager.h
    djvGLSLTestPlayback.h
    djvGLSLTestPlaybackToolBar.h
    djvGLSLTestScaleOp.h
    djvGLSLTestSharpenOp.h
    djvGLSLTestUtil.h
    djvGLSLTestViewWidget.h)
set(mocHeader
    djvGLSLTestAbstractOp.h
    djvGLSLTestApplication.h
    djvGLSLTestBlurOp.h
    djvGLSLTestColorOp.h
    djvGLSLTestContext.h
    djvGLSLTestEdgeOp.h
    djvGLSLTestExposureOp.h
    djvGLSLTestImageLoad.h
    djvGLSLTestLevelsOp.h
    djvGLSLTestMainWindow.h
    djvGLSLTestNullOp.h
    djvGLSLTestOpDockWidget.h
    djvGLSLTestOpListDockWidget.h
    djvGLSLTestOpsManager.h
    djvGLSLTestPlayback.h
    djvGLSLTestPlaybackToolBar.h
    djvGLSLTestScaleOp.h
    djvGLSLTestSharpenOp.h
    djvGLSLTestViewWidget.h)
set(source
    djvGLSLTestAbstractOp.cpp
    djvGLSLTestApplication.cpp
    djvGLSLTestBlurOp.cpp
    djvGLSLTestColorOp.cpp
    djvGLSLTestContext.cpp
    djvGLSLTestEdgeOp.cpp
    djvGLSLTestExposureOp.cpp
    djvGLSLTestImageLoad.cpp
    djvGLSLTestLevelsOp.cpp
    djvGLSLTestMainWindow.cpp
    djvGLSLTestNullOp.cpp
    djvGLSLTestOpDockWidget.cpp
    djvGLSLTestOpListDockWidget.cpp
    djvGLSLTestOpsManager.cpp
    djvGLSLTestPlayback.cpp
    djvGLSLTestPlaybackToolBar.cpp
    djvGLSLTestScaleOp.cpp
    djvGLSLTestSharpenOp.cpp
    djvGLSLTestUtil.cpp
    djvGLSLTestViewWidget.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvGLSLTest
    ${CMAKE_SOURCE_DIR}/lib/djvViewLib
    ${CMAKE_SOURCE_DIR}/lib/djvUI
    ${CMAKE_SOURCE_DIR}/lib/djvGraphics
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${OPENGL_INCLUDE_DIRS})
add_executable(djvGLSLTest ${header} ${source} ${mocSource})
target_link_libraries(djvGLSLTest djvViewLib)
set_target_properties(djvGLSLTest PROPERTIES FOLDER tests CXX_STANDARD 11)

