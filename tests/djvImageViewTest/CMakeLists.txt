set(header
    djvImageViewTest.h)

set(mocHeader
    djvImageViewTest.h)

set(source
    djvImageViewTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImageViewTest
    ${djvGuiIncludeDirs})

add_executable(djvImageViewTest ${header} ${source} ${mocSource})

target_link_libraries(djvImageViewTest ${djvGuiLibs})

set_target_properties(djvImageViewTest PROPERTIES FOLDER tests CXX_STANDARD 11)

if (djvImageIoPlugins)

    add_dependencies(djvImageViewTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

