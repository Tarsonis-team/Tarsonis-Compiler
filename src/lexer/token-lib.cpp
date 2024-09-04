#include <unordered_map>
#include <string>

#pragma once

using namespace std;



enum TokenType {
    KEYWORD,
    IDENTIFIER,
    PUNCT
};

class Token {
public:
    string value;
    int id;
    TokenType type;

    Token(string value, int id):value(value), id(id) {
        // TODO: define type
    }
};

const unordered_map<string, int> tokens = {
        {"for", 1},
        {"while", 2}
};

