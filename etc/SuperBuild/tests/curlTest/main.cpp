#include <curl/curl.h>

#include <iostream>

int main(int argc, char* argv[])
{
    if (0 == curl_global_init(CURL_GLOBAL_NOTHING))
    {
        curl_global_cleanup();
    }
    return 0;
}
