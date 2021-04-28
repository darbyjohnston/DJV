set(header
    AudioSystem.h
    Data.h
    DataInline.h
    Info.h
    InfoInline.h
    Type.h
    TypeInline.h
    Namespace.h)
set(source
    AudioSystem.cpp
    Data.cpp
    Info.cpp
    Type.cpp)

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

