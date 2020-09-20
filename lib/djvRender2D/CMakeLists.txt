set(header
    DataFunc.h
    Data.h
    DataInline.h
    Enum.h
    EnumFunc.h
    FontSystem.h
    FontSystemInline.h
    Render.h
    Render2D.h
    Render2DSystem.h
    RenderInline.h
    RenderPrivate.h)
set(source
    Data.cpp
    DataFunc.cpp
    EnumFunc.cpp
    FontSystem.cpp
    Render.cpp
    Render2DSystem.cpp
    RenderPrivate.cpp)

add_library(djvRender2D ${header} ${source})
set(LIBRARIES
    djvOCIO
    djvGL
    FreeType)
target_link_libraries(djvRender2D ${LIBRARIES})
set_target_properties(
    djvRender2D
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvRender2D
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvRender2D)

