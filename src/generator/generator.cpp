#include "generator.hpp"

#include "parser/visitor/abstract-visitor.hpp"
#include "parser/statement.hpp"
#include "parser/return.hpp"
#include "parser/std-function.hpp"
#include "llvm/Analysis/StackSafetyAnalysis.h"
#include <llvm-14/llvm/IR/Function.h>

namespace generator {

llvm::Type* Generator::typenameToType(const std::string& name) {
    if (name.empty()) {
        return llvm::Type::getVoidTy(context);;
    }
    return m_type_table.at(name);
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
    m_type_table.emplace("real", llvm::Type::getFloatTy(context));

    m_tree->accept(*this);
    module->print(llvm::outs(), nullptr);
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
    // Target llvm:: ...
}

void Generator::visit(parsing::ArrayType& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::Variable& node) {}

void Generator::visit(parsing::ArrayVariable& node) {
    
}

void Generator::visit(parsing::PrimitiveVariable& node) {
    // this is never an array, arrays get dispathed into ArrayVariable visit method
    llvm::AllocaInst *var = builder.CreateAlloca(typenameToType(node.m_type->m_name), nullptr, node.m_name);

    if (node.m_assigned) {
        node.m_assigned->accept(*this);
        builder.CreateStore(current_expression, var);
    }
    m_var_table[node.m_name] = var;
}

void Generator::visit(parsing::Body& node) {
    for(const auto& stmt : node.m_items) {
        stmt->accept(*this);
    }
}

void Generator::visit(parsing::Routine& node) {
    // Generate types of arguments
    std::vector<llvm::Type*> arg_types{};
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
        // llvm::AllocaInst *space = builder.CreateAlloca(typenameToType(arg->m_name), nullptr, arg->m_name);
        // m_var_table[arg->m_name] = space;
        arg->accept(*this);

        
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

    if (m_routine_table.count(node.m_routine_name) == 0) {
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
        par->accept(*this);
        params.push_back(current_expression);
    }

    current_expression = builder.CreateCall(routine, params, "call_" + node.m_routine_name);
}

void Generator::visit(parsing::RoutineCallResult& node) {
    node.m_routine_call->accept(*this);
}

void Generator::visit(parsing::RoutineParameter& node) {
    llvm::AllocaInst *space = builder.CreateAlloca(typenameToType(node.m_type), nullptr, node.m_name);
    m_var_table[node.m_name] = space;

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
}

void Generator::visit(parsing::ReturnStatement& node) {
    std::cout << "Generating return statement...\n";

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

    node.m_range->m_begin->accept(*this);
    startValue = current_expression;

    node.m_range->m_end->accept(*this);
    endValue = current_expression;

    // Creating identifier (var i)
    llvm::AllocaInst* iterator_var = builder.CreateAlloca(startValue->getType(), nullptr, node.m_identifier->m_name);
    builder.CreateStore(startValue, iterator_var);

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

    node.m_condition->accept(*this);
    llvm::Value* cond = current_expression;

    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(context, "loop", parent_func);
    llvm::BasicBlock* skipBB = llvm::BasicBlock::Create(context, "loopskip");

    builder.CreateCondBr(cond, loopBB, skipBB);

    // generating body
    builder.SetInsertPoint(loopBB);
    node.m_body->accept(*this);
    builder.CreateCondBr(cond, loopBB, skipBB);

    parent_func->getBasicBlockList().push_back(skipBB);
    builder.SetInsertPoint(skipBB);
}

void Generator::visit(parsing::Assignment& node) {
    // after this is called current_expression contains
    // the value to be assigned
    node.m_expression->accept(*this);

    // and this thing sets out current_lvalue thing
    // so now we have an address to store or value to
    is_lvalue = true;
    node.m_modifiable->accept(*this);

    builder.CreateStore(current_expression, current_lvalue);
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
    // Target llvm:: ...
}

void Generator::visit(parsing::RecordAccess& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::TypeAliasing& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::StdFunction& node) {
    std::cout << "Parsing std function " << node.m_routine_name << " call...\n";
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
