#include <iostream>
#include <vector>

#include "lexer/lexer.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    char* source_file_path = argv[1];

    Lexer lexer(source_file_path);

    vector<Token> code = lexer.parse();

    for (auto &tok : code) {
        std::cout << tok.m_value;
    }

    return 0;
}
