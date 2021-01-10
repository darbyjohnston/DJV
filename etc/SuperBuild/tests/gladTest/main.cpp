#include <glad.h>

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "major: " << GLVersion.major << std::endl;
    std::cout << "minor: " << GLVersion.minor << std::endl;
    return 0;
}
