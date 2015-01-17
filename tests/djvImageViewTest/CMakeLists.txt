set(header
    djvImageViewTest.h)

set(mocHeader
    djvImageViewTest.h)

set(source
    djvImageViewTest.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImageViewTest
    ${djvGuiIncludeDirs})

add_executable(djvImageViewTest ${header} ${source} ${mocSource})

target_link_libraries(djvImageViewTest ${djvGuiLibs})

set_target_properties(djvImageViewTest PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvImageViewTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

