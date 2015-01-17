set(header
    djvGlslTestApplication.h
    djvGlslTestBlur.h
    djvGlslTestColor.h
    djvGlslTestEdge.h
    djvGlslTestExposure.h
    djvGlslTestOp.h
    djvGlslTestLevels.h
    djvGlslTestScale.h
    djvGlslTestSharpen.h
    djvGlslTestUtil.h
    djvGlslTestWidget.h)

set(headerMoc
    djvGlslTestApplication.h
    djvGlslTestBlur.h
    djvGlslTestColor.h
    djvGlslTestExposure.h
    djvGlslTestLevels.h
    djvGlslTestOp.h
    djvGlslTestScale.h
    djvGlslTestSharpen.h)

set(source
    djvGlslTestApplication.cpp
    djvGlslTestBlur.cpp
    djvGlslTestColor.cpp
    djvGlslTestOp.cpp
    djvGlslTestEdge.cpp
    djvGlslTestExposure.cpp
    djvGlslTestLevels.cpp
    djvGlslTestScale.cpp
    djvGlslTestSharpen.cpp
    djvGlslTestUtil.cpp
    djvGlslTestWidget.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${headerMoc})
else()
    QT4_WRAP_CPP(mocSource ${headerMoc})
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

