set(header
    djvGuiTest.h
    djvGuiTest.h
    djvPixmapUtilTest.h)    

set(source
    djvGuiTest.cpp
    djvPixmapUtilTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvGuiTest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${djvGuiIncludeDirs})

add_library(djvGuiTest ${header} ${source})

target_link_libraries(djvGuiTest djvTestLib ${djvGuiLibs})

set_target_properties(djvGuiTest PROPERTIES FOLDER tests CXX_STANDARD 11)

