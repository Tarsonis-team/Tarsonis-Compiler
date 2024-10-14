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

    std::string gr_to_str() {
        switch (this->m_grammar) {
            case GrammarUnit::PROGRAM: return "PROGRAM";
            case GrammarUnit::BODY: return "BODY";
            case GrammarUnit::RANGE: return "RANGE";
            case GrammarUnit::IDENTIFIER: return "IDENTIFIER";
            case GrammarUnit::VARIABLE: return "VARIABLE";
            case GrammarUnit::ROUTINE: return "ROUTINE";
            case GrammarUnit::PARAMETER: return "PARAMETER";
            case GrammarUnit::RECORD: return "RECORD";
            case GrammarUnit::TYPE: return "TYPE";
            case GrammarUnit::ARRAY: return "ARRAY";
            case GrammarUnit::IF: return "IF";
            case GrammarUnit::FOR: return "FOR";
            case GrammarUnit::WHILE: return "WHILE";
            case GrammarUnit::CALL: return "CALL";
            case GrammarUnit::ASSIGNMENT: return "ASSIGNMENT";
            case GrammarUnit::INTEGER: return "INTEGER";
            case GrammarUnit::BOOL: return "BOOL";
            case GrammarUnit::REAL: return "REAL";
            case GrammarUnit::ACCESS_ARRAY: return "ACCESS_ARRAY";
            case GrammarUnit::ACCESS_RECORD: return "ACCESS_RECORD";
            case GrammarUnit::PLUS: return "PLUS";
            case GrammarUnit::MINUS: return "MINUS";
            case GrammarUnit::MULTIPLICATE: return "MULTIPLICATE";
            case GrammarUnit::DIVISION: return "DIVISION";
            case GrammarUnit::MOD: return "MOD";
            case GrammarUnit::GREATER: return "GREATER";
            case GrammarUnit::LESS: return "LESS";
            case GrammarUnit::GREATER_EQUAL: return "GREATER_EQUAL";
            case GrammarUnit::LESS_EQUAL: return "LESS_EQUAL";
            case GrammarUnit::EQUAL: return "EQUAL";
            case GrammarUnit::NOT_EQUAL: return "NOT_EQUAL";
            case GrammarUnit::AND: return "AND";
            case GrammarUnit::OR: return "OR";
            case GrammarUnit::XOR: return "XOR";
            case GrammarUnit::TRUE: return "TRUE";
            case GrammarUnit::FALSE: return "FALSE";
            default: return "UNKNOWN";
        }
    }


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

    void print() override {
        std::cout << "Range: \n";
    }

    bool m_reverse;
    std::shared_ptr<Expression> m_begin;
    std::shared_ptr<Expression> m_end;
};

}  // namespace parsing
