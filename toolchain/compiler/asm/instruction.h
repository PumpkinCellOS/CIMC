#pragma once

#include <memory>

#include "ast.h"
#include "runtimedata.h"

namespace assembler
{

class InstructionOperation
{
public:
    static std::shared_ptr<InstructionOperation> create(const Instruction&, const RuntimeData::Section*);

    virtual bool execute(RuntimeData&) const = 0;

protected:
    InstructionOperation(const Instruction& instruction)
    : m_instruction(instruction) {}

    const Instruction& m_instruction;
};

namespace instructions
{

class Data : public InstructionOperation
{
public:
    Data(const Instruction& instruction)
    : InstructionOperation(instruction) {}

    virtual bool execute(RuntimeData&) const override;
};

class Code : public InstructionOperation
{
public:
    Code(const Instruction& instruction)
    : InstructionOperation(instruction) {}

    virtual bool execute(RuntimeData&) const override;
};

}

}
