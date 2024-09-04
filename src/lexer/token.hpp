#pragma once

#include <string>
#include <unordered_map>

extern const std::unordered_map<std::string, int> tokens;

enum class TokenType
{
    KEYWORD,
    IDENTIFIER,
    PUNCT
};

struct Token
{
    std::string m_value;
    int m_id;
    TokenType m_type;

    Token(std::string value, int id);

};
