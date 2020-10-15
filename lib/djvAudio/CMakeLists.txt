set(header
    AudioSystem.h
    AudioSystemFunc.h
    Data.h
    DataInline.h
    DataFunc.h
    DataFuncInline.h
    Info.h
    InfoInline.h
    TypeFunc.h
    TypeFuncInline.h
    Type.h
    Namespace.h)
set(source
    AudioSystem.cpp
    AudioSystemFunc.cpp
    Data.cpp
    DataFunc.cpp
    Info.cpp
    TypeFunc.cpp)

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

