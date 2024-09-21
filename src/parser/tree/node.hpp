#include <vector>

#include "../../lexer/token.hpp"

template <typename T>
class Node {
public:
    explicit Node(T& value, Node* prev);
    explicit Node(T& value);
    explicit Node();

    T value;
    std::vector<Node> next;
    Node* prev;
};