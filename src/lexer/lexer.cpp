#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "sequence-breaker.hpp"
#include "token.hpp"

namespace
{

bool is_reserved_keyword(const std::string& sequence)
{
    return tokens.contains(sequence);
}

bool is_constant(const std::string& sequence)
{
    uint8_t amount_of_dots = 0; // dont forget there are also real numbers
    for (const auto ch : sequence)
    {
        if (ch == '.')
        {
            if (amount_of_dots == 1)
                throw std::runtime_error("Undefined sequence: \"" + sequence + "\"");
            ++amount_of_dots;
            continue;
        }

        if (!std::isdigit(ch))
        {
            return false;
        }
    }
    return true;
}

} // namespace

namespace lexical
{

std::vector<Token> Lexer::ParseStrippedSequence(const std::string& stripped)
{
    std::vector<Token> res;

    if (is_reserved_keyword(stripped))
    {
        res.push_back(Token::asReservedKeyword(stripped));
        return res;
    }

    if (is_constant(stripped))
    {
        res.push_back(Token::asConstant(stripped));
        return res;
    }

    // there are cases like 'a:=4;' where there is no
    // whitespace between the tokens, so we need additional
    // processing of the retreived stripped value.

    // as for this call - we expect this:
    // a:=5; -> a, :=, 5, ;
    // (3-2/5+12 - 1) -> (, 3, -, 2, /, 5, +, 12, -, 1, )
    const auto broken = SequenceBreaker(stripped)
                            .breakBy("->")
                            .breakBy(":=")
                            .breakBy(":")
                            .breakBy(";")
                            .breakBy(".")
                            .breakBy("*")
                            .breakBy("/")
                            .breakBy("+")
                            .breakBy("-")
                            .breakBy("%")
                            .breakBy("\n")
                            .breakBy("[")
                            .breakBy("]")
                            .breakBy("(")
                            .breakBy(")")
                            .breakBy(">=")
                            .breakBy("<=")
                            .breakBy("<")
                            .breakBy(">")
                            .except(".")
                            .between(isdigit, isdigit)
                            .done();

    for (const auto& tok : broken)
    {
        if (is_reserved_keyword(tok))
            res.push_back(Token::asReservedKeyword(tok));
        else if (is_constant(tok))
            res.push_back(Token::asConstant(tok));
        else
        {
            if (isdigit(tok.at(0)))
                throw std::runtime_error("Identifier can not start with a digit: \"" + tok + "\"");
            res.push_back(Token::asIdentifier(tok));
        }
    }

    return res;
}

} // namespace lexical
