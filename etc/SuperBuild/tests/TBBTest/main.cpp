#include <tbb/tbb_stddef.h>

#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "TBB version: " << tbb::TBB_runtime_interface_version() << std::endl;
    return 0;
}
