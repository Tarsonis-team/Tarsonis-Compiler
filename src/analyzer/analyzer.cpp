#include "analyzer.hpp"
#include "parser/AST-node.hpp"
#include "parser/declaration.hpp"
#include <memory>

#include "analyzer/strategies/check-types.hpp"

/*
 * Collect symbol table
*/
Analyzer::Analyzer(std::shared_ptr<parsing::Program> program) : m_program(program)
{
}

void CheckTypes::check_tree() {
    for (auto& decl : m_ast->m_declarations) {
        if (decl->get_grammar() != GrammarUnit::ROUTINE) {
            // TODO: records
            continue;
        }

        std::shared_ptr<parsing::Routine> routine = std::dynamic_pointer_cast<parsing::Routine>(decl);

        std::cout << "ROUTINE " << routine->m_name << " should return ";
        if (routine->return_type.empty()) {
            std::cout << "nothing (void)";

            // void returns something
            if (routine->m_body->m_return.get()) {
                std::cout << "\n";
                return_type_error(routine->m_name, "VOID", "SOME_EXPRESSION");
            }
        } else {
            std::cout << routine->return_type;
        }
        std::cout << std::endl;

        for (auto& parameter : routine->m_params) {
            if (parameter->m_type == "integer" || parameter->m_type == "bool" || parameter->m_type == "real") {
//                m_types_map[parameter->m_name] = std::make_shared<PrimitiveTypeNode>(parameter->m_type);
            }

        }

        write_out_types(routine->m_body->m_items);
    }
}

void CheckTypes::write_out_types(std::vector<std::shared_ptr<parsing::ASTNode>>& nodes) {
    for (auto& code_node : nodes) {
        if (true) {}
    }
}
