#pragma once

#include <memory>

#include "ast.h"

namespace assembler
{

class DirectiveOperation
{
public:
    static std::shared_ptr<DirectiveOperation> create(const Directive&);

    virtual bool execute(RuntimeData&) const = 0;

protected:
    DirectiveOperation(const Directive& directive)
    : m_directive(directive) {}

    const Directive& m_directive;
};

namespace directives
{

class Globl : public DirectiveOperation
{
public:
    virtual bool execute(RuntimeData&) const override;

    Globl(const Directive& directive)
    : DirectiveOperation(directive) {}
};

class Type : public DirectiveOperation
{
public:
    virtual bool execute(RuntimeData&) const override;

    Type(const Directive& directive)
    : DirectiveOperation(directive) {}
};

class Section : public DirectiveOperation
{
public:
    virtual bool execute(RuntimeData&) const override;

    Section(const Directive& directive)
    : DirectiveOperation(directive) {}
};

}

}
