//! References:
//!
//! - http://www.w3.org/Graphics/Color/sRGB.html

#include <iostream>

#include <math.h>

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: srgb (size)" << std::endl;
        
        return 1;
    }

    const int size = atoi(argv[1]);
    
    std::cout << "LUT: 1 " << size << std::endl;
    std::cout << std::endl;
    
    for (int i = 0; i < size; ++i)
    {
        const double input = i / static_cast<double>(size);
        
        double output = 0.0;
        
        if (input < 0.00304)
        {
            output = 12.92 * input;
        }
        else
        {
            output = 1.055 * pow(input, 1.0 / 2.4) - 0.055;
        }
        
        std::cout << "      " << static_cast<int>(output * size) << std::endl;
    }
    
    return 0;
}
