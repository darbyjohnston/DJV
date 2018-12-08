set(source
    main.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

add_executable(djv_ls ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djv_ls djvAV)
set_target_properties(
    djv_ls
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)

