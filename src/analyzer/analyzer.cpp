#include "analyzer.hpp"
#include "parser/AST-node.hpp"
#include "parser/declaration.hpp"
#include "parser/expression.hpp"
#include "parser/statement.hpp"
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
            switch (decl->get_grammar()) {
                case GrammarUnit::ALIAS: {
                    auto alias = std::dynamic_pointer_cast<parsing::TypeAliasing>(decl);
                    m_alias_map[alias->m_to] = std::make_shared<parsing::Type>(alias->m_from);
                    break;
                }
                case GrammarUnit::ARRAY_TYPE: {
                    auto arr_type = std::dynamic_pointer_cast<parsing::ArrayType>(decl);
                    m_alias_map[arr_type->m_name] = arr_type;
                    break;
                }
                case GrammarUnit::RECORD_TYPE: {
                    auto record_type = std::dynamic_pointer_cast<parsing::RecordType>(decl);
                    m_alias_map[record_type->m_name] = record_type;
                    break;
                }

                default:
                    break;
            }

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
            if (parameter->get_grammar() == GrammarUnit::ARRAY) {
                auto arr = std::dynamic_pointer_cast<parsing::ArrayVariable>(parameter);
                m_var_map[arr->m_name] = arr->m_type;
            } else {
                m_var_map[parameter->m_name] = convert(parameter->m_type);
            }
        }

        write_out_types(routine->m_body->m_items);

        if (!routine->m_body->m_return.get()) {
            std::cout << "<void>\n";
            continue;
        }

        auto return_type = get_expression_type(routine->m_body->m_return);

        std::cout << "Returns actually:\n";
        return_type->print();
        m_var_map[routine->m_name] = return_type;

        // Compare two results
        if (return_type->m_name == routine->return_type
            || is_primitive(return_type->m_name) && is_primitive(routine->return_type)) {
            std::cout << "<< they are equal";
        } else {
            std::cout << "<< they are not equal";
            return_type_error(routine->m_name, routine->return_type, return_type->gr_to_str());
        }
        std::cout << "\n\n";
    }

//    print_aliases();
//    std::cout << "\n";
//    print_map();
}

std::shared_ptr<parsing::Type> CheckTypes::convert(std::string& type) {
    if (is_primitive(type)) {
        return std::make_shared<parsing::Type>(type);
    } else {
        if (m_alias_map.count(type) > 0) {
            return m_alias_map[type];
        } else {
            print_error("Unknown type: " + type);
            return nullptr;
        }
    }
}

void CheckTypes::write_out_types(std::vector<std::shared_ptr<parsing::ASTNode>>& nodes) {
    for (auto& code_node : nodes) {
        if (code_node->get_grammar() == GrammarUnit::VARIABLE) {
            auto variable = std::dynamic_pointer_cast<parsing::PrimitiveVariable>(code_node);
            m_var_map[variable->m_name] = convert(variable->m_type);

            if (variable->m_assigned.get()) {
                // Check for correctness of assignments
                std::cout << "Checking initializing of " + variable->m_name + " (" + variable->m_type + ")...\n";
                auto result_type = get_expression_type(variable->m_assigned);
                result_type->print();

                if (result_type->m_name != variable->m_type) {
                    if (!is_primitive(result_type->m_name) || !is_primitive(variable->m_type)) {
                        print_error("Invalid initializing of " + variable->m_name + " (" + variable->m_type + ")");
                    }
                }
                std::cout << "Initializing is valid\n";
            }

        }
        if (code_node->get_grammar() == GrammarUnit::ARRAY) {
            auto arr = std::dynamic_pointer_cast<parsing::ArrayVariable>(code_node);
            m_var_map[arr->m_name] = arr->m_type;
        }

        // Check for correctness of assignments
        if (code_node->get_grammar() == GrammarUnit::ASSIGNMENT) {
            auto assignment = std::dynamic_pointer_cast<parsing::Assignment>(code_node);
            auto result_type = get_expression_type(assignment->m_expression);

            auto id_name = assignment->m_modifiable->m_head_name;
            std::cout << "Checking assignment to var " + id_name + " ...\n";
            result_type->print();

            if (m_var_map.count(id_name) == 0 || result_type->m_name != m_var_map[id_name]->m_name) {
                if (!is_primitive(result_type->m_name) || !is_primitive(m_var_map[id_name]->m_name)) {
                    print_error("Invalid assignment at " + id_name);
                }
            }
            std::cout << "Assignment is valid\n";
        }
    }
}

