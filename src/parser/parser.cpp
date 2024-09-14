#include <utility>
#include <vector>

#include "parser.hpp"

namespace parse {

Parser::Parser(std::vector<Token>& tokens) : tokens(std::move(tokens))
{
}

Tree Parser::create_tree() {
    // ...
}

}
