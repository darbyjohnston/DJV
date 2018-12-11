set(source
    main.cpp)

add_executable(djv_convert ${header} ${source})
target_link_libraries(djv_convert djvAV)
set_target_properties(
    djv_convert
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)
