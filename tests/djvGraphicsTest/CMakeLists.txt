set(header
    djvColorProfileTest.h
    djvColorTest.h
    djvColorUtilTest.h
    djvGraphicsContextTest.h
    djvGraphicsTest.h
    djvImageIOTest.h
    djvImageTagsTest.h
    djvImageTest.h
    djvOpenGLImageTest.h
    djvOpenGLTest.h
    djvPixelDataTest.h
    djvPixelDataUtilTest.h
    djvPixelTest.h)
set(mocHeader)
set(source
    djvColorProfileTest.cpp
    djvColorTest.cpp
    djvColorUtilTest.cpp
    djvGraphicsContextTest.cpp
    djvImageIOTest.cpp
    djvImageTagsTest.cpp
    djvImageTest.cpp
    djvOpenGLImageTest.cpp
    djvOpenGLTest.cpp
    djvPixelDataTest.cpp
    djvPixelDataUtilTest.cpp
    djvPixelTest.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvGraphicsTest
    ${CMAKE_SOURCE_DIR}/tests/djvTestLib
    ${CMAKE_SOURCE_DIR}/lib/djvGraphics
    ${CMAKE_SOURCE_DIR}/lib/djvCore
    ${CMAKE_BINARY_DIR}
    ${OPENGL_INCLUDE_DIRS})
add_library(djvGraphicsTest ${header} ${source} ${mocSource})
target_link_libraries(djvGraphicsTest djvTestLib djvGraphics)
set_target_properties(djvGraphicsTest PROPERTIES FOLDER tests CXX_STANDARD 11)

