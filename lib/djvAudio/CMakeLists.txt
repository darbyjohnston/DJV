set(header
    AudioData.h
    AudioDataInline.h
    AudioFunc.h
    AudioFuncInline.h
    AudioSystem.h
    AudioSystemFunc.h
    Audio.h)
set(source
    AudioData.cpp
    AudioFunc.cpp
    AudioSystem.cpp
    AudioSystemFunc.cpp)

add_library(djvAudio ${header} ${source})
set(LIBRARIES
    djvSystem
    RtAudio)
target_link_libraries(djvAudio ${LIBRARIES})
set_target_properties(
    djvAudio
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvAudio
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvAudio)

