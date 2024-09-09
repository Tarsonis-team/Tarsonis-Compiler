#include <iostream>
#include <vector>
#include <filesystem>
#include <cstdlib>

#include "lexer/lexer.hpp"


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

    lexical::Lexer lexer(source_file_path);

    std::vector<Token> code = lexer.parse();

    for (auto& tok : code)
    {
        std::cout << "\"" << (tok.m_value == "\n" ? "newline" : tok.m_value) << "\", \n";
    }

    return EXIT_SUCCESS;
}
