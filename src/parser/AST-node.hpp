#pragma once
#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include "grammar-units.hpp"

namespace parsing {

using std::cout;

class ASTNode {
public:
    explicit ASTNode(GrammarUnit gr) : m_grammar(gr) {}
    ASTNode(ASTNode&&node) = default;
    virtual ~ASTNode() = default;
    virtual void print() {
        cout << "Some AST-Node, the default print function\n";
    };

    virtual GrammarUnit get_grammar() const {
        return this->m_grammar;
    };


    GrammarUnit m_grammar;
    std::weak_ptr<ASTNode> parent;
};

class Body : public ASTNode {

public:
    explicit Body() : ASTNode(GrammarUnit::BODY) {}

    void print() override {
        std::cout << "Body:\n";
        for (auto& item : m_items) {
            item->print();
        }
        std::cout << "End of Body\n";
    }

    std::vector<std::shared_ptr<ASTNode>> m_items;
};


class Statement : public ASTNode {

public:
    ~Statement() override = default;
    explicit Statement(GrammarUnit gr) : ASTNode(gr) {}
};

class Expression : public ASTNode {

public:
    ~Expression() override = default;
    explicit Expression() : ASTNode(GrammarUnit::DIVISION) {}
};

class Declaration : public ASTNode {

public:
    ~Declaration() override = default;
    explicit Declaration(GrammarUnit gr, std::string name) : ASTNode(gr), m_name(std::move(name)) {}
    Declaration(Declaration&&) = default;
    std::string m_name;
};

class Program : public ASTNode {
public:

    void print() override {
        cout << "Beginning of the Program:\n";
        for (auto& declaration : m_declarations) {
            declaration->print();
        }
        cout << "End of the program\n";
    }

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
