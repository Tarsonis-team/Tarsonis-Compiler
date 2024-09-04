#include <iostream>
#include <vector>

#include "lexer.cpp"
#include "token-lib.cpp"

using namespace std;

int main()
{
    Lexer lexer("code.txt");

    vector<Token> code = lexer.parse();

    return 0;
}
