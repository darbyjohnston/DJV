set(header
    Color.h
    ColorInline.h
    Data.h
    DataInline.h
    Info.h
    InfoInline.h
    Tags.h
    TagsInline.h
    Type.h
    TypeInline.h)
set(source
    Color.cpp
    Data.cpp
    Info.cpp
    Tags.cpp
    Type.cpp)

add_library(djvImage ${header} ${source})
target_compile_definitions(djvImage PUBLIC IlmImf_FOUND)
set(LIBRARIES
    djvSystem
    IlmBase)
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

