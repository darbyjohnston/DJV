set(source main.cpp)

add_executable(djv ${header} ${source})
target_link_libraries(djv djvViewApp)
set_target_properties(
    djv
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)

install(
    TARGETS djv
    RUNTIME DESTINATION ${DJV_INSTALL_BIN})
