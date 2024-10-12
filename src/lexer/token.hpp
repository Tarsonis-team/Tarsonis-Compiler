#pragma once

#include <string>
#include <unordered_map>

extern const std::unordered_map<std::string, int> tokens;

enum TokenType {
    TOKEN_IDENTIFIER = 0, // 0 for Identifiers
    TOKEN_CONST = -1,     // -1 for consts

    TOKEN_ROUTINE = 1,
    TOKEN_WHILE = 2,
    TOKEN_FOR = 3,
    TOKEN_INTEGER = 4,
    TOKEN_REAL = 5,
    TOKEN_BOOLEAN = 6,
    TOKEN_IS = 7,
    TOKEN_END = 8,
    TOKEN_LOOP = 9,
    TOKEN_IN = 10,
    TOKEN_REVERSE = 11,
    TOKEN_IF = 12,
    TOKEN_THEN = 13,
    TOKEN_ELSE = 14,
    TOKEN_AND = 15,
    TOKEN_OR = 16,
    TOKEN_XOR = 17,
    TOKEN_TRUE = 18,
    TOKEN_FALSE = 19,
    TOKEN_ARRAY = 20,
    TOKEN_RECORD = 21,
    TOKEN_VAR = 22,
    TOKEN_TYPE = 23,
    TOKEN_NOT = 24,
    TOKEN_RETURN = 25,
    TOKEN_DEFER = 26,
    TOKEN_NEWLINE = 27,
    TOKEN_DOTDOT = 28,
    TOKEN_LPAREN = 29,
    TOKEN_RPAREN = 30,
    TOKEN_COMA   = 31,
    TOKEN_ARROW  = 32,
    TOKEN_COLON = 33,
    TOKEN_LBRACKET = 34,
    TOKEN_RBRACKET = 35
};

struct Token
{
    std::string m_value;
    int m_id;

    static Token asIdentifier(const std::string& value);
    static Token asReservedKeyword(const std::string& keyword);
    static Token asConstant(const std::string& cnst);

private:
    Token(std::string value, int id);
};
