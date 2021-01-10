#include <iostream>

#include <config.h>
#include <unistd.h>
#include <util/exec.h>

int main(int argc, char* argv[])
{
    std::cout << std::endl;
    std::cout << argv[0] << " (SPP Toolchain) " << SCC_VER << " for " << SCC_OS_NAME << " (" << SCC_OS_ARCH << ")" << std::endl << std::endl;

    std::cout << "- Current directory: " << getcwd(nullptr, 0) << std::endl;

    std::vector<std::string> args;
    for(size_t s = 1; s < argc; s++)
        args.push_back(argv[s]);

    util::spawn_process_and_wait("../compiler/" + (std::string)SCC_PREFIX + "cc", args);
    return 0;
}
