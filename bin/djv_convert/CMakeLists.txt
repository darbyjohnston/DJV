set(source main.cpp)

add_executable(djv_convert ${header} ${source})
target_link_libraries(djv_convert djvCmdLineApp)
set_target_properties(
    djv_convert
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)

install(
    TARGETS djv_convert
    RUNTIME DESTINATION ${DJV_INSTALL_BIN})