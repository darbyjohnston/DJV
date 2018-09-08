set(header
    djvImagePlayTest.h)
set(mocHeader
    djvImagePlayTest.h)
set(source
    djvImagePlayTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImagePlayTest
    ${CMAKE_SOURCE_DIR}/lib/djvUI
    ${CMAKE_SOURCE_DIR}/lib/djvGraphics
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${OPENGL_INCLUDE_DIRS})
add_executable(djvImagePlayTest ${header} ${source} ${mocSource})
target_link_libraries(djvImagePlayTest djvUI)
set_target_properties(djvImagePlayTest PROPERTIES FOLDER tests CXX_STANDARD 11)

