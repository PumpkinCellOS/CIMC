set(SOURCES
    "asm/lexer.cpp"
    "cpp/parser.cpp"
    "compiler.cpp"
    "cpp_compiler.cpp"
    "cpp/lexer.cpp"
    "main.cpp")

add_executable(${SCC_PREFIX}-cc ${SOURCES})
target_include_directories(${SCC_PREFIX}-cc PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${SCC_PREFIX}-cc scc-util)
