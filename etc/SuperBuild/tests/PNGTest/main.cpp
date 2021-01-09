#include <png.h>

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "PNG version: " << png_access_version_number() << std::endl;
    return 0;
}
