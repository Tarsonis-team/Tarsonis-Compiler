#pragma once

#include "parser/AST-node.hpp"
#include "parser/declaration.hpp"
#include <string>
#include <vector>
#include <unordered_map>


struct CheckTypes
{
    void check_tree();
    void write_out_types(std::vector<std::shared_ptr<parsing::ASTNode>>& nodes);

    explicit CheckTypes(std::shared_ptr<parsing::Program>& program) : m_ast(program) {
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
    std::unordered_map<std::string, std::shared_ptr<parsing::Type>> m_var_map;
    std::unordered_map<std::string, std::shared_ptr<parsing::Type>> m_alias_map;

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

    std::shared_ptr<parsing::Type> convert(std::string& type);
    std::shared_ptr<parsing::Type> get_expression_type(std::shared_ptr<parsing::Expression>& expression);
};
