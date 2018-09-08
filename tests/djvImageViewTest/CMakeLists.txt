set(header
    djvImageViewTest.h)
set(mocHeader
    djvImageViewTest.h)
set(source
    djvImageViewTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvImageViewTest
    ${CMAKE_SOURCE_DIR}/lib/djvUI
    ${CMAKE_SOURCE_DIR}/lib/djvGraphics
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${OPENGL_INCLUDE_DIRS})
add_executable(djvImageViewTest ${header} ${source} ${mocSource})
target_link_libraries(djvImageViewTest djvUI)
set_target_properties(djvImageViewTest PROPERTIES FOLDER tests CXX_STANDARD 11)

