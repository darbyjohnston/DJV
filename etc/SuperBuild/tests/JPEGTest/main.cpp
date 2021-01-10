#include <stdio.h>

extern "C"
{
#include <jpeglib.h>

} // extern "C"

#include <iostream>

int main(int argc, char* argv[])
{
    struct jpeg_error_mgr errorMgr;
    jpeg_std_error(&errorMgr);
    return 0;
}
