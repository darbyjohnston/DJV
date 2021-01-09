#include <openvkl/openvkl.h>

#include <iostream>

int main(int argc, char* argv[])
{
    if (vklLoadModule("ispc_driver") != VKL_NO_ERROR)
    {
        std::cout << "Cannot load OpenVKL module";
        return 0;
    }
    vklShutdown();
    return 0;
}
