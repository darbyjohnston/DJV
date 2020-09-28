set(header)
set(source main.cpp)

add_executable(djv_test_pattern ${header} ${source})
target_link_libraries(djv_test_pattern djvRender2D djvCmdLineApp)
set_target_properties(
    djv_test_pattern
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)

install(
    TARGETS djv_test_pattern
    RUNTIME DESTINATION ${DJV_INSTALL_BIN})
