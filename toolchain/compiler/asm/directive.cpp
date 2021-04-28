#include "directive.h"

#include "runtimedata.h"

namespace assembler
{

std::shared_ptr<DirectiveOperation> DirectiveOperation::create(const Directive& directive)
{
    if(directive.mnemonic == "globl")
        return std::make_shared<directives::Globl>(directive);
    else if(directive.mnemonic == "type")
        return std::make_shared<directives::Type>(directive);
    else if(directive.mnemonic == "section")
        return std::make_shared<directives::Section>(directive);
    return nullptr;
}

namespace directives
{

bool Globl::execute(RuntimeData& data) const
{
    std::cout << "Globl::execute " << std::endl;
    if(m_directive.members.size() != 1)
        BUILDER_ERROR(&m_directive, "invalid argument count for .globl, need 1");

    auto name = m_directive.members[0];
    if(name->type != Operand::Type::Name)
        BUILDER_ERROR(&m_directive, "invalid operand type, need name");

    data.add_symbol(name->value);
    return true;
}

bool Type::execute(RuntimeData& data) const
{
    std::cout << "Type::execute " << std::endl;
    if(m_directive.members.size() != 2)
        BUILDER_ERROR(&m_directive, "invalid argument count for .type, need 2");

    auto name = m_directive.members[0];
    if(name->type != Operand::Type::Name)
        BUILDER_ERROR(&m_directive, "invalid operand 1 type, need name");

    auto type = m_directive.members[1];
    if(type->type != Operand::Type::Name)
        BUILDER_ERROR(&m_directive, "invalid operand 2 type, need name");

    RuntimeData::Symbol::Type symbol_type;
    if(type->value == "function")
        symbol_type = RuntimeData::Symbol::Type::Function;
    else if(type->value == "variable")
        symbol_type = RuntimeData::Symbol::Type::Variable;
    else
        BUILDER_ERROR(&m_directive, "invalid symbol type: " + type->value);

    data.set_symbol_type(name->value, symbol_type);
    return true;
}

bool Section::execute(RuntimeData& data) const
{
    std::cout << "Section::execute " << std::endl;
    if(m_directive.members.size() != 1)
        BUILDER_ERROR(&m_directive, "invalid argument count for .type, need 2");

    auto name = std::dynamic_pointer_cast<SectionOperand>(m_directive.members[0]);
    if(!name)
        BUILDER_ERROR(&m_directive, "invalid operand 1 type, need SectionOperand");

    data.switch_section(name->value);
    return true;
}

}

}
