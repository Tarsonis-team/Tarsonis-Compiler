#pragma once

#include <memory>
#include "AST-node.hpp"
#include "declaration.hpp"


using namespace parsing;

class If : public Statement {

public:
    explicit If() : Statement(GrammarUnit::IF) {}
    std::unique_ptr<Expression> m_condition;
    std::unique_ptr<Body> m_then;
    std::unique_ptr<Body> m_else;
};

class For : public Statement {

public:
    explicit For() : Statement(GrammarUnit::FOR) {}
    std::unique_ptr<Range> m_range;
    std::unique_ptr<Body> m_body;
    std::unique_ptr<Variable> m_identifier;
};

class While : public Statement {

public:
    explicit While() : Statement(GrammarUnit::WHILE) {}
    std::unique_ptr<Body> m_body;
    std::unique_ptr<Expression> m_condition;
    std::unique_ptr<Variable> m_identifier;
};

class RoutineCall : public Statement {

public:
    explicit RoutineCall() : Statement(GrammarUnit::CALL) {}
    std::unique_ptr<Routine> m_routine;
};

class Assignment : public Statement {

public:
    explicit Assignment() : Statement(GrammarUnit::ASSIGNMENT) {}
    std::unique_ptr<Variable> m_identifier;
    std::unique_ptr<Expression> m_expression;
};



