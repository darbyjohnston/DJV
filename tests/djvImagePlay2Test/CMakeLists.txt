set(header
    djvImagePlay2TestApplication.h
    djvImagePlay2TestImage.h
    djvImagePlay2TestWidget.h)

set(source
    djvImagePlay2TestApplication.cpp
    djvImagePlay2TestImage.cpp
    djvImagePlay2TestWidget.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImagePlay2Test
    ${djvGuiIncludeDirs})

add_executable(djvImagePlay2Test ${header} ${source})

target_link_libraries(djvImagePlay2Test ${djvGuiLibs})

set_target_properties(djvImagePlay2Test PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvImagePlay2Test ${djvImageIoPlugins})

endif (djvImageIoPlugins)

