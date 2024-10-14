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
    std::shared_ptr<Expression> m_value;
    void print() override {
        cout << "VAR:" << m_name << " ";
        if (m_value.get()) {
            cout << "value: ";
            m_value->print();
        }
        cout << "\n";
    }
};

class RoutineParameter : public Declaration {
public:
    RoutineParameter(std::string name, std::string type) : Declaration(GrammarUnit::PARAMETER, std::move(name)), m_type(std::move(type)) {}
    RoutineParameter(RoutineParameter&& param) = default;
    std::string m_type;

    void print() override {
        cout << "VAR_PAR:" << m_name;
    }
};

class Routine : public Declaration {
public:
    explicit Routine(std::string name) : Declaration(GrammarUnit::ROUTINE, std::move(name)) {}
    
    void print() override {
        cout << "Routine declaration, name: " << m_name << " -> " << (return_type.empty() ? "void" : return_type) << '\n';
        m_body->print();

        if (m_body->m_return.get()) {
            cout << "Returns: ";
            m_body->m_return->print();
        } else {
            cout << "Returns nothing";
        }

        cout << "\n";
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
    void print() override {
        cout << "TYPE:" << m_name << "\n";
    }
};

class RecordType : public Type {
public:
    explicit RecordType(std::string name) : Type(std::move(name)) {}
    std::vector<std::shared_ptr<Declaration>> m_fields;
    void print() override {
        cout << "TYPE_RECORD:" << m_name << ":\n";
        for (auto& field : m_fields) {
            cout << " ";
            field->print();
        }
        cout << "\n";
    }
};

class ArrayType : public Type {
public:
    explicit ArrayType(std::string type, std::shared_ptr<Expression> size) : Type("array"), m_type(std::move(type)), m_size(size) {}
    std::string m_type;
    std::shared_ptr<Expression> m_size;
    void print() override {
        std::cout << "type: " << m_type << " size: "; m_size->print();
    }
};

class TypeAliasing : public Type {
public:
    explicit TypeAliasing(std::string from, std::string to) : Type(to), m_from(std::move(from)), m_to(std::move(to)) {}
    std::string m_from;
    std::string m_to;
};


class PrimitiveVariable : public Variable {
public:
    void print() override {
        std::cout << "varname: " << m_name << " " << "type: " << m_type; 
        if (m_assigned.get()) {
            std::cout << " assigned "; m_assigned->print();
        }
        std::cout << "\n";
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
    void print() override {
        std::cout << "Array " << m_name << " "; m_type->print();
        std::cout << "\n";

    }
};

} // namespace parsing
