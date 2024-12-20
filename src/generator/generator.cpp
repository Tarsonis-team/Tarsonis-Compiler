#include "generator.hpp"

#include "parser/visitor/abstract-visitor.hpp"
#include "parser/statement.hpp"
#include "parser/return.hpp"
#include "parser/std-function.hpp"
#include "llvm/Analysis/StackSafetyAnalysis.h"
#include <llvm-14/llvm/IR/Function.h>

#include <vector>

namespace generator {

llvm::Type* Generator::typenameToType(const std::string& name) {
    if (name.empty()) {
        return llvm::Type::getVoidTy(context);;
    }
    return m_type_table.at(name);
}

std::string get_array_typename(const std::string& inner_type, int size) {
    return "array_" + inner_type + "_" + std::to_string(size);
}

void Generator::gen_expr_fork(parsing::Math& node, llvm::Value*& left, llvm::Value*& right) {
    std::cout << "Generating expression for " << node.gr_to_str() << "...\n";

    is_lvalue = false;
    node.m_left->accept(*this);
    left = current_expression;

    is_lvalue = false;
    node.m_right->accept(*this);
    right = current_expression;
}

void Generator::apply() {
    m_type_table.emplace("integer", llvm::Type::getInt32Ty(context));
    m_type_table.emplace("real", llvm::Type::getDoubleTy(context));
    m_type_table.emplace("boolean", llvm::Type::getInt1Ty(context));

    m_tree->accept(*this);

    std::cout << "\n";

    module->print(llvm::outs(), nullptr);

    std::error_code EC;
    llvm::raw_fd_ostream fileStream("output.ll", EC);

    if (EC) {
        llvm::errs() << "Error opening file: " << EC.message() << "\n";
        return;
    }

    module->print(fileStream, nullptr);
    fileStream.close();

    llvm::outs() << "LLVM IR written to output.ll\n";
}

void Generator::visit(parsing::If& node) {
    // Go the parent function (last point) inside CFG
    llvm::Function* parent_func = builder.GetInsertBlock()->getParent();

    node.m_condition->accept(*this);
    llvm::Value* cond = current_expression;

    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(context, "then", parent_func);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(context, "else");
    llvm::BasicBlock* skipBB = llvm::BasicBlock::Create(context, "ifskip");

    if (node.m_else) {
        // if we do have else part - then we create that branching
        builder.CreateCondBr(cond, thenBB, elseBB);
    } else {
        // otherwise we don't. and if condition is false, we skip 
        // the if statement
        builder.CreateCondBr(cond, thenBB, skipBB);
    }

    // generating THEN part
    builder.SetInsertPoint(thenBB);
    node.m_then->accept(*this);
    builder.CreateBr(skipBB);

    if (node.m_else) {
        // else is generated only in case it exists
        parent_func->getBasicBlockList().push_back(elseBB);
        builder.SetInsertPoint(elseBB);
        node.m_else->accept(*this);
        builder.CreateBr(skipBB);
    }

    parent_func->getBasicBlockList().push_back(skipBB);
    builder.SetInsertPoint(skipBB);
}

void Generator::visit(parsing::ASTNode& node) {}

void Generator::visit(parsing::Declaration& node) {}

void Generator::visit(parsing::Type& node) {}

void Generator::visit(parsing::RecordType& node) {
    std::cout << "Generating a record " << node.m_name << "...\n";

    m_ast_decl_table[node.m_name] = std::make_shared<parsing::RecordType>(node);

    std::vector<llvm::Type*> struct_fields;
    std::vector<std::string> field_names;

    for (auto& field : node.m_fields) {
        auto var_field = std::dynamic_pointer_cast<parsing::Variable>(field);
        struct_fields.push_back(typenameToType(var_field->m_type->m_name));
        field_names.push_back(field->m_name);
    }

    llvm::StructType* struct_type = llvm::StructType::create(context, struct_fields, node.m_name);
    m_type_table[node.m_name] = struct_type;
    m_records_table[node.m_name] = field_names;
}

void Generator::visit(parsing::ArrayType& node) {
    auto inner_type_str = node.m_type->m_name;

    // if (m_type_table.contains(get_array_typename(inner_type_str)))
    // {
    //     // this type was created before
    //     return;
    // }

    std::cout << "Generating array type with inner type: " << inner_type_str << "...\n";

    m_ast_decl_table[node.m_name] = std::make_shared<parsing::ArrayType>(node);

    auto *inner_type = typenameToType(inner_type_str);
    // llvm::Type* ptr = nullptr;

    // if (inner_type->isIntegerTy()) {
    //     ptr = llvm::Type::getInt32PtrTy(context);
    // } else if (inner_type->isDoubleTy()) {
    //     ptr = llvm::Type::getDoublePtrTy(context);
    // } else if (inner_type->isIntegerTy(1)) {
    //     // bool
    //     ptr = llvm::Type::getInt1PtrTy(context);
    // } else {
    //     // complex
    //     ptr = llvm::PointerType::getUnqual(inner_type);
    // }

    node.m_size->accept(*this);
    auto *array_size = llvm::dyn_cast<llvm::ConstantInt>(current_expression);
    if (!array_size) {
        throw std::runtime_error("Array size must be a constant integer for static allocation.");
    }

    llvm::ArrayType* array_type = llvm::ArrayType::get(inner_type, array_size->getZExtValue());

    node.m_generated_size = array_size->getZExtValue();

    m_type_table.emplace(get_array_typename(inner_type_str, array_size->getZExtValue()), array_type);
}

void Generator::visit(parsing::Variable& node) {}

void Generator::visit(parsing::ArrayVariable& node) {
    std::cout << "Generating array var...\n";

    m_ast_decl_table[node.m_name] = node.m_type;

    node.m_type->accept(*this);

    auto *element_type = typenameToType(node.m_type->m_type->m_name);
    auto *array_type = m_type_table[get_array_typename(node.m_type->m_type->m_name, node.m_type->m_generated_size)];

    llvm::AllocaInst* arr_var = builder.CreateAlloca(array_type, nullptr, node.m_name);
    m_var_table[node.m_name] = arr_var;
}

void Generator::visit(parsing::PrimitiveVariable& node) {
    // this is never an array, arrays get dispathed into ArrayVariable visit method
    std::cout << "Generating non-array variable " << node.m_name << " of type: " << node.m_type->m_name << "...\n";

    m_ast_decl_table[node.m_name] = node.m_type;

    llvm::AllocaInst *var = builder.CreateAlloca(typenameToType(node.m_type->m_name), nullptr, node.m_name);

    if (node.m_value) {
        node.m_value->accept(*this);
        builder.CreateStore(current_expression, var);
    }
    m_var_table[node.m_name] = var;

    if (m_records_table.contains(node.m_type->m_name)) {
        std::cout << node.m_name << " -> " << node.m_type->m_name << "\n";
        m_recordnames_table[node.m_name] = node.m_type->m_name;
    }
}

void Generator::visit(parsing::Body& node) {
    auto outer_scope = m_var_table;
    for(const auto& stmt : node.m_items) {
        stmt->accept(*this);
    }
    m_var_table = std::move(outer_scope);
}

void Generator::visit(parsing::Routine& node) {
    // Generate types of arguments
    std::cout << "\nGenerating routine " << node.m_name << "...\n\n";

    std::vector<llvm::Type*> arg_types;
    arg_types.reserve(node.m_params.size());
    for (const auto& param : node.m_params) {
        arg_types.push_back(typenameToType(param->m_type));
    }

    // function type generation
    auto* ft = llvm::FunctionType::get(typenameToType(node.return_type), arg_types, false);
    current_function = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, node.m_name, module.get());

