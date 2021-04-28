set(SOURCES
    "asm/ast.cpp"
    "asm/builder.cpp"
    "asm/directive.cpp"
    "asm/instruction.cpp"
    "asm/lexer.cpp"
    "asm/parser.cpp"
    "asm/runtimedata.cpp"
    "cpp/parser.cpp"
    "cpp/parser_new.cpp"
    "compiler.cpp"
    "cpp_compiler.cpp"
    "cpp/lexer.cpp"
    "main.cpp")

add_executable(${SCC_PREFIX}-cc ${SOURCES})
target_include_directories(${SCC_PREFIX}-cc PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${SCC_PREFIX}-cc scc-util)
