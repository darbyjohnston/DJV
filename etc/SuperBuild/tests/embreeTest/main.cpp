#include <embree3/rtcore.h>

#include <iostream>

int main(int argc, char* argv[])
{
    RTCDevice device = rtcNewDevice(NULL);
    if (!device)
    {
        std::cout << "Could not create embree device" << std::endl;
        return 0;
    }
    rtcReleaseDevice(device);
    return 0;
}
