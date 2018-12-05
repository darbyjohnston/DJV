set(source
    main.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

add_executable(djv_convert ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djv_convert djvAV)
set_target_properties(
    djv_convert
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)
