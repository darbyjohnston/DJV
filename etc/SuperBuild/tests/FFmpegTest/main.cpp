#include <libavutil/timecode.h>

#include <iostream>

int main(int argc, char* argv[])
{
    AVTimecode tc;
    tc.start = 0;
    tc.flags = 0;
    tc.rate.num = 1;
    tc.rate.den = 24;
    tc.fps = 24;
    char buf[AV_TIMECODE_STR_SIZE];
    std::cout << "timecode: " << av_timecode_make_string(&tc, buf, 0) << std::endl;
    return 0;
}
