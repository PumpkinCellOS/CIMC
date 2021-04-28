#include "ast.h"

#include "directive.h"
#include "instruction.h"
#include "runtimedata.h"
#include "../cpp_compiler.h"

namespace assembler
{

bool Instruction::execute(RuntimeData& data) const
{
    //std::cout << "Instruction::execute " << display() << std::endl;
    return InstructionOperation(*this).execute(data);
}

bool Directive::execute(RuntimeData& data) const
{
    //std::cout << "Directive::execute " << display() << std::endl;
    auto operation = DirectiveOperation::create(*this);
    if(!operation)
        BUILDER_ERROR(this, "invalid directive");
    return operation->execute(data);
}

bool Assignment::execute(RuntimeData&) const
{
    //std::cout << "Assignment::execute " << display() << std::endl;
    return true;
}

bool Label::execute(RuntimeData& data) const
{
    std::cout << "Label::execute " << display() << std::endl;
    auto current_section = data.current_section();
    if(current_section)
        current_section->labels.insert(std::make_pair(name, current_section->instructions.size()));
    else
        BUILDER_ERROR(this, "labels not allowed outside section");
    return true;
}

}
