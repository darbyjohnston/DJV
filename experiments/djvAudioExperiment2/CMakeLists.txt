find_package(OpenAL REQUIRED)
find_package(dr_libs REQUIRED)

set(header
    djvAudioExperiment2.h)
set(mocHeader
    djvAudioExperiment2.h)
set(source
    djvAudioExperiment2.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})
add_executable(djvAudioExperiment2 ${header} ${source} ${mocSource} ${qmSource})
target_link_libraries(djvAudioExperiment2 djvCore OpenAL)
set_target_properties(djvAudioExperiment2 PROPERTIES FOLDER experiments CXX_STANDARD 11)
