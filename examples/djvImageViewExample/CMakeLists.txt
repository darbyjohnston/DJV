set(header
    djvImageViewExample.h)
set(mocHeader
    djvImageViewExample.h)
set(source
    djvImageViewExample.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/examples/djvImageViewExample
    ${CMAKE_SOURCE_DIR}/lib/djvUI
    ${CMAKE_SOURCE_DIR}/lib/djvGraphics
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${OPENGL_INCLUDE_DIRS})
add_executable(djvImageViewExample ${header} ${source} ${mocSource})
target_link_libraries(djvImageViewExample djvUI)
set_target_properties(djvImageViewExample PROPERTIES FOLDER examples CXX_STANDARD 11)