    m_routine_table[node.m_name] = current_function;

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", current_function);
    builder.SetInsertPoint(BB);
    // arguments generation
    int arg_idx = 0;
    for (auto &arg : node.m_params) {
        llvm::AllocaInst *space = builder.CreateAlloca(typenameToType(arg->m_type), nullptr, arg->m_name);
        m_var_table[arg->m_name] = space;

        if (m_ast_decl_table.contains(arg->m_type)) {
            m_ast_decl_table[arg->m_name] = m_ast_decl_table.at(arg->m_type);
        }

        if (m_records_table.contains(arg->m_type)) {
            m_recordnames_table[arg->m_name] = arg->m_type;
        }

        arg->accept(*this);

        llvm::Value* arg_value = current_function->getArg(arg_idx);
        builder.CreateStore(arg_value, space);

        arg_idx++;
    }

    node.m_body->accept(*this);

    if (node.return_type.empty()) {
        std::cout << "RETURN VOID\n";
        builder.CreateRetVoid();
        return;
    }

    // verifyFunction(*current_function);
}

void Generator::visit(parsing::RoutineCall& node) {
    std::cout << "Generating routine call...\n";

    if (!m_routine_table.contains(node.m_routine_name)) {
        throw std::runtime_error("Routine doesn't exist: " + node.m_routine_name); 
    }
    llvm::Function* routine = m_routine_table[node.m_routine_name];

    // checking number of arguments
    if (routine->arg_size() != node.m_parameters.size()) {
        throw std::runtime_error("Invalid number of arguments for a routine call: " + node.m_routine_name); 
    }

    // create a vector of arguments
    std::vector<llvm::Value*> params;
    for (auto& par : node.m_parameters) {
        is_lvalue = false;
        par->accept(*this);
        params.push_back(current_expression);
    }

    current_expression = builder.CreateCall(routine, params, "call_" + node.m_routine_name);
}

