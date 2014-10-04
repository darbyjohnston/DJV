set(header)

set(source
    djvImagePlayTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImagePlayTest
    ${djvGuiIncludeDirs})

add_executable(djvImagePlayTest ${header} ${source})

target_link_libraries(djvImagePlayTest ${djvGuiLibs})

set_target_properties(djvImagePlayTest PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvImagePlayTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

