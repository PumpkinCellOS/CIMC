#pragma once

#include "ast.h"
#include "lexer.h"

namespace assembler
{

std::shared_ptr<Operand> parse_operand(Lexer& lexer);
std::shared_ptr<Identifier> parse_identifier(Lexer& lexer);
std::shared_ptr<RelativeAddress> parse_relative_address(Lexer& lexer);
std::shared_ptr<AddressOperand> parse_address_operand(Lexer& lexer);
std::shared_ptr<ISROperand> parse_isr_operand(Lexer& lexer);
std::shared_ptr<SectionOperand> parse_section_operand(Lexer& lexer);
std::shared_ptr<Instruction> parse_instruction(Lexer& lexer);
std::shared_ptr<Directive> parse_directive(Lexer& lexer);
std::shared_ptr<Assignment> parse_assignment(Lexer& lexer);
std::shared_ptr<Label> parse_label(Lexer& lexer);
std::shared_ptr<Operation> parse_operation(Lexer& lexer);
std::shared_ptr<Block> parse_block(Lexer& lexer);

}
