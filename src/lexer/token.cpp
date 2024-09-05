#include <string>
#include <unordered_map>

#include "token.hpp"

Token::Token(std::string value, int id) : m_value(value), m_id(id)
{
    if (1 <= id && id <= 26) {
        this->m_type = TokenType::KEYWORD;
    }
    // TODO: define type
}

// clang-format off
const std::unordered_map<std::string, int> tokens = {
        // 0 for Identifiers
        {"routine", 1},
        {"while", 2},
        {"for", 3},
        {"integer", 4},
        {"real", 5},
        {"boolean", 6},
        {"is", 7},
        {"end", 8},
        {"loop", 9},
        {"in", 10},
        {"reverse", 11},
        {"if", 12},
        {"then", 13},
        {"else", 14},
        {"and", 15},
        {"or", 16},
        {"xor", 17},
        {"true", 18},
        {"false", 19},
        {"array", 20},
        {"record", 21},
        {"var", 22},
        {"type", 23},
        {"not", 24},
        {"return", 25},
        {"defer", 26},
        {"\n", 27}
};
// clang-format on
