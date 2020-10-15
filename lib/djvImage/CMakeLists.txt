set(header
    ColorFunc.h
    Color.h
    ColorInline.h
    Data.h
    DataFunc.h
    DataInline.h
    Info.h
    InfoFunc.h
    InfoInline.h
    Namespace.h
    Tags.h
    TagsInline.h
    Type.h
    TypeFunc.h
    TypeFuncInline.h
    TypeInline.h)
set(source
    Color.cpp
    ColorFunc.cpp
    Data.cpp
    DataFunc.cpp
    Info.cpp
    InfoFunc.cpp
    Tags.cpp
    TypeFunc.cpp)

add_library(djvImage ${header} ${source})
target_compile_definitions(djvImage PUBLIC IlmImf_FOUND)
set(LIBRARIES
    djvSystem
    IlmBase::Imath
    IlmBase::IlmThread)
target_link_libraries(djvImage ${LIBRARIES})
set_target_properties(
    djvImage
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvImage
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvImage)

