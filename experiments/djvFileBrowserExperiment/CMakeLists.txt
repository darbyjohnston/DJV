set(header
    AbstractWorker.h
    ActionData.h
    Actions.h
    Application.h
    ContactSheetDelegate.h
    Controls.h
    Dir.h
    Info.h
    Item.h
    Model.h
    Thumbnail.h
    Util.h
    View.h
    Widget.h)
set(mocHeader
    AbstractWorker.h
    ActionData.h
    Actions.h
    Application.h
    ContactSheetDelegate.h
    Controls.h
    Dir.h
    Info.h
    Item.h
    Model.h
    Thumbnail.h
    Util.h
    View.h
    Widget.h)
set(source
    AbstractWorker.cpp
    ActionData.cpp
    Actions.cpp
    Application.cpp
    ContactSheetDelegate.cpp
    Controls.cpp
    Dir.cpp
    Info.cpp
    Item.cpp
    Model.cpp
    Thumbnail.cpp
    Util.cpp
    View.cpp
    Widget.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/experiments/djvFileBrowserExperiment
    ${OPENGL_INCLUDE_DIRS})
add_executable(djvFileBrowserExperiment ${header} ${source} ${mocSource})
target_link_libraries(djvFileBrowserExperiment djvUI)
set_target_properties(djvFileBrowserExperiment PROPERTIES FOLDER experiments CXX_STANDARD 11)