void Generator::visit(parsing::RoutineCallResult& node) {
    node.m_routine_call->accept(*this);
}

void Generator::visit(parsing::RoutineParameter& node) {
    // llvm::AllocaInst *space = builder.CreateAlloca(typenameToType(node.m_type), nullptr, node.m_name);
    // m_var_table[node.m_name] = space;

    // not sure about first param
    // builder.CreateStore(nullptr, space);
}

void Generator::visit(parsing::Statement& node) {}

void Generator::visit(parsing::Expression& node) {}

void Generator::visit(parsing::Modifiable& node) {
    if (node.m_chain.empty()) {
        auto *var = m_var_table.at(node.m_head_name);
        llvm::Type *allocatedType = var->getAllocatedType();
        if (is_lvalue) {
            current_lvalue = var;
        } else {
            current_expression = builder.CreateLoad(allocatedType, var, node.m_head_name);
        }
        return;
    }

    auto *var = m_var_table.at(node.m_head_name);
    current_expression = var;

    std::string cur_rec_type = "";
    
    std::shared_ptr<parsing::Type> cur_chain_type = nullptr;
    if (m_records_table.contains(node.m_head_name)) {
        cur_rec_type = m_recordnames_table.at(node.m_head_name);
    }

    if (m_ast_decl_table.contains(node.m_head_name)) {
        cur_chain_type = m_ast_decl_table.at(node.m_head_name);
    }

    if (cur_chain_type == nullptr) {
        throw std::runtime_error("Unknown variable (Generator stage): " + node.m_head_name);
    }

    for (auto& item : node.m_chain) {
        if (cur_chain_type != nullptr && m_records_table.contains(cur_chain_type->m_name)) {
            cur_rec_type = cur_chain_type->m_name;
        }
        
        if (auto rec_access = std::dynamic_pointer_cast<parsing::RecordAccess>(item)) {
            rec_access->m_record_type = cur_rec_type;

            std::shared_ptr<parsing::RecordType> rec_item_type = nullptr;
            if (m_ast_decl_table.contains(cur_chain_type->m_name)) {
                rec_item_type = std::dynamic_pointer_cast<parsing::RecordType>(m_ast_decl_table.at(cur_chain_type->m_name));
            }

            if (rec_item_type == nullptr) {
                throw std::runtime_error("Unknown record type: " + cur_chain_type->m_name);
            }
            
            std::shared_ptr<parsing::Declaration> accessed_field = nullptr;
            for (auto& field : rec_item_type->m_fields) {
                if (field->m_name == rec_access->identifier) {
                    accessed_field = field;
                    break;
                }
            }

            if (accessed_field == nullptr) {
                throw std::runtime_error("Accessed field wasn't found: " + node.m_head_name);
            }

            if (auto new_arr = std::dynamic_pointer_cast<parsing::ArrayVariable>(accessed_field)) {
                // array
                cur_chain_type = new_arr->m_type->m_type;
            } else {
                auto new_primitive = std::dynamic_pointer_cast<parsing::Variable>(accessed_field);
                
                if (m_records_table.contains(new_primitive->m_type->m_name)) {
                    // record
                    cur_chain_type = m_ast_decl_table.at(new_primitive->m_type->m_name);
                } else {
                    if (m_ast_decl_table.contains(new_primitive->m_type->m_name)) {
                        // alias
                        cur_chain_type = m_ast_decl_table.at(new_primitive->m_type->m_name);
                    } else {
                        // simple
                        cur_chain_type = nullptr;
                    }
                }
            }
        }

        item->accept(*this);

        if (auto arr_access = std::dynamic_pointer_cast<parsing::ArrayAccess>(item)) {
            auto arr_item_type = std::dynamic_pointer_cast<parsing::ArrayType>(cur_chain_type);

            if (arr_item_type == nullptr) {
                if (m_ast_decl_table.contains(cur_chain_type->m_name)) {
                    arr_item_type = std::dynamic_pointer_cast<parsing::ArrayType>(m_ast_decl_table.at(cur_chain_type->m_name));   
                }
            }

            cur_chain_type = arr_item_type->m_type;
        }
    }

    if (is_lvalue) {
        current_lvalue = current_expression;
    } else {
        current_expression = builder.CreateLoad(current_access_type, current_expression, "accessed_value");
    }
}

