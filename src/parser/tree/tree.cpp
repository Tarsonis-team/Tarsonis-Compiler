#include "tree.hpp"

template <typename T>
Tree<T>::Tree(Node<T>* root): root(root) {}

template <typename T>
Tree<T>::Tree(): root(nullptr) {}
