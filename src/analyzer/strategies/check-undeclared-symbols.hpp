#pragma once

#include "parser/AST-node.hpp"
#include "parser/grammar-units.hpp"
#include <exception>
#include <memory>
#include <string>
#include <vector>

struct CheckUndeclaredSymbols
{
    explicit CheckUndeclaredSymbols(std::shared_ptr<parsing::Program> program) : m_ast(program) {}

    std::vector<std::string> checkErrors()
    {
        // what this table is supposed to hold?
        // typename -> object of the type
        // varname  -> object of the type
        std::unordered_map<std::string, std::shared_ptr<parsing::Declaration>> table;
        table.emplace("integer", std::make_shared<parsing::Declaration>(GrammarUnit::INTEGER, "integer"));
        table.emplace("real", std::make_shared<parsing::Declaration>(GrammarUnit::REAL, "real"));
        table.emplace("boolean", std::make_shared<parsing::Declaration>(GrammarUnit::BOOL, "boolean"));
        table.emplace("array", std::make_shared<parsing::Declaration>(GrammarUnit::ARRAY, "array"));

        try {
            m_ast->checkUndeclared(table);
        } 
        catch (const std::exception& err) {
            return { err.what() };
        }
        std::cout << "CheckUndeclaredSymbols reports no errors.\n";
        return { };
    }

    std::shared_ptr<parsing::Program> m_ast;
};
