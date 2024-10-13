#pragma once
#include <memory>
#include <vector>
#include <string>

#include "grammar-units.hpp"

namespace parsing {

class ASTNode {
public:
    explicit ASTNode(GrammarUnit gr) : m_grammar(gr) {}

    virtual ASTNode parse();
    GrammarUnit m_grammar;
    std::weak_ptr<ASTNode> parent;
};

class Body : public ASTNode {

public:
    explicit Body() : ASTNode(GrammarUnit::BODY) {}
    std::vector<std::shared_ptr<ASTNode>> m_items;
};


class Statement : public ASTNode {

public:
    explicit Statement(GrammarUnit gr) : ASTNode(gr) {}
};

class Expression : public ASTNode {

public:
    explicit Expression(GrammarUnit gr) : ASTNode(gr) {}
};

class Declaration : public ASTNode {

public:
    explicit Declaration(GrammarUnit gr, std::string name) : ASTNode(gr), m_name(std::move(name)) {}
    std::string m_name;
};

class Program : public ASTNode {

public:
    explicit Program() : ASTNode(GrammarUnit::PROGRAM) {}
    std::vector<std::shared_ptr<Declaration>> m_declarations;
};

class Range : public ASTNode {

public:
    explicit Range() : ASTNode(GrammarUnit::RANGE), m_reverse(false) {}
    bool m_reverse;
    std::shared_ptr<Expression> m_begin;
    std::shared_ptr<Expression> m_end;
};

}  // namespace parsing
