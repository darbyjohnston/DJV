set(header
    djvImagePlay2TestApplication.h
    djvImagePlay2TestContext.h
    djvImagePlay2TestLoad.h
    djvImagePlay2TestPlayback.h
    djvImagePlay2TestPlayBar.h
    djvImagePlay2TestView.h
    djvImagePlay2TestWindow.h)

set(mocHeader
    djvImagePlay2TestApplication.h
    djvImagePlay2TestContext.h
    djvImagePlay2TestLoad.h
    djvImagePlay2TestPlayback.h
    djvImagePlay2TestPlayBar.h
    djvImagePlay2TestView.h
    djvImagePlay2TestWindow.h)

set(source
    djvImagePlay2TestApplication.cpp
    djvImagePlay2TestContext.cpp
    djvImagePlay2TestLoad.cpp
    djvImagePlay2TestPlayback.cpp
    djvImagePlay2TestPlayBar.cpp
    djvImagePlay2TestView.cpp
    djvImagePlay2TestWindow.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImagePlay2Test
    ${CMAKE_SOURCE_DIR}/lib/djvViewLib
    ${djvGuiIncludeDirs})

add_executable(djvImagePlay2Test ${header} ${source} ${mocSource})

target_link_libraries(djvImagePlay2Test djvViewLib ${djvGuiLibs})

set_target_properties(djvImagePlay2Test PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvImagePlay2Test ${djvImageIoPlugins})

endif (djvImageIoPlugins)
