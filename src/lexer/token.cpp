#include <string>
#include <unordered_map>

#include "token.hpp"

Token::Token(std::string value, int id) : m_value(value), m_id(id)
{
    // TODO: define type
}

// clang-format off
const std::unordered_map<std::string, int> tokens {
    { "for", 1 },
    { "while", 2 }
};
// clang-format on
