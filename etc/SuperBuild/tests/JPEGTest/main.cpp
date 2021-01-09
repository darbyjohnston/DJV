#include <stdio.h>

#include <jpeglib.h>

#include <iostream>

int main(int argc, char* argv[])
{
    struct jpeg_error_mgr errorMgr;
    jpeg_std_error(&errorMgr);
    return 0;
}
