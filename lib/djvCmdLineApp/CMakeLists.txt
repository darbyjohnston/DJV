set(header
    Application.h
	GLFWSystem.h)
set(source
    Application.cpp
	GLFWSystem.cpp)

add_library(djvCmdLineApp ${header} ${source})
target_link_libraries(djvCmdLineApp djvAV)
set_target_properties(
    djvCmdLineApp
    PROPERTIES
    FOLDER lib
    CXX_STANDARD 11)

