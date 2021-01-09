#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

int main(int argc, char* argv[])
{
    FT_Library ftLibrary = nullptr;
    FT_Error ftError = FT_Init_FreeType(&ftLibrary);
    if (ftError)
    {
        std::cout << "FreeType could not be initialized" << std::endl;
        return 1;
    }
    int versionMajor = 0;
    int versionMinor = 0;
    int versionPatch = 0;
    FT_Library_Version(ftLibrary, &versionMajor, &versionMinor, &versionPatch);
    std::cout << "FreeType version: " << versionMajor << "." << versionMinor << "." << versionPatch << std::endl;
    return 0;
}
