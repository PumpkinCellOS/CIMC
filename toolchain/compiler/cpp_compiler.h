#pragma once

#include "compiler.h"

#include <limits>

#define STREAM_MAX std::numeric_limits<std::streamsize>::max()

#define PARSE_ERROR(stream, message) do { \
        char nd[17]; \
        nd[16] = 0; \
        stream.read(nd, 16); \
        std::cout << "error: " << message << std::endl; \
        std::cout << "  near '" << nd << "'" << std::endl; \
        return false; \
    } while(0)

#define PARSE_WARNING(stream, message) do { \
        char nd[17]; \
        nd[16] = 0; \
        stream.read(nd, 16); \
        std::cout << "warning: " << message << std::endl; \
        std::cout << "  near '" << nd << "'" << std::endl; \
        return false; \
    } while(0)

namespace cpp_compiler
{

bool compile_to_asm(convert::InputFile& input, convert::OutputFile& output, const compiler::Options& options);

} // cpp_compiler
