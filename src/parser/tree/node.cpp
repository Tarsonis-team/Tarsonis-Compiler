#include "node.hpp"

template <typename T>
Node<T>::Node(T& value, Node<T> *prev): value(value), prev(prev) {}

template <typename T>
Node<T>::Node(T& value): value(value), prev(nullptr) {}

template <typename T>
Node<T>::Node(): prev(nullptr) {}
