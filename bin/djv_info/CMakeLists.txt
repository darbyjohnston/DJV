set(source
    main.cpp)

add_executable(djv_info ${header} ${source})
target_link_libraries(djv_info djvAV)
set_target_properties(
    djv_info
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)
