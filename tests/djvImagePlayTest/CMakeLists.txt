set(header
    djvImagePlayTest.h)

set(mocHeader
    djvImagePlayTest.h)

set(source
    djvImagePlayTest.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImagePlayTest
    ${djvGuiIncludeDirs})

add_executable(djvImagePlayTest ${header} ${source} ${mocSource})

target_link_libraries(djvImagePlayTest ${djvGuiLibs})

set_target_properties(djvImagePlayTest PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvImagePlayTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

