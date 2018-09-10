set(header
    djvImageViewExample.h)
set(mocHeader
    djvImageViewExample.h)
set(source
    djvImageViewExample.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/examples/djvImageViewExample
    ${OPENGL_INCLUDE_DIRS})
add_executable(djvImageViewExample ${header} ${source} ${mocSource})
target_link_libraries(djvImageViewExample djvUI)
set_target_properties(djvImageViewExample PROPERTIES FOLDER examples CXX_STANDARD 11)
