#pragma once

#include "AST-node.hpp"

#include <vector>
#include <string>
#include <memory>
#include <iostream>

namespace parsing {

class Variable : public Declaration {

public:
    explicit Variable(std::string name) : Declaration(GrammarUnit::VARIABLE, std::move(name)) {}
    std::weak_ptr<Expression> m_value;
};

class RoutineParameter : public Declaration {
public:
    RoutineParameter(std::string name, std::string type) : Declaration(GrammarUnit::PARAMETER, std::move(name)), m_type(std::move(type)) {}
    RoutineParameter(RoutineParameter&& param) = default;
    std::string m_type;
};

class Routine : public Declaration {
public:
    explicit Routine(std::string name) : Declaration(GrammarUnit::ROUTINE, std::move(name)) {}
    
    void print() override {
        cout << "Routine declaration, name: " << m_name << " -> " << (return_type.empty() ? "void" : return_type) << '\n';
        m_body->print();
        cout << "End of routine declaration\n";
    }

    std::shared_ptr<Body> m_body;
    std::vector<std::shared_ptr<RoutineParameter>> m_params;
    std::string return_type;
};

class Type : public Declaration {
public:
    ~Type() override = default;
    explicit Type(std::string name) : Declaration(GrammarUnit::TYPE, std::move(name)) {}
};

class RecordType : public Type {
public:
    explicit RecordType(std::string name) : Type(std::move(name)) {}
    std::vector<std::shared_ptr<Declaration>> m_fields;
};

class ArrayType : public Type {
public:
    explicit ArrayType(std::string type, std::shared_ptr<Expression> size) : Type("array"), m_type(std::move(type)), m_size(size) {}
    std::string m_type;
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
    void print() override {
        std::cout << "varname: " << m_name << " " << "type: " << m_type; 
        if (m_assigned.get()) {
            std::cout << " assigned "; m_assigned->print();
            std::cout << "\n";
        }

    }
    explicit PrimitiveVariable(std::string name, std::string type) : Variable(std::move(name)), m_type(std::move(type)) {}
    PrimitiveVariable(std::string name, std::string type, std::shared_ptr<Expression> expr) : Variable(std::move(name)), m_type(std::move(type)), m_assigned(expr) {}

    std::string m_type;
    std::shared_ptr<Expression> m_assigned;
};

class ArrayVariable : public Variable {
public:
    explicit ArrayVariable(std::string name, std::shared_ptr<ArrayType> type) : Variable(std::move(name)), m_type(std::move(type)) {}
    std::shared_ptr<ArrayType> m_type;
};

class RecordVariable : public Variable {
public:
    explicit RecordVariable(std::string name, std::string type) : Variable(std::move(name)), m_type(std::move(type)) {}
    std::string m_type;
};

} // namespace parsing
