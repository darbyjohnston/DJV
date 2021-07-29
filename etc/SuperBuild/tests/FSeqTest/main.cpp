#include <fseq.h>

#include <iostream>

int main(int argc, char* argv[])
{
    FSeqFileName components;
    fseqFileNameInit(&components);
    fseqFileNameSplit("/tmp/render.1.exr", &components, FSEQ_STRING_LEN);
    std::cout << "path: " << components.path << std::endl;
    std::cout << "base: " << components.base << std::endl;
    std::cout << "number: " << components.number << std::endl;
    std::cout << "extension: " << components.extension << std::endl;
    fseqFileNameDel(&components);
    return 0;
}
