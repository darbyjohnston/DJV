#include <glm/glm.hpp>
#include <glm/vec2.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    glm::vec2 x(1.F, 2.F);
    glm::vec2 y(3.F, 4.F);
    std::cout << "x * y: " << glm::length(x * y) << std::endl;
    return 0;
}
