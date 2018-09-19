set(header
    UITest.h
    UITest.h)    
set(source
    UITest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvUITest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${OPENGL_INCLUDE_DIRS})
add_library(djvUITest ${header} ${source})
target_link_libraries(djvUITest djvTestLib djvUI)
set_target_properties(djvUITest PROPERTIES FOLDER tests CXX_STANDARD 11)

