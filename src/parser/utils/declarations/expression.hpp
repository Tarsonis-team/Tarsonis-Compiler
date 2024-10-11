
#include "AST-node.hpp"

using namespace parsing;

class Primary : public Expression {};

class Integer : public Primary {};

class Boolean : public Primary {};

class Real : public Primary {};

class Modifiable : public Primary {
    
};

class AccessArray : public Modifiable {

};

class AccessRecord : public Modifiable {

};
