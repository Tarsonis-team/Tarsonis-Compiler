#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include "AST-node.hpp"
#include "declaration.hpp"
#include "expression.hpp"


namespace parsing {

class If : public Statement {

public:
    explicit If() : Statement(GrammarUnit::IF) {}

    void print() override {
        std::cout << "If statement then\n";
        m_then->print();
        if (m_else.get()) {
            std::cout << "else\n";
            m_else->print();
        }
    }

    std::shared_ptr<Expression> m_condition;
    std::shared_ptr<Body> m_then;
    std::shared_ptr<Body> m_else;
};

class For : public Statement {

public:
    explicit For() : Statement(GrammarUnit::FOR) {}

    void print() override {
        std::cout << "For";
        m_range->print();
        
        m_body->print();
        std::cout << "End of While statement\n";
    }

    std::shared_ptr<Range> m_range;
    std::shared_ptr<Body> m_body;
    std::shared_ptr<Variable> m_identifier;
};

class While : public Statement {

public:
    explicit While() : Statement(GrammarUnit::WHILE) {}

    void print() override {
        std::cout << "While statement cond:\n";
        m_condition->print();
        m_body->print();
        std::cout << "End of While statement\n";
    }

    std::shared_ptr<Body> m_body;
    std::shared_ptr<Expression> m_condition;
};

class RoutineCall : public Statement {

public:
    explicit RoutineCall() : Statement(GrammarUnit::CALL) {}
    std::shared_ptr<Routine> m_routine;
    std::vector<std::shared_ptr<Expression>> m_parameters;
};

class RoutineCallResult : public Expression {

public:
    explicit RoutineCallResult() : Expression() {
        this->m_grammar = GrammarUnit::ROUTINE_CALL;
    }

    std::shared_ptr<RoutineCall> m_routine_call;
};

class Assignment : public Statement {

public:
    explicit Assignment() : Statement(GrammarUnit::ASSIGNMENT) {}

    void print() override {
        std::cout << "Assignment ";
        m_modifiable->print();
        std::cout << " to ";
        m_expression->print();
    }

    std::shared_ptr<Modifiable> m_modifiable;
    std::shared_ptr<Expression> m_expression;
};

}  // namespace parsing
