set(header
    djvViewWidgetTest.h)

set(source
    djvViewWidgetTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvViewWidgetTest
    ${djvGuiIncludeDirs}
    ${CMAKE_SOURCE_DIR}/lib/djvViewLib)

add_executable(djvViewWidgetTest ${header} ${source})

target_link_libraries(djvViewWidgetTest ${djvGuiLibs} djvViewLib)

set_target_properties(djvViewWidgetTest PROPERTIES FOLDER tests)
