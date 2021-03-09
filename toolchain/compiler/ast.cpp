#include "ast.h"

namespace cpp_compiler
{

namespace AST
{

std::string indent(size_t depth, std::string _fill, std::string _lastfill)
{
    std::string str;
    for(size_t s = 0; s < depth; s++)
    {
        if(s == depth - 1)
            str += _lastfill;
        else
            str += _fill;
    }
    return str;
}

std::shared_ptr<Expression> parse_expression(LexOutput& output);

std::shared_ptr<Declaration> parse_declaration(LexOutput& output)
{
    std::shared_ptr<Declaration> declaration;

    size_t position = output.index();
    declaration = std::make_shared<FunctionDefinition>();
    if(!declaration->from_lex(output))
    {
        output.set_index(position);
        declaration = std::make_shared<VariableDeclaration>();
        if(!declaration->from_lex(output))
        {
            return nullptr;
        }
    }
    return declaration;
}

// translation-unit ::= [declaration ... ]
bool TranslationUnit::from_lex(LexOutput& output)
{
    while(output.peek())
    {
        std::shared_ptr<Declaration> declaration = parse_declaration(output);
        if(declaration)
        {
            subnodes.push_back(declaration);
            continue;
        }

        PARSE_ERROR(output, "expected declaration");
    }
    return true;
}

bool SimpleTypeSpecifier::from_lex(LexOutput& output)
{
    auto token = output.consume_token_of_type(Token::TypeName);
    if(!token)
        return false;

    if(token->value == "unsigned")
    {
        auto token2 = output.consume_token_of_type(Token::TypeName);
        if(!token2)
            simple_type = SimpleType::UnsignedInt; // if given 'unsigned', expand to 'unsigned int'
        else if(token2->value == "int")
            simple_type = SimpleType::UnsignedInt;
        else if(token2->value == "short")
            simple_type = SimpleType::UnsignedShort;
        else if(token2->value == "long")
            simple_type = SimpleType::UnsignedLong;
        else if(token2->value == "char")
            simple_type = SimpleType::UnsignedChar;
        else
            PARSE_ERROR(output, "expected integer type specifier after 'unsigned'");
    }
    else
    {
        if(token->value == "int")
            simple_type = SimpleType::Int;
        else if(token->value == "short")
            simple_type = SimpleType::Short;
        else if(token->value == "long")
            simple_type = SimpleType::Long;
        else if(token->value == "bool")
            simple_type = SimpleType::Bool;
        else if(token->value == "char")
            simple_type = SimpleType::Char;
        else if(token->value == "void")
            simple_type = SimpleType::Void;
        else
            PARSE_ERROR(output, "invalid simple-type-specifier");
    }
    return true;
}

std::shared_ptr<TypeSpecifier> parse_type_specifier(LexOutput& output)
{
    std::shared_ptr<TypeSpecifier> type_specifier = std::make_shared<SimpleTypeSpecifier>();
    if(type_specifier->from_lex(output))
    {
        // TODO: Make it better when lexer will be able to distinguish operators!
        auto op = output.peek();
        if(op->type == Token::Operator)
        {
            bool all_are_stars = true;
            for(auto ch : op->value)
            {
                if(ch != '*')
                {
                    all_are_stars = false;
                    break;
                }
            }

            if(!all_are_stars)
                PARSE_ERROR(output, "invalid pointer specifier");

            output.consume_token();
            auto ptr_type_specifier = std::make_shared<PointerTypeSpecifier>();
            ptr_type_specifier->type_specifier = type_specifier;
            ptr_type_specifier->level = op->value.size();
            return ptr_type_specifier;
        }
        return type_specifier;
    }
    return nullptr;
}

bool PointerTypeSpecifier::from_lex(LexOutput& output)
{
    return true;
}

std::shared_ptr<Statement> parse_statement(LexOutput& output)
{
    // Statement
    std::shared_ptr<Statement> statement;

    size_t position = output.index();
    statement = std::make_shared<CodeBlock>();
    if(!statement->from_lex(output))
    {
        output.set_index(position);
        statement = std::make_shared<IfStatement>();
        if(!statement->from_lex(output))
        {
            output.set_index(position);
            statement = std::make_shared<ReturnStatement>();
            if(!statement->from_lex(output))
            {
                output.set_index(position);
                statement = std::make_shared<ExpressionStatement>();
                if(!statement->from_lex(output))
                {
                    output.set_index(position);
                    statement = std::make_shared<DeclarationStatement>();
                    if(!statement->from_lex(output))
                    {
                        return nullptr;
                    }
                }
            }
        }
    }

    // Semicolon (if applicable)
    if(!statement->need_semicolon())
        return statement;

    auto semicolon = output.consume_token_of_type(Token::Semicolon);
    if(semicolon)
        return statement;

    PARSE_ERROR(output, "expected ';' in statement");
}

std::shared_ptr<Expression> parse_parenthised_expression(LexOutput& output)
{
    auto lc = output.consume_token_of_type(Token::LeftBracket);
    if(!lc)
        return nullptr;

    std::shared_ptr<Expression> expression = parse_expression(output);
    if(!expression)
        PARSE_ERROR(output, "expected expression in parentheses");

    auto rc = output.consume_token_of_type(Token::RightBracket);
    if(!rc)
        PARSE_ERROR(output, "expected ')'");

    return expression;
}

// primary-expression ::= ( expression ) | integer-literal | identifier
std::shared_ptr<Expression> parse_primary_expression(LexOutput& output)
{
    std::shared_ptr<Expression> expression;

    size_t position = output.index();
    expression = parse_parenthised_expression(output);
    if(!expression)
    {
        expression = std::make_shared<IntegerLiteral>();
        if(!expression->from_lex(output))
        {
            output.set_index(position);
            expression = std::make_shared<Identifier>();
            if(!expression->from_lex(output))
            {
                output.set_index(position);
                return nullptr;
            }
        }
    }

    return expression;
}

// TODO: Make function name expression instead of identifier (allows function pointers)
// function-call ::= function-expression | subscript | primary-expression
std::shared_ptr<Expression> parse_function_call(LexOutput& output)
{
    std::shared_ptr<Expression> expression;

    size_t position = output.index();
    expression = std::make_shared<FunctionCall>();
    if(!expression->from_lex(output))
    {
        output.set_index(position);
        expression = std::make_shared<Subscript>();
        if(!expression->from_lex(output))
        {
            output.set_index(position);
            expression = parse_primary_expression(output);
            if(!expression)
            {
                output.set_index(position);
                return nullptr;
            }
        }
    }

    return expression;
}

// unary-expression ::= function-call
// | unary-operator unary-expression
std::shared_ptr<Expression> parse_unary_expression(LexOutput& output)
{
    size_t position = output.index();
    std::shared_ptr<Expression> lhs = parse_function_call(output);
    if(!lhs)
    {
        output.set_index(position);

        // Unary OP
        auto oper = output.consume_token_of_type(Token::Operator);
        if(!oper)
        {
            output.set_index(position);
            return nullptr; // Just function call (not operator)
        }

        // Other expression
        auto rhs = parse_unary_expression(output);
        if(!rhs)
            PARSE_ERROR(output, "expected expression after unary operator");

        std::shared_ptr<UnaryExpression> unary = std::make_shared<UnaryExpression>();
        unary->expression = rhs;

        if(oper->value == "&")
            unary->type = UnaryExpression::Type::Address;
        else if(oper->value == "*")
            unary->type = UnaryExpression::Type::Dereference;
        else if(oper->value == "+")
            unary->type = UnaryExpression::Type::Plus;
        else if(oper->value == "-")
            unary->type = UnaryExpression::Type::Minus;
        else if(oper->value == "~")
            unary->type = UnaryExpression::Type::BitNegate;
        else if(oper->value == "!")
            unary->type = UnaryExpression::Type::LogicNegate;
        else if(oper->value == "++")
            unary->type = UnaryExpression::Type::Increment;
        else if(oper->value == "--")
            unary->type = UnaryExpression::Type::Decrement;

        return unary;
    }
    return lhs;
}

// assignment-expression ::= unary-expression assignment-op assignment-expression
// | unary-expression
std::shared_ptr<Expression> parse_assignment_expression(LexOutput& output)
{
    // First unary expression
    std::shared_ptr<Expression> lhs = parse_unary_expression(output);
    if(!lhs)
        return nullptr;

    // Assignment OP
    auto op = output.consume_token_of_type(Token::Operator);
    if(!op)
        return lhs; // Just unary expression

    // Other assignment expression or unary expression
    auto rhs = parse_assignment_expression(output);
    if(!rhs)
        PARSE_ERROR(output, "expected expression after assignment operator");

    std::shared_ptr<AssignmentExpression> assignment = std::make_shared<AssignmentExpression>();
    assignment->lhs = lhs;
    // TODO: Actually save the operator!
    assignment->rhs = rhs;
    return assignment;
}

// expression ::= assignment-expression
// TODO: comma expression
std::shared_ptr<Expression> parse_expression(LexOutput& output)
{
    // TODO: Make FunctionCall a postfix-expression
    std::shared_ptr<Expression> expression = parse_assignment_expression(output);
    return expression;
}

bool IntegerLiteral::from_lex(LexOutput& output)
{
    auto token = output.peek();
    if(token && token->type == Token::Number)
    {
        output.consume_token();
        try
        {
            value = std::stoi(token->value);
        }
        catch(...)
        {
            PARSE_ERROR(output, "invalid integer literal");
        }
        return true;
    }
    return false;
}

bool Identifier::from_lex(LexOutput& output)
{
    auto name_token = output.consume_token_of_type(Token::Name);
    if(!name_token)
        return false;

    name = name_token->value;
    return true;
}

// TODO: Make function-name an expression!
// function-call ::= function-name ( [expression,...] )
bool FunctionCall::from_lex(LexOutput& output)
{
    auto name = output.peek();
    if(name && name->type == Token::Name)
    {
        function_name = name->value;

        output.consume_token();
        auto lc = output.consume_token_of_type(Token::LeftBracket);
        if(!lc)
            return false;

        // Arguments
        bool is_first = true;
        while(true)
        {
            auto expression = parse_expression(output);
            if(!expression)
            {
                if(is_first)
                    break;
                else
                    PARSE_ERROR(output, "expected expression in function call");
            }

            arguments.push_back(expression);

            auto comma = output.consume_token_of_type(Token::Comma);
            if(!comma)
                break;

            is_first = false;
        }

        auto rc = output.consume_token_of_type(Token::RightBracket);
        if(!rc)
            PARSE_ERROR(output, "expected ')'");

        return true;
    }
    return false;
}

bool Subscript::from_lex(LexOutput& output)
{
    subscripted = parse_primary_expression(output);
    if(subscripted)
    {
        auto lc = output.consume_token_of_type(Token::LeftSquareBracket);
        if(!lc)
            return false;

        // Subscript value
        in_subscript = parse_expression(output);
        if(!in_subscript)
            PARSE_ERROR(output, "expected expression in subscript");

        auto rc = output.consume_token_of_type(Token::RightSquareBracket);
        if(!rc)
            PARSE_ERROR(output, "expected ']'");

        return true;
    }
    return false;
}

bool CodeBlock::from_lex(LexOutput& output)
{
    // Opening
    auto lb = output.consume_token_of_type(Token::LeftCurly);
    if(!lb)
        return false;

    // Instructions
    do
    {
        auto token = output.peek();
        if(!token || token->type == Token::RightCurly)
            break;

        auto statement = parse_statement(output);
        if(!statement)
            PARSE_ERROR(output, "expected statement");
        subnodes.push_back(statement);
    } while(true);

    // Closing
    auto rb = output.consume_token_of_type(Token::RightCurly);
    if(!rb)
        return false;
    return true;
}

bool IfStatement::from_lex(LexOutput& output)
{
    auto if_keyword = output.consume_token_of_type(Token::Keyword);
    if(if_keyword && if_keyword->value == "if")
    {
        // Condition
        auto lc = output.consume_token_of_type(Token::LeftBracket);
        if(!lc)
            PARSE_ERROR(output, "expected '(' after 'if'");

        condition = parse_expression(output);
        if(!condition)
            PARSE_ERROR(output, "expected expression");

        auto rc = output.consume_token_of_type(Token::RightBracket);
        if(!rc)
            PARSE_ERROR(output, "expected ')' after condition");

        // IF statement
        if_statement = parse_statement(output);
        if(!if_statement)
            PARSE_ERROR(output, "expected statement after 'if'");

        // Optional ELSE
        auto else_keyword = output.peek();

        if(else_keyword && else_keyword->type == Token::Keyword && else_keyword->value == "else")
        {
            output.consume_token();
            else_statement = parse_statement(output);
        }

        return true;
    }
    return false;
}

bool ReturnStatement::from_lex(LexOutput& output)
{
    auto retkw = output.peek();
    if(retkw && retkw->type == Token::Keyword && retkw->value == "return")
    {
        output.consume_token();
        expression = parse_expression(output);
        return true;
    }
    return false;
}

bool ExpressionStatement::from_lex(LexOutput& output)
{
    expression = parse_expression(output);
    if(!expression)
        return false;
    return true;
}

bool TypeSpecifier::from_lex(LexOutput& output)
{
    auto token = output.consume_token_of_type(Token::TypeName);
    if(!token)
        return false;
    name = token->value;
    return true;
}

// function-definition ::= type-specifier name ( [arg-definition,]... ) function-body
bool FunctionDefinition::from_lex(LexOutput& output)
{
    // Type
    type = parse_type_specifier(output);
    if(!type)
        return false;

    // Name
    auto name_token = output.consume_token_of_type(Token::Name);
    if(!name_token)
    {
        PARSE_ERROR(output, "expected function name");
    }
    name = name_token->value;

    // Arguments open
    if(!output.consume_token_of_type(Token::LeftBracket))
        return false;

    // Arguments
    while(true)
    {
        auto argument = std::make_shared<VariableDeclaration>();
        if(!argument->from_lex(output))
            break;
        args.push_back(argument);

        // Comma
        auto comma = output.consume_token_of_type(Token::Comma);
        if(!comma)
            break;
    }

    // Arguments close
    if(!output.consume_token_of_type(Token::RightBracket))
    {
        PARSE_ERROR(output, "expected ')' in function-definition");
    }

    // Function body
    auto code_block = std::make_shared<CodeBlock>();
    if(!code_block->from_lex(output))
        PARSE_ERROR(output, "expected function body");
    body = code_block;

    return true;
}

// variable-declaration ::= type-specifier name [= expression]
bool VariableDeclaration::from_lex(LexOutput& output)
{
    type = parse_type_specifier(output);
    if(!type)
        return false;

    auto name_token = output.consume_token_of_type(Token::Name);
    if(!name_token)
    {
        PARSE_ERROR(output, "expected name in variable declaration");
    }
    name = name_token->value;

    auto default_value_eq = output.consume_token_of_type(Token::Operator);
    if(default_value_eq)
    {
        default_value = parse_expression(output);
        if(!default_value)
            PARSE_ERROR(output, "expected expression in initializer");
    }

    return true;
}

bool DeclarationStatement::from_lex(LexOutput& output)
{
    auto decl = parse_declaration(output);
    if(!decl)
        return false;
    declaration = decl;
    return true;
}

}

}
