set(header
    djvUITest.h
    djvUITest.h
    djvPixmapUtilTest.h)    
set(source
    djvUITest.cpp
    djvPixmapUtilTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvUITest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${CMAKE_SOURCE_DIR}/lib/djvUI
    ${CMAKE_SOURCE_DIR}/lib/djvGraphics
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${OPENGL_INCLUDE_DIRS})
add_library(djvUITest ${header} ${source})
target_link_libraries(djvUITest djvTestLib djvUI)
set_target_properties(djvUITest PROPERTIES FOLDER tests CXX_STANDARD 11)

