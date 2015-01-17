set(header
    djvGuiTestExport.h)

set(source
    djvGuiTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvGuiTest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${djvGuiIncludeDirs})

add_library(djvGuiTest ${header} ${source})

target_link_libraries(djvGuiTest djvTestLib ${djvGuiLibs})

set_target_properties(djvGuiTest PROPERTIES FOLDER tests)

