#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "opcodes.h"

namespace assembler
{

class RuntimeData
{
public:
    struct Section
    {
        std::string name;
        std::vector<std::shared_ptr<Opcode>> instructions;
        std::map<std::string, size_t> labels;
    };

    struct Symbol
    {
        std::string name;
        Section* section = nullptr;

        enum class Type
        {
            Variable,
            Function,
            Invalid
        } type = Type::Invalid;
    };

    void switch_section(std::string name);
    bool add_symbol(std::string name);
    bool set_symbol_type(std::string name, Symbol::Type type);
    const Section* current_section() const { return m_current_section; }
    Section* current_section() { return m_current_section; }

    void display() const;

private:
    std::map<std::string, Section> m_sections;
    std::map<std::string, Symbol> m_symbols;
    Section* m_current_section = nullptr;
};

}
