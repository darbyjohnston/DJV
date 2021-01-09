#include <openvdb/openvdb.h>

int main(int argc, char* argv[])
{
    openvdb::initialize();
    openvdb::uninitialize();
    return 0;
}
