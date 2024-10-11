#pragma once

#include "AST-node.hpp"
#include "declaration.hpp"


using namespace parsing;

class If : public Statement {


public:
    Body m_then;
    Body m_else;
};

class For : public Statement {

public:
    Range m_range;
    Body m_body;
    Variable m_identifier;
};

class While : public Statement {};

class RoutineCall : public Statement {};

class Assignment : public Statement {};



