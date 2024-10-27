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
                    m_alias_map[alias->m_to] = std::make_shared<PrimitiveReturn>(alias->m_from);
                    break;
                }
                case GrammarUnit::ARRAY_TYPE: {
                    auto arr_type = std::dynamic_pointer_cast<parsing::ArrayType>(decl);
                    m_alias_map[arr_type->m_name] = std::make_shared<ArrayReturn>(arr_type->m_type);
                    break;
                }
                case GrammarUnit::RECORD_TYPE: {
                    auto record = std::dynamic_pointer_cast<parsing::RecordType>(decl);
                    auto record_type = std::make_shared<RecordTypeReturn>(record->m_name);

                    for (auto& field : record->m_fields) {
                        if (field->get_grammar() == GrammarUnit::ARRAY) {
                            auto field_array = std::dynamic_pointer_cast<parsing::ArrayVariable>(field);
                            auto entry = std::make_shared<ArrayReturn>(field_array->m_type->m_type);
                            record_type->m_fields[field->m_name] = entry;
                        } else {
                            auto field_var = std::dynamic_pointer_cast<parsing::PrimitiveVariable>(field);
                            record_type->m_fields[field->m_name] = convert(field_var->m_type);
                        }
                    }

                    m_alias_map[record->m_name] = record_type;
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
                m_var_map[arr->m_name] = std::make_shared<ArrayReturn>(arr->m_type->m_type);
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

        if (return_type->equal_to(routine->return_type)) {
            std::cout << "<< they are equal";
        } else {
            std::cout << "<< they are not equal";
            return_type_error(routine->m_name, routine->return_type, return_type->get_string());
        }
        std::cout << "\n\n";
    }

//    print_aliases();
//    std::cout << "\n";
//    print_map();
}

std::shared_ptr<ReturnType> CheckTypes::convert(std::string& type) {
    if (is_primitive(type)) {
        return std::make_shared<PrimitiveReturn>(type);
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
        }
        if (code_node->get_grammar() == GrammarUnit::ARRAY) {
            auto arr = std::dynamic_pointer_cast<parsing::ArrayVariable>(code_node);
            m_var_map[arr->m_name] = std::make_shared<ArrayReturn>(arr->m_type->m_type);
        }
    }
}

std::shared_ptr<ReturnType> CheckTypes::get_expression_type(std::shared_ptr<parsing::Expression> &expression) {
    switch (expression->get_grammar()) {
        case GrammarUnit::INTEGER:
            return std::make_shared<PrimitiveReturn>("integer");
        case GrammarUnit::REAL:
            return std::make_shared<PrimitiveReturn>("real");
        case GrammarUnit::TRUE:
        case GrammarUnit::FALSE:
        case GrammarUnit::BOOL:
            return std::make_shared<PrimitiveReturn>("bool");
        case GrammarUnit::IDENTIFIER: {
            auto variable = std::dynamic_pointer_cast<parsing::Modifiable>(expression);

            std::shared_ptr<ReturnType> cur_item = m_var_map[variable->m_head_name];

            for (auto& node : variable->m_chain) {
                bool aliasing_case = true;
                while (aliasing_case) {
                    aliasing_case = false;

                    switch (cur_item->get_format()) {
                        case ReturnTypeFormat::RECORD: {
                            auto access = std::dynamic_pointer_cast<parsing::Modifiable::RecordAccess>(node);
                            if (access == nullptr) {
                                print_error("Invalid accessing a record: " + variable->m_head_name);
                                return nullptr;
                            }

                            auto record = std::dynamic_pointer_cast<RecordTypeReturn>(cur_item);

                            if (record->m_fields.count(access->identifier) == 0) {
                                print_error("Record " + record->m_record_name + " has no attribute " + access->identifier);
                                return nullptr;
                            }

                            cur_item = record->m_fields[access->identifier];
                            break;
                        }
                        case ReturnTypeFormat::ARRAY: {
                            auto access = std::dynamic_pointer_cast<parsing::Modifiable::ArrayAccess>(node);
                            if (access == nullptr) {
                                print_error("Invalid accessing an array: " + variable->m_head_name);
                                return nullptr;
                            }

                            // Check whether accessing is performed with integer or not
                            auto access_type = std::dynamic_pointer_cast<PrimitiveReturn>(get_expression_type(access->access));
                            if (access_type == nullptr || access_type->m_type != "integer") {
                                print_error("Impossible to access an array with non-integer value: " + access_type->m_type);
                                return nullptr;
                            }

                            auto array = std::dynamic_pointer_cast<ArrayReturn>(cur_item);
                            cur_item = std::make_shared<PrimitiveReturn>(array->m_item_type);

                            break;
                        }
                        case ReturnTypeFormat::PRIMITIVE: {
                            auto identifier = std::dynamic_pointer_cast<PrimitiveReturn>(cur_item);

                            if (is_primitive(identifier->m_type)) {
                                print_error("Impossible to access [ ] or . a primitive variable: " + variable->m_head_name);
                                return nullptr;
                            }

                            cur_item = convert(identifier->m_type);
                            aliasing_case = true;
                            break;
                        }
                        default:
                            print_error("Error in chain of identifier");
                    }
                }
            }

            return cur_item;
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

            if (!left_type->equal_to(right_type)) {
                std::cout << "Left part:\n";
                left_type->print();

                std::cout << "\nRight part:\n";
                right_type->print();
                print_error("Expression performs " + math->gr_to_str() + " on elements of different types");

                return nullptr;
            }

            if (left_type->get_format() != ReturnTypeFormat::PRIMITIVE) {
                std::cout << "Invalid node at an expression:\n";
                left_type->print();

                print_error("Impossible to perform actions on non-primitive types");
            }

            return left_type;
        }
    }
}
