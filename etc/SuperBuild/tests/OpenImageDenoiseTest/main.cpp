#include <OpenImageDenoise/oidn.h>

#include <iostream>

int main(int argc, char* argv[])
{
    OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
    if (!device)
    {
        std::cout << "Could not initialize OpenImageDenoise device";
        return 0;
    }
    oidnReleaseDevice(device);
    return 0;
}
