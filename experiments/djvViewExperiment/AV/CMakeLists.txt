set(header
    Context.h)
set(mocHeader
    Context.h)
set(source
    Context.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
add_library(djvViewExperimentAV ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvViewExperimentAV djvViewExperimentCore)
set_target_properties(
    djvViewExperimentAV
    PROPERTIES
    FOLDER experiments/djvViewExperiment
    CXX_STANDARD 11)

