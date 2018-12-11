set(source
    main.cpp)

add_executable(djv_ls ${header} ${source})
target_link_libraries(djv_ls djvAV)
set_target_properties(
    djv_ls
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)

