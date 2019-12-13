set(header
    Application.h
    CameraWidget.h
    ISettingsWidget.h
    InfoWidget.h
    MainWindow.h
    RenderWidget.h
    SettingsWidget.h)
set(source
    Application.cpp
    CameraWidget.cpp
    ISettingsWidget.cpp
    InfoWidget.cpp
    MainWindow.cpp
    RenderWidget.cpp
    SettingsWidget.cpp
    main.cpp)

add_executable(SceneViewExample ${header} ${source})
target_link_libraries(SceneViewExample djvDesktopApp djvUIComponents)
set_target_properties(
    SceneViewExample
    PROPERTIES
    FOLDER examples
    CXX_STANDARD 11)
