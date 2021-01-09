#include <iostream>

#include <config.h>

int main(int argc, char* argv[])
{
    std::cout << argv[0] << " (spp Compiler) " << SCC_VER << " for " << SCC_OS_NAME << " (" << SCC_OS_ARCH << ")" << std::endl;
    return 0;
}
