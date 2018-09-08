set(header
    djvImagePlayExample.h)
set(mocHeader
    djvImagePlayExample.h)
set(source
    djvImagePlayExample.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/examples/djvImagePlayExample
    ${CMAKE_SOURCE_DIR}/lib/djvUI
    ${CMAKE_SOURCE_DIR}/lib/djvGraphics
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${OPENGL_INCLUDE_DIRS})
add_executable(djvImagePlayExample ${header} ${source} ${mocSource})
target_link_libraries(djvImagePlayExample djvUI)
set_target_properties(djvImagePlayExample PROPERTIES FOLDER examples CXX_STANDARD 11)
