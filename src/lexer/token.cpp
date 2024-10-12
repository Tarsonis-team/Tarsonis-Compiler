#include <string>
#include <unordered_map>

#include "token.hpp"

Token::Token(std::string value, int id) : m_value(std::move(value)), m_id(id)
{
}

Token Token::asIdentifier(const std::string& value)
{
    return { value, 0 };
}

Token Token::asReservedKeyword(const std::string& keyword)
{
    return { keyword, tokens.at(keyword) };
}

Token Token::asConstant(const std::string& cnst)
{
    return { cnst, -1 };
}

// clang-format off
// token signature -> id of the token
const std::unordered_map<std::string, int> tokens {
        // -1 for consts
        // 0 for Identifiers
        {"routine",   1},
        {"while",     2},
        {"for",       3},
        {"integer",   4},
        {"real",      5},
        {"boolean",   6},
        {"is",        7},
        {"end",       8},
        {"loop",      9},
        {"in",       10},
        {"reverse", 11},
        {"if",      12},
        {"then",    13},
        {"else",    14},
        {"and",     15},
        {"or",      16},
        {"xor",     17},
        {"true",    18},
        {"false",   19},
        {"array",   20},
        {"record",  21},
        {"var",     22},
        {"type",    23},
        {"not",     24},
        {"return",  25},
        {"defer",   26},
        {"\n",      27},
        {"..",      28},
        {"(",       29},
        {")",       30},
        {",",       31},
        {"->",      32},
        {":",       33},
        {"[",       34},
        {"]",       35},
};
// clang-format on
