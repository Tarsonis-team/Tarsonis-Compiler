#include <vector>

#include "./../lexer/token.hpp"

class Node {
public:
    explicit Node(Token& token, Node* prev);
    Token token;
    std::vector<Node> next;
    Node* prev;
};