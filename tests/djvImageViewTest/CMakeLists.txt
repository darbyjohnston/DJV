set(header)

set(source
    djvImageViewTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImageViewTest
    ${djvGuiIncludeDirs})

add_executable(djvImageViewTest ${header} ${source})

target_link_libraries(djvImageViewTest ${djvGuiLibs})

set_target_properties(djvImageViewTest PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvImageViewTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

