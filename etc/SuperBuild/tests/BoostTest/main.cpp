#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    std::string s("a b c");
    std::vector<std::string> r;
    boost::split(r, s, boost::is_any_of(" "));
    for (const auto& i : r)
    {
        std::cout << "split: " << i << std::endl;
    }
    return 0;
}
