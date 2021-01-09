#include <iostream>

#include <config.h>
#include <util/exec.h>

int main(int argc, char* argv[])
{
    std::cout << std::endl;
    std::cout << argv[0] << " (spp Compiler) " << SCC_VER << " for " << SCC_OS_NAME << " (" << SCC_OS_ARCH << ")" << std::endl << std::endl;
    
    util::spawn_process_and_wait("compiler/" + (std::string)SCC_PREFIX + "c", {"cc-test"});
    util::spawn_process_and_wait("linker/" + (std::string)SCC_PREFIX + "ld", {"ld-test"});
    return 0;
}
