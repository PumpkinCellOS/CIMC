#pragma once

#include <fstream>
#include <string>

bool compiler_make_asm_from_file(std::string input, std::string output);
bool compiler_make_object_from_file(std::string input, std::string output);

namespace convert
{

bool cpp_to_asm(std::ifstream& input, std::ofstream& output);
bool asm_to_obj(std::ifstream& input, std::ofstream& output);

}
