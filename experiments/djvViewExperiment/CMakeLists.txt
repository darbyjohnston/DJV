include_directories(
    ${CMAKE_SOURCE_DIR}/experiments/djvViewExperiment)

add_subdirectory(Core)
add_subdirectory(AV)
add_subdirectory(UI)
add_subdirectory(ViewLib)

set(source
    main.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
add_executable(djvViewExperiment ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvViewExperiment djvViewExperimentViewLib)
set_target_properties(
    djvViewExperiment
    PROPERTIES
    FOLDER experiments
    CXX_STANDARD 11)
