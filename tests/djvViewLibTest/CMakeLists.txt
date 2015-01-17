set(header
    djvViewLibTestExport.h)

set(source
    djvViewLibTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvViewLibTest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${CMAKE_SOURCE_DIR}/lib/djvViewLib
    ${djvGuiIncludeDirs})

add_library(djvViewLibTest ${header} ${source})

target_link_libraries(djvViewLibTest djvTestLib ${djvViewLibLibs})

set_target_properties(djvViewLibTest PROPERTIES FOLDER tests)

