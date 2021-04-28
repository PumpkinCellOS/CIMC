#include "runtimedata.h"

#include <iostream>

namespace assembler
{

void RuntimeData::switch_section(std::string name)
{
    auto it = m_sections.find(name);
    if(it == m_sections.end())
    {
        auto it2 = m_sections.insert(std::make_pair(name, Section{name}));
        m_current_section = &it2.first->second;
        return;
    }
    m_current_section = &it->second;
}

bool RuntimeData::add_symbol(std::string name)
{
    auto it = m_symbols.find(name);
    if(it != m_symbols.end())
        return false;
    m_symbols.insert(std::make_pair(name, Symbol{name, m_current_section}));
    return true;
}

bool RuntimeData::set_symbol_type(std::string name, Symbol::Type type)
{
    auto it = m_symbols.find(name);
    if(it == m_symbols.end())
        return false;
    it->second.type = type;
    return true;
}

void RuntimeData::display() const
{
    auto hex_display = [](size_t offset, std::string data)
    {
        std::cout << "      " << offset << ": ";
        for(unsigned c: data)
            std::cout << std::hex << (unsigned)(c & 0xFF) << " ";
    };

    std::cout << "Sections:" << std::endl;
    for(auto& it: m_sections)
    {
        std::cout << "  ." << it.first << std::endl;
        size_t offset = 0;
        std::cout << "    Dump:" << std::endl;
        for(auto& op: it.second.instructions)
        {
            hex_display(offset, op->payload(*this));
            std::cout << op->display() << std::endl;
            offset++;
        }
        std::cout << "    Labels:" << std::endl;
        for(auto& label: it.second.labels)
        {
            std::cout << "      " << label.first << ": " << label.second << std::endl;
            offset++;
        }
    }

    std::cout << "Symbols:" << std::endl;
    for(auto& it: m_symbols)
    {
        auto section = it.second.section;
        std::cout << "  " << it.first   << " type " << (int)it.second.type
                  << " section ." << (section ? section->name : "<None>") << std::endl;
    }
}

}
