set(header
    ColorFunc.h
    Color.h
    ColorInline.h
    ImageData.h
    ImageDataFunc.h
    ImageDataInline.h
    Image.h
    ImageFunc.h
    Pixel.h
    PixelFunc.h
    PixelFuncInline.h
    PixelInline.h
    Tags.h)
set(source
    Color.cpp
    ColorFunc.cpp
    Image.cpp
    ImageData.cpp
    ImageDataFunc.cpp
    ImageFunc.cpp
    PixelFunc.cpp
    Tags.cpp)

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

