#pragma once

#include <map>
#include <string>

namespace assembler
{

class RuntimeData
{
public:
    struct Section
    {
        std::string name;
        size_t offset = 0;
    };

    struct Symbol
    {
        std::string name;
        size_t offset = 0;
        Section* section = nullptr;

        enum class Type
        {
            Variable,
            Function,
            Invalid
        } type = Type::Invalid;
    };

    void advance() { m_current_offset++; }
    void switch_section(std::string name);
    bool add_symbol(std::string name);
    bool set_symbol_type(std::string name, Symbol::Type type);
    const Section* current_section() const { return m_current_section; }

    void display() const;

private:
    std::map<std::string, Section> m_sections;
    std::map<std::string, Symbol> m_symbols;
    Section* m_current_section = nullptr;

    size_t m_current_offset = 0;
};

}
