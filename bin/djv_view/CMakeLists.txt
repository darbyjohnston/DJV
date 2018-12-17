set(source main.cpp)

add_executable(djv_view ${header} ${source})
target_link_libraries(djv_view djvViewLib)
set_target_properties(
    djv_view
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)

install(
    TARGETS djv_view
    RUNTIME DESTINATION ${DJV_INSTALL_BIN})
