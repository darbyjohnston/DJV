set(source GLFWTest.cpp)

add_executable(GLFWTest ${header} ${source})
target_link_libraries(GLFWTest
    GLFW
    Threads::Threads
    ${CMAKE_DL_LIBS})
set_target_properties(
    GLFWTest
    PROPERTIES
    FOLDER tests
    CXX_STANDARD 11)
