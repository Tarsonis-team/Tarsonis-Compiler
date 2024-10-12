#pragma once

#include <memory>
#include "AST-node.hpp"
#include "declaration.hpp"


namespace parsing {

class If : public Statement {

public:
    explicit If() : Statement(GrammarUnit::IF) {}
    std::shared_ptr<Expression> m_condition;
    std::shared_ptr<Body> m_then;
    std::shared_ptr<Body> m_else;
};

class For : public Statement {

public:
    explicit For() : Statement(GrammarUnit::FOR) {}
    std::shared_ptr<Range> m_range;
    std::shared_ptr<Body> m_body;
    std::shared_ptr<Variable> m_identifier;
};

class While : public Statement {

public:
    explicit While() : Statement(GrammarUnit::WHILE) {}
    std::shared_ptr<Body> m_body;
    std::shared_ptr<Expression> m_condition;
    std::shared_ptr<Variable> m_identifier;
};

class RoutineCall : public Statement {

public:
    explicit RoutineCall() : Statement(GrammarUnit::CALL) {}
    std::shared_ptr<Routine> m_routine;
};

class Assignment : public Statement {

public:
    explicit Assignment() : Statement(GrammarUnit::ASSIGNMENT) {}
    std::shared_ptr<Variable> m_identifier;
    std::shared_ptr<Expression> m_expression;
};

}  // namespace parsing
