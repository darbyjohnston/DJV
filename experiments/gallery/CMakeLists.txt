find_package(curl REQUIRED)

set(source gallery.cpp)

add_executable(gallery ${header} ${source})
target_link_libraries(gallery djvDesktopApp curl)
set_target_properties(
    gallery
    PROPERTIES
    FOLDER experiments
    CXX_STANDARD 11)
