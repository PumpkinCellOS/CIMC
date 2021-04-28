#include "builder.h"

namespace assembler
{

bool Builder::build()
{
    std::cout << "Building semantic tree..." << std::endl;

    for(auto& op: m_block->members)
    {
        op->execute(m_data);
    }

    return true;
}

void Builder::display()
{
    std::cout << "Builder dump:" << std::endl;
    m_data.display();
}

}
