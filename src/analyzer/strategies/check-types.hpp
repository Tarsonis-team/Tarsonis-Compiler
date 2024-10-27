#pragma once

#include "parser/AST-node.hpp"
#include <string>
#include <vector>
#include <unordered_map>

enum class TypeFormat {
    PRIMITIVE,
    RECORD,
    ARRAY
};

class TypeNode {
public:
    virtual TypeFormat get_format();
    virtual void print();
    explicit TypeNode() = default;
    virtual ~TypeNode() = default;
};

class PrimitiveTypeNode : public TypeNode {
public:
    TypeFormat get_format() override {
        return TypeFormat::PRIMITIVE;
    }
    void print() override {
        std::cout << "Primitive variable of type " << m_type << "\n";
    }

    explicit PrimitiveTypeNode(std::string type) : TypeNode(), m_type(std::move(type)) {}
    std::string m_type;
};

class ArrayTypeNode : public TypeNode {
public:
    TypeFormat get_format() override {
        return TypeFormat::ARRAY;
    }
    void print() override {
        std::cout << "Array of type " << m_item_type << " with " << m_dimensions << " dimensions\n";
    }

    explicit ArrayTypeNode(std::string type, int d) : TypeNode(), m_item_type(std::move(type)), m_dimensions(d) {}
    std::string m_item_type;
    int m_dimensions;
};

class RecordTypeNode : public TypeNode {
public:
    TypeFormat get_format() override {
        return TypeFormat::RECORD;
    }
    void print() override {
        std::cout << "Record with fields:\n";
        for (auto& field : m_fields) {
            field->print();
        }
    }

    explicit RecordTypeNode() : TypeNode() {}
    std::vector<std::shared_ptr<TypeNode>> m_fields;
};



struct CheckTypes
{
    void check_tree();
    void write_out_types(std::vector<std::shared_ptr<parsing::ASTNode>>& nodes);

    explicit CheckTypes(std::shared_ptr<parsing::Program> program) : m_ast(program) {
        std::cout << "\nChecking return types of functions...\n";
        check_tree();
        std::cout << "\nAll return types of a given tree are valid\n";
    }

    std::vector<std::string> checkErrors()
    {
        return {};
    }

    void return_type_error(const std::string& function_name, const std::string& expected, const std::string& actual) {
        std::cout << "\nERROR: Return expression of a function " << function_name << " contains " << actual
            << " while " << expected << " was expected\n";

        throw std::runtime_error("Invalid return type of function " + function_name);
    }

    std::shared_ptr<parsing::Program> m_ast;
    std::unordered_map<std::string, std::shared_ptr<TypeNode>> m_types_map;

    void print_map() {
        for (auto& entry : m_types_map) {
            entry.second->print();
        }
    }
};