void Generator::visit(parsing::ReturnStatement& node) {
    std::cout << "Generating return statement...\n";

    is_lvalue = false;
    node.m_expr->accept(*this);
    llvm::Value* ret_res = current_expression;

    builder.CreateRet(ret_res);
}

void Generator::visit(parsing::Range& node) {}

void Generator::visit(parsing::For& node) {
    std::cout << "\nGenerating For-loop statement...\n";

    llvm::Function* parent_function = builder.GetInsertBlock()->getParent();

    // Working with range
    llvm::Value* startValue = nullptr;
    llvm::Value* endValue = nullptr;
    llvm::Value* stepValue = nullptr;

    if (node.m_range->m_reverse) {
        stepValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), -1);
    } else {
        stepValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 1);
    }

    is_lvalue = false;
    node.m_range->m_begin->accept(*this);
    startValue = current_expression;

    is_lvalue = false;
    node.m_range->m_end->accept(*this);
    endValue = current_expression;

    // Creating identifier (var i)
    llvm::AllocaInst* iterator_var = builder.CreateAlloca(startValue->getType(), nullptr, node.m_identifier->m_name);
    builder.CreateStore(startValue, iterator_var);
    m_var_table[node.m_identifier->m_name] = iterator_var;

    // Top-level structure
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(context, "loop", parent_function);
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(context, "afterloop", parent_function);

    // parent_function->getBasicBlockList().push_back(loopBB);
    llvm::Value* for_var = builder.CreateLoad(iterator_var->getAllocatedType(), iterator_var, node.m_identifier->m_name);

    // Condition to break a loop
    llvm::Value* for_condition = builder.CreateICmpSLT(for_var, endValue, "loopcond");
    builder.CreateCondBr(for_condition, loopBB, afterBB);
    
    builder.SetInsertPoint(loopBB);
    node.m_body->accept(*this);

    // i++
    for_var = builder.CreateLoad(iterator_var->getAllocatedType(), iterator_var, node.m_identifier->m_name);
    llvm::Value* upd_value = builder.CreateAdd(for_var, stepValue, "nextvalue");
    builder.CreateStore(upd_value, iterator_var);

    for_var = builder.CreateLoad(iterator_var->getAllocatedType(), iterator_var, node.m_identifier->m_name);

    // Condition to break a loop
    for_condition = builder.CreateICmpSLT(for_var, endValue, "loopcond");
    builder.CreateCondBr(for_condition, loopBB, afterBB);

    // parent_function->getBasicBlockList().push_back(afterBB);
    builder.SetInsertPoint(afterBB);

    std::cout << "> For-loop statement was generated\n\n";
}

