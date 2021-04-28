#include "ast.h"

#include "directive.h"
#include "instruction.h"
#include "runtimedata.h"
#include "../cpp_compiler.h"

namespace assembler
{

bool Instruction::execute(RuntimeData& data) const
{
    std::cout << "Instruction::execute " << display() << std::endl;
    auto operation = InstructionOperation::create(*this, data.current_section());
    if(!operation)
        BUILDER_ERROR(this, "invalid instruction");
    return operation->execute(data);
}

bool Directive::execute(RuntimeData& data) const
{
    std::cout << "Directive::execute " << display() << std::endl;
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

bool Label::execute(RuntimeData&) const
{
    //std::cout << "Label::execute " << display() << std::endl;
    return true;
}

}
