#include <ImfVersion.h>

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Is magic: " << Imf::isImfMagic(reinterpret_cast<const char*>(&Imf::MAGIC)) << std::endl;
    return 0;
}
