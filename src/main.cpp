#include <exception>
#include <iostream>
#include <vector>
#include <filesystem>
#include <cstdlib>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"


int main(int argc, char* argv[])
{
    // if (argc < 2)
    // {
    //     std::cerr << "Error: path to a source file is not provided\n";
    //     return EXIT_FAILURE;
    // }
    // std::string source_file_path = argv[1];
    std::string source_file_path = "/home/max/vscdir/tarsonis/tests/examples/aplusb.tr";

    if (!std::filesystem::exists(source_file_path)) {
        std::cerr << "Error: path to a source file is not valid\n";
        return EXIT_FAILURE;
    }

    lexical::Lexer lexer(source_file_path);
    std::vector<Token> tokens;
    try {
        tokens = lexer.parse();
    } catch (const std::exception& err) {
        std::cout << err.what() << '\n';
        return EXIT_FAILURE;
    }
    for (int i = 0; i < tokens.size(); ++i) {
        std::cout << i << ": \"" << (tokens[i].m_value == "\n" ? "newline" : tokens[i].m_value) << "\" " << tokens[i].m_id << "\n";
    }

    std::cout << "\n";

    try {
        auto parser = parsing::Parser(tokens);
        auto program_ast = parser.parse();
        program_ast->print();
    } catch (const std::exception& err) {
        std::cout << err.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
