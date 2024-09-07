#include <iostream>
#include <vector>

#include "lexer/lexer.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    char* source_file_path = argv[1];

    lexical::Lexer lexer(source_file_path);

    vector<Token> code = lexer.parse();

    for (auto& tok : code)
    {
        std::cout << "\"" << (tok.m_value == "\n" ? "newline" : tok.m_value) << "\", \n";
    }

    return 0;
}
