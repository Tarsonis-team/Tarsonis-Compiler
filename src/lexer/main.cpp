#include <iostream>
#include <vector>

#include "token-lib.cpp"
#include "lexer.cpp"

using namespace std;

int main() {
    Lexer lexer("code.txt");

    vector<Token>code = lexer.parse();

    return 0;
}

