#include <utility>
#include <vector>

#include "parser.hpp"
#include "utils/declarations/declaration.cpp"

namespace parse {

Parser::Parser(std::vector<Token>& tokens) : tokens(std::move(tokens))
{
}

Tree<Declaration> Parser::get_decl_tree() {
    // ...
    typedef Constant<int> constant;

    Constant<int> test_const = Constant<int>(VarType::INT, 1);
    Node<constant> test_root = Node<constant>(test_const);
    Tree<constant> test_tree = Tree(&test_root);
}

}
