#pragma once

#include "AST-node.hpp"

#include <vector>
#include <string>
#include <memory>

namespace parsing {

class Variable : public Declaration {

public:
    explicit Variable(std::string name) : Declaration(GrammarUnit::VARIABLE, std::move(name)) {}
    std::weak_ptr<Expression> m_value;
};

class RoutineParameter : public Declaration {

public:
    RoutineParameter(std::string name, std::string type) : Declaration(GrammarUnit::PARAMETER, std::move(name)), m_type(std::move(type)) {}
    std::string m_identifier;
    std::string m_type;
};

class Routine : public Declaration {

public:
    explicit Routine(std::string name) : Declaration(GrammarUnit::ROUTINE, std::move(name)) {}
    
    std::unique_ptr<Body> m_body;
    std::vector<std::shared_ptr<RoutineParameter>> m_params;
    std::string return_type;
};

class Type : public Declaration {
public:
    explicit Type(std::string& name) : Declaration(GrammarUnit::TYPE, name) {}
    std::unique_ptr<Declaration> m_original;
};

class Record : public Type {
public:
    explicit Record(std::string& name) : Type(name) {}
    std::vector<std::unique_ptr<Declaration>> m_fields;
};

class Array : public Type {
public:
    explicit Array(std::string& name, int size) : Type(name), m_size(size) {}
    std::vector<std::unique_ptr<Declaration>> m_items;
    int m_size;
};

class TypeAliasing : public Type {
public:
    explicit TypeAliasing(std::string from, std::string to) : Type(to), m_from(std::move(from)), m_to(std::move(to)) {}
    std::string m_from;
    std::string m_to;
};

class PrimitiveVariable : public Variable {
public:
    explicit PrimitiveVariable(std::string name, std::string type) : Variable(std::move(name)), m_type(std::move(type)) {}
    std::string m_type;
};


class ArrayVariable : public Variable {
public:
    explicit ArrayVariable(std::string name, std::string type) : Variable(std::move(name)), m_type(std::move(type)) {}
    std::string m_type;
};

class RecordVariable : public Variable {
public:
    explicit RecordVariable(std::string name, std::string type) : Variable(std::move(name)), m_type(std::move(type)) {}
    std::string m_type;
};

} // namespace parsing
