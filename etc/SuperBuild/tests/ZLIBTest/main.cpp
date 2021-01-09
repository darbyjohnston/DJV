#include <zlib.h>

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "ZLIB version: " << zlibVersion() << std::endl;
    return 0;
}
