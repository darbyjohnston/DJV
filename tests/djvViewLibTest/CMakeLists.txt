set(header
    ViewLibTest.h)
set(source
    ViewLibTest.cpp)

include_directories(${OPENGL_INCLUDE_DIRS})
add_library(djvViewLibTest ${header} ${source})
target_link_libraries(djvViewLibTest djvTestLib djvViewLib)
set_target_properties(djvViewLibTest PROPERTIES FOLDER tests CXX_STANDARD 11)

