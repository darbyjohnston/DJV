set(source
    main.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

add_executable(djv_view ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djv_view djvViewLib)
set_target_properties(
    djv_view
    PROPERTIES
    FOLDER bin
    CXX_STANDARD 11)
