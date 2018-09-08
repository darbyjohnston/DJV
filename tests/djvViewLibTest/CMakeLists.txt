set(header
    djvViewLibTest.h)
set(source
    djvViewLibTest.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/lib/djvViewLib
    ${CMAKE_SOURCE_DIR}/lib/djvUI
    ${CMAKE_SOURCE_DIR}/lib/djvGraphics
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${OPENGL_INCLUDE_DIRS})
add_library(djvViewLibTest ${header} ${source})
target_link_libraries(djvViewLibTest djvTestLib djvViewLib)
set_target_properties(djvViewLibTest PROPERTIES FOLDER tests CXX_STANDARD 11)

