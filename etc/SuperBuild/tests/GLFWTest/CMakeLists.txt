find_package(GLFW REQUIRED)

set(source main.cpp)

add_executable(GLFWTest ${header} ${source})
target_link_libraries(GLFWTest GLFW)

add_test(GLFWTest GLFWTest)