std::shared_ptr<parsing::Type> CheckTypes::get_expression_type(std::shared_ptr<parsing::Expression> &expression) {
    switch (expression->get_grammar()) {
        case GrammarUnit::INTEGER:
            return std::make_shared<parsing::Type>("integer");
        case GrammarUnit::REAL:
            return std::make_shared<parsing::Type>("real");
        case GrammarUnit::TRUE:
        case GrammarUnit::FALSE:
        case GrammarUnit::BOOL:
            return std::make_shared<parsing::Type>("bool");
        case GrammarUnit::IDENTIFIER: {
            auto variable = std::dynamic_pointer_cast<parsing::Modifiable>(expression);

            std::shared_ptr<parsing::Type> cur_type = m_var_map[variable->m_head_name];

            for (auto& node : variable->m_chain) {
                bool aliasing_case = true;
                while (aliasing_case) {
                    aliasing_case = false;

                    switch (cur_type->get_grammar()) {
                        case GrammarUnit::RECORD_TYPE: {
                            auto access = std::dynamic_pointer_cast<parsing::Modifiable::RecordAccess>(node);
                            if (access == nullptr) {
                                print_error("Invalid accessing a record: " + variable->m_head_name);
                                return nullptr;
                            }

                            auto record = std::dynamic_pointer_cast<parsing::RecordType>(cur_type);

                            bool field_found = false;
                            for (auto& field : record->m_fields) {
                                if (field->m_name == access->identifier) {
                                    field_found = true;

                                    if (field->get_grammar() == GrammarUnit::VARIABLE) {
                                        auto var_field = std::dynamic_pointer_cast<parsing::PrimitiveVariable>(field);
                                        cur_type = std::make_shared<parsing::Type>(var_field->m_type);
                                    } else if (field->get_grammar() == GrammarUnit::ARRAY) {
                                        auto var_field = std::dynamic_pointer_cast<parsing::ArrayVariable>(field);
                                        cur_type = var_field->m_type;
                                    } else {
                                        print_error("Unknown type of a field in record: " + field->m_name);
                                        return nullptr;
                                    }

                                    break;
                                }
                            }

                            if (!field_found) {
                                print_error("Record " + record->m_name + " has no attribute " + access->identifier);
                                return nullptr;
                            }

                            break;
                        }
                        case GrammarUnit::ARRAY_TYPE: {
                            auto access = std::dynamic_pointer_cast<parsing::Modifiable::ArrayAccess>(node);
                            if (access == nullptr) {
                                print_error("Invalid accessing an array: " + variable->m_head_name);
                                return nullptr;
                            }

                            // Check whether accessing is performed with integer or not
                            auto access_type = get_expression_type(access->access);
                            if (access_type->m_name != "integer") {
                                print_error("Impossible to access an array with non-integer value: " + access_type->m_name);
                                return nullptr;
                            }

                            auto array = std::dynamic_pointer_cast<parsing::ArrayType>(cur_type);
                            cur_type = std::make_shared<parsing::Type>(array->m_type);

                            break;
                        }
                        case GrammarUnit::DEFAULT_TYPE:
                        case GrammarUnit::VARIABLE: {
                            auto identifier = cur_type;

                            if (is_primitive(identifier->m_name)) {
                                print_error("Impossible to access [ ] or . a primitive variable: " + variable->m_head_name);
                                return nullptr;
                            }

                            cur_type = convert(identifier->m_name);

                            break;
                        }
                        default:
                            print_error("Error in chain of identifier");
                    }

                    if (!is_primitive(cur_type->m_name)) {
                        aliasing_case = true;
                    }

                    if (cur_type->get_grammar() == GrammarUnit::ARRAY_TYPE || cur_type->get_grammar() == GrammarUnit::RECORD_TYPE) {
                        aliasing_case = false;
                    }
                }
            }

            return cur_type;
        }
        case GrammarUnit::ROUTINE_CALL: {
            auto call = std::dynamic_pointer_cast<parsing::RoutineCallResult>(expression);
            if (m_var_map.count(call->m_routine_call->m_routine_name) == 0) {
                print_error("Call of undeclared or void function: " + call->m_routine_call->m_routine_name);
                return nullptr;
            }
            return m_var_map[call->m_routine_call->m_routine_name];
        }
        default: {
            auto math = std::dynamic_pointer_cast<parsing::Math>(expression);
            if (math == nullptr) {
                print_error("Expression contains something wrong");
            }

            auto left_type = get_expression_type(math->m_left);
            auto right_type = get_expression_type(math->m_right);

            if (left_type->get_grammar() != right_type->get_grammar()
                || (left_type->m_name != right_type->m_name
                    && (!is_primitive(left_type->m_name) || !is_primitive(right_type->m_name)))) {

                std::cout << "Left part:\n";
                left_type->print();

                std::cout << "\nRight part:\n";
                right_type->print();
                print_error("Expression performs " + math->gr_to_str() + " on elements of different types");

                return nullptr;
            }

            if (left_type->get_grammar() != GrammarUnit::DEFAULT_TYPE) {
                std::cout << "Invalid node at an expression:\n";
                left_type->print();

                print_error("Impossible to perform actions on non-primitive types");
            }

            return left_type;
        }
    }
}
