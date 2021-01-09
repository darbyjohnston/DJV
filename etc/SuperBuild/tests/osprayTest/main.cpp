#include <ospray/ospray.h>

#include <iostream>

int main(int argc, char* argv[])
{
    if (ospInit() != OSP_NO_ERROR)
    {
        std::cout << "Could not initialize ospray" << std::endl;
        return 0;
    }
    ospShutdown();
    return 0;
}
