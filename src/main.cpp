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
        std::cout << tok.m_value << " (" << tok.m_id << ", ";
        switch (tok.m_type) {
            case TokenType::KEYWORD: {
                cout << "K";
                break;
            }
            case TokenType::IDENTIFIER: {
                cout << "I";
                break;
            }
            case TokenType::PUNCT: {
                cout << "P";
                break;
            }
        }
    }

    return 0;
}
