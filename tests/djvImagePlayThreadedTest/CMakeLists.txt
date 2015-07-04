set(header
    djvImagePlayThreadedTest.h)

set(mocHeader
    djvImagePlayThreadedTest.h)

set(source
    djvImagePlayThreadedTest.cpp)

if (Qt5Core_DIR)
    QT5_WRAP_CPP(mocSource ${mocHeader})
else()
    QT4_WRAP_CPP(mocSource ${mocHeader})
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImagePlayThreadedTest
    ${djvGuiIncludeDirs})

add_executable(djvImagePlayThreadedTest ${header} ${source} ${mocSource})

target_link_libraries(djvImagePlayThreadedTest ${djvGuiLibs})

set_target_properties(djvImagePlayThreadedTest PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvImagePlayThreadedTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