void Generator::visit(parsing::While& node) {
    llvm::Function* parent_func = builder.GetInsertBlock()->getParent();

    is_lvalue = false;
    node.m_condition->accept(*this);
    llvm::Value* cond = current_expression;

    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(context, "loop", parent_func);
    llvm::BasicBlock* skipBB = llvm::BasicBlock::Create(context, "loopskip");

    builder.CreateCondBr(cond, loopBB, skipBB);

    // generating body
    builder.SetInsertPoint(loopBB);
    node.m_body->accept(*this);

    is_lvalue = false;
    node.m_condition->accept(*this);
    cond = current_expression;

    builder.CreateCondBr(cond, loopBB, skipBB);

    parent_func->getBasicBlockList().push_back(skipBB);
    builder.SetInsertPoint(skipBB);
}

void Generator::visit(parsing::Assignment& node) {
    // after this is called current_expression contains
    // the value to be assigned
    is_lvalue = false;
    node.m_expression->accept(*this);
    llvm::Value* expr_value = current_expression;

    // and this thing sets out current_lvalue thing
    // so now we have an address to store or value to
    is_lvalue = true;
    node.m_modifiable->accept(*this);

    builder.CreateStore(expr_value, current_lvalue);
}

void Generator::visit(parsing::Program& node) {
    for (const auto& decl : node.m_declarations) {
        decl->accept(*this);
    }
}

void Generator::visit(parsing::True& node) {
    current_expression = llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), 1);
}

void Generator::visit(parsing::False& node) {
    current_expression = llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), 0);
}

void Generator::visit(parsing::Math& node) {}

void Generator::visit(parsing::Real& node) {
    current_expression = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), node.m_value);
}

void Generator::visit(parsing::Boolean& node) {
    current_expression = llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), node.m_value ? 1 : 0);
}

void Generator::visit(parsing::Integer& node) {
    current_expression = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), node.m_value);
}

void Generator::visit(parsing::ArrayAccess& node) {
    std::cout << "Accessing an array...\n";

    llvm::Value* outer = current_expression;

    node.access->accept(*this);
    llvm::Value* index = current_expression;

    std::cout << "array type:";
    outer->getType()->print(llvm::errs());
    std::cout << "\n";

    if (!outer->getType()->isPointerTy()) {
        throw std::runtime_error("Outer must be a pointer to the array!");
    }

    llvm::Value* element = builder.CreateGEP(
        outer->getType()->getPointerElementType(),
        outer,
        {llvm::ConstantInt::get(context, llvm::APInt(32, 0)), index},
        "arr_index"
    );

    current_expression = element;
    current_access_type = outer->getType()->getPointerElementType()->getArrayElementType();
}

