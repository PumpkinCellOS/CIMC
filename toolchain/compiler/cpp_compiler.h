#pragma once

#include "compiler.h"

#include <limits>

#define STREAM_MAX std::numeric_limits<std::streamsize>::max()

#define PARSE_ERROR(lexoutput, message) do { \
        std::cout << "\e[31merror:\e[0m " << (message) << std::endl << "  token " << (lexoutput).index() << ", near "; \
        if((lexoutput).peek()) \
            (lexoutput).peek()->display(); \
        else \
            std::cout << "EOF" << std::endl; \
        return {}; \
    } while(0)

#define PARSE_WARNING(lexoutput, message) do { \
        std::cout << "\e[93mwarning:\e[0m " << (message) << std::endl << "  token " << (lexoutput).index() << ", near "; \
        if((lexoutput).peek()) \
            (lexoutput).peek()->display(); \
        else \
            std::cout << "EOF" << std::endl; \
    } while(0)

#define LEX_ERROR(stream, message) do { \
        char nd[17]; \
        nd[16] = 0; \
        (stream).read(nd, 16); \
        std::cout << "\e[31merror:\e[0m " << (message) << std::endl; \
        std::cout << "  near '" << nd << "'" << std::endl; \
        return {}; \
    } while(0)

#define LEX_WARNING(stream, message) do { \
        char nd[17]; \
        nd[16] = 0; \
        (stream).read(nd, 16); \
        std::cout << "\e[93mwarning:\e[0m " << (message) << std::endl; \
        std::cout << "  near '" << nd << "'" << std::endl; \
    } while(0)

#define BUILDER_ERROR(op, message) do { \
        std::cout << "\e[31merror:\e[0m " << (message) << std::endl << "  node " << (op)->display_name() << ", near "; \
        if(op) \
            std::cout << (op)->display() << std::endl; \
        else \
            std::cout << "EOF" << std::endl; \
        return {}; \
    } while(0)

#define BUILDER_WARNING(op, message) do { \
        std::cout << "\e[93mwarning:\e[0m " << (message) << std::endl << "  node " << (op)->display_name() << ", near "; \
        if(op) \
            std::cout << (op)->display() << std::endl; \
        else \
            std::cout << "EOF" << std::endl; \
    } while(0)

namespace cpp_compiler
{

bool compile_to_asm(convert::InputFile& input, convert::OutputFile& output, const compiler::Options& options);

} // cpp_compiler
