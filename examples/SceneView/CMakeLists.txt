set(header
    Application.h
    CameraTool.h
    ITool.h
    InfoTool.h
    MainWindow.h
    RenderTool.h)
set(source
    Application.cpp
    CameraTool.cpp
    ITool.cpp
    InfoTool.cpp
    MainWindow.cpp
    RenderTool.cpp
    main.cpp)

add_executable(SceneView ${header} ${source})
target_link_libraries(SceneView djvDesktopApp djvUIComponents)
set_target_properties(
    SceneView
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)
