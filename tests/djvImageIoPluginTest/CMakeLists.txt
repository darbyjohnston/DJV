set(header
    djvImageIoPluginTest.h
    djvImageIoPluginTestExport.h)

set(source
    djvImageIoPluginTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImageIoPluginTest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${djvCoreIncludeDirs})

add_library(djvImageIoPluginTest ${header} ${source})

target_link_libraries(djvImageIoPluginTest djvTestLib ${djvCoreLibs})

set_target_properties(djvImageIoPluginTest PROPERTIES FOLDER tests)

if (djvImageIoPlugins)

    add_dependencies(djvImageIoPluginTest ${djvImageIoPlugins})

endif (djvImageIoPlugins)

