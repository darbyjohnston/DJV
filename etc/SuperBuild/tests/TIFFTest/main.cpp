#include <tiffio.h>

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "TIFF version: " << TIFFGetVersion() << std::endl;
    return 0;
}
