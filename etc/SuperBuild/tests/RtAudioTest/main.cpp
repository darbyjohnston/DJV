#include <rtaudio/RtAudio.h>

#include <iostream>

int main(int argc, char* argv[])
{
    RtAudio audio;
    const unsigned int devices = audio.getDeviceCount();
    for (unsigned int i = 0; i < devices; ++i)
    {
        const auto info = audio.getDeviceInfo(i);
        if (info.probed)
        {
            std::cout << "Device: " << info.name << std::endl;
        }
    }
    return 0;
}
