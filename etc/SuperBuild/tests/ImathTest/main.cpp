#include <half.h>

#include <iostream>

int main(int argc, char* argv[])
{
    Imath::half h(1.F);
    std::cout << "half: " << h << std::endl;
    return 0;
}
