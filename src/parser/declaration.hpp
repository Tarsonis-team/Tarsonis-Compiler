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
    
    std::shared_ptr<Body> m_body;
    std::vector<std::shared_ptr<RoutineParameter>> m_params;
    std::string return_type;
};

class Type : public Declaration {
public:
    explicit Type(std::string name) : Declaration(GrammarUnit::TYPE, std::move(name)) {}
};

class RecordType : public Type {
public:
    explicit RecordType(std::string& name) : Type(name) {}
    std::vector<std::unique_ptr<Declaration>> m_fields;
};

class ArrayType : public Type {
public:
    explicit ArrayType(Type type, std::shared_ptr<Expression> size) : Type("array"), m_type(std::move(type)), m_size(size) {}
    Type m_type;
    std::shared_ptr<Expression> m_size;
};

class TypeAliasing : public Type {
public:
    explicit TypeAliasing(std::string from, std::string to) : Type(to), m_from(std::move(from)), m_to(std::move(to)) {}
    std::string m_from;
    std::string m_to;
};

class IntType : public Type {
public:
    explicit IntType() : Type("int"){}
};


class RealType : public Type {
public:
    explicit RealType() : Type("real"){}
};

class BoolType : public Type {
public:
    explicit BoolType() : Type("bool"){}
};

class CustomType : public Type {
public:
    explicit CustomType(std::string identifier) : Type("custom"), m_identifier(std::move(identifier)){}
    std::string m_identifier;
};

class PrimitiveVariable : public Variable {
public:
    explicit PrimitiveVariable(std::string name, std::string type) : Variable(std::move(name)), m_type(std::move(type)) {}
    std::string m_type;
};


class ArrayVariable : public Variable {
public:
    explicit ArrayVariable(std::string name, Type type) : Variable(std::move(name)), m_type(std::move(type)) {}
    Type m_type;
};

class RecordVariable : public Variable {
public:
    explicit RecordVariable(std::string name, RecordType) : Variable(std::move(name)), m_type(std::move(type)) {}
    RecordType m_type;
};

} // namespace parsing
