#pragma once

#include <string>

namespace assembler
{

class RuntimeData;

class Opcode
{
public:
    virtual std::string payload(const RuntimeData&) const = 0;
};

namespace opcodes
{

class NoArgumentOpcode : public Opcode
{
public:
    NoArgumentOpcode(unsigned char id)
    : m_id(id) {}

    virtual std::string payload(const RuntimeData&) const override { return std::string((char*)&m_id, 1); }

private:
    unsigned char m_id;
};

class InvalidOpcode : public NoArgumentOpcode
{
public:
    InvalidOpcode()
    : NoArgumentOpcode(0x7e) {}
};

}

}
