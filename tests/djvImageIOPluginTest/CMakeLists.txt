set(header
    djvImageIOPluginTest.h)
set(source
    djvImageIOPluginTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImageIOPluginTest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${CMAKE_SOURCE_DIR}/lib/djvGraphics
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${OPENGL_INCLUDE_DIRS})
add_library(djvImageIOPluginTest ${header} ${source})
target_link_libraries(djvImageIOPluginTest djvTestLib djvGraphics)
set_target_properties(djvImageIOPluginTest PROPERTIES FOLDER tests CXX_STANDARD 11)

