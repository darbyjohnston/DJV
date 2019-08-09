set(header jpeg.h)
set(source gallery.cpp jpeg.cpp)

add_executable(gallery ${header} ${source})
target_link_libraries(gallery djvDesktopApp curl)
set_target_properties(
    gallery
    PROPERTIES
    FOLDER experiments
    CXX_STANDARD 11)
