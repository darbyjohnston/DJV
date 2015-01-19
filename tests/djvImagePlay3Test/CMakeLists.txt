set(header
    djvImagePlay3Test.h)

set(mocHeader
    djvImagePlay3Test.h)

set(source
    djvImagePlay3Test.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImagePlay3Test
    ${djvGuiIncludeDirs})

add_executable(djvImagePlay3Test ${header} ${source} ${mocSource})

target_link_libraries(djvImagePlay3Test ${djvGuiLibs})

set_target_properties(djvImagePlay3Test PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvImagePlay3Test ${djvImageIoPlugins})

endif (djvImageIoPlugins)

