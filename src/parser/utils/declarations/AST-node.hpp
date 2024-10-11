#pragma once
#include <memory>
#include <vector>

#include "grammar-units.hpp"

namespace parsing {

class ASTNode {
public:
    explicit ASTNode(GrammarUnit gr) : m_grammar(gr) {}

    virtual ASTNode parse();
    GrammarUnit m_grammar;
    std::weak_ptr<ASTNode> parent;
};

class Program : public ASTNode {

};

class Body : public ASTNode {

public:
    std::vector<ASTNode> m_items;
};


class Statement : public ASTNode {

};

class Expression : public ASTNode {

};

class Declaration : public ASTNode {

};

class Range : public ASTNode {

public:
    bool m_reverse;
    Expression* m_begin;
    Expression* m_end;    
};

class Identifier : public ASTNode {

};

}  // namespace parsing
