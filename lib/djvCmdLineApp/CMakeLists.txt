set(header
    Application.h
    CmdLineApp.h)
set(source
    Application.cpp)

add_library(djvCmdLineApp ${header} ${source})
target_link_libraries(djvCmdLineApp djvScene)
set_target_properties(
    djvCmdLineApp
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

