#include <exception>
#include <iostream>
#include <vector>
#include <filesystem>
#include <cstdlib>

#include "lexer/lexer-controller.hpp"


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Error: path to a source file is not provided\n";
        return EXIT_FAILURE;
    }
    std::string source_file_path = argv[1];

    if (!std::filesystem::exists(source_file_path)) {
        std::cerr << "Error: path to a source file is not valid\n";
        return EXIT_FAILURE;
    }

    lexical::LexerController lexer(source_file_path);
    std::vector<Token> code;
    try {
        code = lexer.parse();
    } catch (const std::exception& err) {
        std::cout << err.what() << '\n';
        return EXIT_FAILURE;
    }

    for (auto& tok : code)
    {
        std::cout << "\"" << (tok.m_value == "\n" ? "newline" : tok.m_value) << "\" " << tok.m_id << "\n";
    }

    return EXIT_SUCCESS;
}
