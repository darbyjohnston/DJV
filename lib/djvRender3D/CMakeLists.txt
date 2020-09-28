set(header
    Camera.h
    CameraInline.h
    Light.h
    LightInline.h
    Material.h
    MaterialInline.h
    Namespace.h
    Render.h)
set(source
    Camera.cpp
    Light.cpp
    Material.cpp
    Render.cpp)

add_library(djvRender3D ${header} ${source})
set(LIBRARIES
    djvOCIO
    djvGL)
target_link_libraries(djvRender3D ${LIBRARIES})
set_target_properties(
    djvRender3D
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

#install(
#    TARGETS djvRender3D
#    RUNTIME DESTINATION bin
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib)
#install(FILES ${header} DESTINATION include/djvRender3D)

