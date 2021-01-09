#include <glad.h>

#include <iostream>

int main(int argc, char* argv[])
{
    if (!gladLoadGL())
    {
        std::cout << "Could not load glad" << std::endl;
        return 0;
    }
    return 0;
}