void Generator::visit(parsing::RecordAccess& node) {
    std::cout << "Accessing a record...\n";

    llvm::Value* record = current_expression;

    std::cout << "record type:";
    record->getType()->print(llvm::errs());
    std::cout << "\n";


    std::cout << "1: " << node.m_record_type << "\n";
    std::cout << "2:\n";

    int index = 0;
    for (auto& field_name : m_records_table.at(node.m_record_type)) {
        std::cout << " " << field_name << " (" << index << ")\n";

        if (field_name == node.identifier) {
            break;
        }
        index++;
    }

    llvm::Value* field_ptr = builder.CreateStructGEP(record->getType()->getPointerElementType(), record, index, node.identifier);
    current_expression = field_ptr;

    llvm::StructType* struct_type = llvm::dyn_cast<llvm::StructType>(record->getType()->getPointerElementType());
    current_access_type = struct_type->getElementType(index);
}

void Generator::visit(parsing::TypeAliasing& node) {
    std::cout << "Generating an aliasing type " << node.m_to << " as " << node.m_from->m_name << "...\n";

    auto default_type = node.m_from;
    llvm::Type* real_type = nullptr;

    node.m_from->accept(*this);

    if (auto arr_type = std::dynamic_pointer_cast<parsing::ArrayType>(default_type)) {
        real_type = typenameToType(get_array_typename(arr_type->m_type->m_name, arr_type->m_generated_size));
    } else {
        real_type = typenameToType(default_type->m_name);
    }

    m_type_table.emplace(node.m_to, real_type);

    m_ast_decl_table[node.m_to] = node.m_from;
}

void Generator::visit(parsing::StdFunction& node) {
    std::cout << "Parsing std function " << node.m_routine_name << " call...\n";

    if (node.m_routine_name == "print") {
        // function print
        llvm::Function* print_function = module->getFunction("printf");
        
        if (!print_function || print_function == nullptr) {
            auto* printfType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(context), // returns int
                llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0), // takes char*
                true // variadic function
            );
            print_function = llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", module.get());
        }

        llvm::Value* print_value = nullptr;
        llvm::Type* print_type = nullptr;

        if (node.m_parameters.size() != 1) {
            throw std::runtime_error("Invalid number of parameters for print function call: " + node.m_parameters.size());
        }

        is_lvalue = false;
        node.m_parameters[0]->accept(*this);
        print_value = current_expression;
        print_type = print_value->getType();

        llvm::Value* print_format;
        if (print_type->isIntegerTy()) {
            // int
            print_format = builder.CreateGlobalStringPtr("%d\n");
        } else if (print_type->isFloatTy() || print_type->isDoubleTy()) {
            // real
            print_format = builder.CreateGlobalStringPtr("%f\n");
        } else if (print_type->isIntegerTy(1)) {
            // bool
            print_format = builder.CreateGlobalStringPtr("%s\n");
            print_value = builder.CreateSelect(print_value, builder.CreateGlobalStringPtr("true"), builder.CreateGlobalStringPtr("false"));
        } else {
            // unrecognised => throw an error?
            print_format = builder.CreateGlobalStringPtr("%s\n");
        }

        builder.CreateCall(print_function, {print_format, print_value});
    } else {
        throw std::runtime_error("std function has no implementation: " + node.m_routine_name);
    }
}

////////////////
// Expressions:

void Generator::visit(parsing::Plus& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
        current_expression = builder.CreateAdd(left, right, "addtmp");
    } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
        current_expression = builder.CreateFAdd(left, right, "faddtmp");
    } else {
        
        // std::cout << "Right-part type: ";
        // right->getType()->print(llvm::errs());
        // std::cout << "\n";

        throw std::runtime_error("Invalid types for '+' operator. Both operands must be integer or real.");
    }
}

void Generator::visit(parsing::Minus& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
        current_expression = builder.CreateSub(left, right, "subtmp");
    } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
        current_expression = builder.CreateFSub(left, right, "fsubtmp");
    } else {
        throw std::runtime_error("Invalid types for '-' operator. Both operands must be integer or real.");
    }
}

