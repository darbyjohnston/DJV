#include <GLFW/glfw3.h>

#include <iostream>

int main(int argc, char* argv[])
{
    if (!glfwInit())
    {
        std::cout << "Could not initialize GLFW" << std::endl;
        return 0;
    }
    std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;
    glfwTerminate();
    return 0;
}
