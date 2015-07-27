set(header
    djvFileBrowserTestApplication.h
    djvFileBrowserTestDir.h
    djvFileBrowserTestModel.h
    djvFileBrowserTestWindow.h)

set(mocHeader
    djvFileBrowserTestApplication.h
    djvFileBrowserTestDir.h
    djvFileBrowserTestModel.h
    djvFileBrowserTestWindow.h)

set(source
    djvFileBrowserTestApplication.cpp
    djvFileBrowserTestDir.cpp
    djvFileBrowserTestModel.cpp
    djvFileBrowserTestWindow.cpp)

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

