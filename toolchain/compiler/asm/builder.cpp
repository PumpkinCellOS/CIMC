#include "builder.h"

namespace assembler
{

bool Builder::build()
{
    std::cout << "Building semantic tree..." << std::endl;

    for(auto& op: m_block->members)
    {
        if(!op->execute(m_data))
        {
            return false;
        }
    }

    return true;
}

void Builder::display()
{
    std::cout << "Builder dump:" << std::endl;
    m_data.display();
}

}
