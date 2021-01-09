#include <blosc.h>

int main(int argc, char* argv[])
{
    blosc_init();
    blosc_destroy();
    return 0;
}
