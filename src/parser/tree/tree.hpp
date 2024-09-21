#include "node.hpp"

template <typename T>
class Tree{
public:
    explicit Tree(Node<T>* root);
    explicit Tree();
    Node<T> root;
};