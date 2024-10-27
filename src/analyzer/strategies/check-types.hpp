#pragma once

#include "parser/AST-node.hpp"
#include <string>
#include <vector>
#include <unordered_map>

enum class ReturnTypeFormat {
    PRIMITIVE,
    RECORD,
    ARRAY,
    INVALID
};

class ReturnType {
public:
    virtual ReturnTypeFormat get_format() {
        return ReturnTypeFormat::INVALID;
    }
    virtual void print() {

    }
    virtual std::string get_string() {
        return "<invalid>";
    }
    explicit ReturnType() = default;
    ~ReturnType() = default;

    virtual bool equal_to(std::string& target_type) {
        return false;
    }
    virtual bool equal_to(std::shared_ptr<ReturnType>& another) {
        return this->get_format() == another->get_format();
    }
};

class PrimitiveReturn : public ReturnType {
public:
    ReturnTypeFormat get_format() override {
        return ReturnTypeFormat::PRIMITIVE;
    }
    void print() override {
        std::cout << "Primitive variable of type " << m_type << "\n";
    }
    bool equal_to(std::string& target_type) override {
        return m_type == target_type;
    }
    bool equal_to(std::shared_ptr<ReturnType>& another) override {
        if (this->get_format() != another->get_format()) {
            return false;
        }

        auto primitive = std::dynamic_pointer_cast<PrimitiveReturn>(another);
        return this->m_type == primitive->m_type;
    }
    std::string get_string() override {
        return m_type;
    }

    explicit PrimitiveReturn(std::string type) : ReturnType(), m_type(std::move(type)) {}
    std::string m_type;
};

class ArrayReturn : public ReturnType {
public:
    ReturnTypeFormat get_format() override {
        return ReturnTypeFormat::ARRAY;
    }
    void print() override {
        std::cout << "Array of type " << m_item_type << "\n";
    }
    bool equal_to(std::string& target_type) override {
        // ?
        return false;
    }
    bool equal_to(std::shared_ptr<ReturnType>& another) override {
        if (this->get_format() != another->get_format()) {
            return false;
        }

        auto arr = std::dynamic_pointer_cast<ArrayReturn>(another);
        return this->m_item_type == arr->m_item_type;
    }
    std::string get_string() override {
        return m_item_type;
    }

    explicit ArrayReturn(std::string type) : ReturnType(), m_item_type(std::move(type)) {}
    std::string m_item_type;
};

class RecordTypeReturn : public ReturnType {
public:
    ReturnTypeFormat get_format() override {
        return ReturnTypeFormat::RECORD;
    }
    void print() override {
        std::cout << "Record " << m_record_name << " with fields:\n";
        for (auto& field : m_fields) {
            std::cout << " {" << field.first << "}: ";
            field.second->print();
        }
    }
    bool equal_to(std::string& target_type) override {
        return m_record_name == target_type;
    }
    bool equal_to(std::shared_ptr<ReturnType>& another) override {
        if (this->get_format() != another->get_format()) {
            return false;
        }

        auto record = std::dynamic_pointer_cast<RecordTypeReturn>(another);
        return this->m_record_name == record->m_record_name;
    }
    std::string get_string() override {
        return "Record " + m_record_name;
    }

    explicit RecordTypeReturn(std::string name) : ReturnType(), m_record_name(std::move(name)) {}
    std::string m_record_name;
    std::unordered_map<std::string, std::shared_ptr<ReturnType>> m_fields;
};



struct CheckTypes
{
    void check_tree();
    void write_out_types(std::vector<std::shared_ptr<parsing::ASTNode>>& nodes);

    explicit CheckTypes(std::shared_ptr<parsing::Program> program) : m_ast(program) {
        std::cout << "\nChecking return types of functions...\n\n";
        check_tree();
        std::cout << "\nAll return types of a given tree are valid\n\n";
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

    void print_error(const std::string& err) {
        throw std::runtime_error("ERROR: " + err);
    }

    std::shared_ptr<parsing::Program> m_ast;
    std::unordered_map<std::string, std::shared_ptr<ReturnType>> m_var_map;
    std::unordered_map<std::string, std::shared_ptr<ReturnType>> m_alias_map;

    void print_map() {
        for (auto& entry : m_var_map) {
            std::cout << "[var " << entry.first << "]: ";
            entry.second->print();
        }
    }

    void print_aliases() {
        for (auto& entry : m_alias_map) {
            std::cout << "[type " << entry.first << "]: ";
            entry.second->print();
        }
    }

    bool is_primitive(std::string& type) {
        return type == "integer" || type == "bool" || type == "real";
    }

    std::shared_ptr<ReturnType> convert(std::string& type);
    std::shared_ptr<ReturnType> get_expression_type(std::shared_ptr<parsing::Expression>& expression);
};
