#pragma once

#include <string>

namespace assembler
{

class RuntimeData;

class Opcode
{
public:
    virtual std::string payload(const RuntimeData&) const = 0;
    virtual std::string display() const = 0;
};

namespace opcodes
{

class NoArgument : public Opcode
{
public:
    NoArgument(unsigned char id)
    : m_id(id) {}

    virtual std::string payload(const RuntimeData&) const override { return std::string((char*)&m_id, 1); }
    virtual std::string display() const { return "NoArgument " + std::to_string((unsigned)m_id); }

private:
    unsigned char m_id;
};

class Invalid : public NoArgument
{
public:
    Invalid()
    : NoArgument(0x7e) {}

    virtual std::string display() const { return "Invalid"; }
};

class Label : public Opcode
{
public:
    Label(std::string name)
    : m_name(name) {}

    virtual std::string payload(const RuntimeData&) const override { return ""; }
    virtual std::string display() const { return "Label " + m_name + ":"; }

private:
    std::string m_name;
};

}

}
