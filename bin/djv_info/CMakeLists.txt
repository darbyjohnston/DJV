set(source
    main.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

add_executable(djv_info ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djv_info djvAV)
set_target_properties(
    djv_info
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)