void Generator::visit(parsing::Multiplication& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
        current_expression = builder.CreateMul(left, right, "multmp");
    } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
        current_expression = builder.CreateFMul(left, right, "fmultmp");
    } else {
        throw std::runtime_error("Invalid types for '*' operator. Both operands must be integer or real.");
    }
}

void Generator::visit(parsing::Division& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
        current_expression = builder.CreateSDiv(left, right, "divtmp");
    } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
        current_expression = builder.CreateFDiv(left, right, "fdivtmp");
    } else {
        throw std::runtime_error("Invalid types for '/' operator. Both operands must be integer or real.");
    }
}

void Generator::visit(parsing::And& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy(1) && right->getType()->isIntegerTy(1)) { // bool
        current_expression = builder.CreateAnd(left, right, "andtmp");
    } else {
        throw std::runtime_error("Invalid types for 'and' operator. Both operands must be boolean.");
    }
}

void Generator::visit(parsing::Or& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy(1) && right->getType()->isIntegerTy(1)) { // bool
        current_expression = builder.CreateOr(left, right, "ortmp");
    } else {
        throw std::runtime_error("Invalid types for 'or' operator. Both operands must be boolean.");
    }
}

void Generator::visit(parsing::Xor& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy(1) && right->getType()->isIntegerTy(1)) { // bool
        current_expression = builder.CreateXor(left, right, "xortmp");
    } else {
        throw std::runtime_error("Invalid types for 'xor' operator. Both operands must be boolean.");
    }
}

void Generator::visit(parsing::Mod& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
        current_expression = builder.CreateSRem(left, right, "modtmp");
    } else {
        throw std::runtime_error("Invalid types for 'mod' operator. Both operands must be integer.");
    }
}

void Generator::visit(parsing::Greater& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
        current_expression = builder.CreateICmpSGT(left, right, "gretmp");
    } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
        current_expression = builder.CreateFCmpUGT(left, right, "gretmp");
    } else {
        throw std::runtime_error("Invalid types for 'greater' operator. Both operands must be of the same type.");
    }
}

void Generator::visit(parsing::Less& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
        current_expression = builder.CreateICmpSLT(left, right, "lesstmp");
    } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
        current_expression = builder.CreateFCmpULT(left, right, "lesstmp");
    } else {
        throw std::runtime_error("Invalid types for 'less' operator. Both operands must be of the same type.");
    }
}

void Generator::visit(parsing::GreaterEqual& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
        current_expression = builder.CreateICmpSGE(left, right, "geqtmp");
    } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
        current_expression = builder.CreateFCmpUGE(left, right, "geqtmp");
    } else {
        throw std::runtime_error("Invalid types for 'greater equal' operator. Both operands must be of the same type.");
    }
}

void Generator::visit(parsing::LessEqual& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
        current_expression = builder.CreateICmpSLE(left, right, "leqtmp");
    } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
        current_expression = builder.CreateFCmpULE(left, right, "leqtmp");
    } else {
        throw std::runtime_error("Invalid types for 'less equal' operator. Both operands must be of the same type.");
    }
}

void Generator::visit(parsing::Equal& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
        current_expression = builder.CreateICmpEQ(left, right, "eqtmp");
    } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
        current_expression = builder.CreateFCmpOEQ(left, right, "eqtmp");
    } else {
        throw std::runtime_error("Invalid types for 'equal' operator. Both operands must be of the same type.");
    }
}

void Generator::visit(parsing::NotEqual& node) {
    llvm::Value* left = nullptr;
    llvm::Value* right = nullptr;
    gen_expr_fork(node, left, right);

    if (left->getType()->isIntegerTy() && right->getType()->isIntegerTy()) {
        current_expression = builder.CreateICmpNE(left, right, "neqtmp");
    } else if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy()) {
        current_expression = builder.CreateFCmpONE(left, right, "neqtmp");
    } else {
        throw std::runtime_error("Invalid types for 'not equal' operator. Both operands must be of the same type.");
    }
}

} // namespace generator
