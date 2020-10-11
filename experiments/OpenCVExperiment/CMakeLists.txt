set(header
    Application.h
    ImageWidget.h
    MainWindow.h)
set(source
    Application.cpp
    ImageWidget.cpp
    MainWindow.cpp
    main.cpp)

add_executable(OpenCVExperiment ${header} ${source})
target_link_libraries(OpenCVExperiment djvDesktopApp djvUIComponents OpenCV)
set_target_properties(
    OpenCVExperiment
    PROPERTIES
    FOLDER experiments
    CXX_STANDARD 11)
