set(header
    djvViewWidgetTest.h)

set(mocHeader
    djvViewWidgetTest.h)

set(source
    djvViewWidgetTest.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvViewWidgetTest
    ${djvGuiIncludeDirs}
    ${CMAKE_SOURCE_DIR}/lib/djvViewLib)

add_executable(djvViewWidgetTest ${header} ${source} ${mocSource})

target_link_libraries(djvViewWidgetTest ${djvGuiLibs} djvViewLib)

set_target_properties(djvViewWidgetTest PROPERTIES FOLDER tests)
