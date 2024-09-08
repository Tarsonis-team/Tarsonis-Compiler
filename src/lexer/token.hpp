#pragma once

#include <string>
#include <unordered_map>

extern const std::unordered_map<std::string, int> tokens;

enum class TokenType
{
    KEYWORD,
    IDENTIFIER,
    CONSTANT,
    OPERATOR, // %, +, /, :=
};

struct Token
{
    std::string m_value;
    int m_id;
    TokenType m_type;

    static Token asIdentifier(const std::string& value);
    static Token asReservedKeyword(const std::string& keyword);
    static Token asConstant(const std::string& cnst);

private:
    Token(std::string value, int id);
};
