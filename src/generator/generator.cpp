#include "generator.hpp"

#include "parser/visitor/abstract-visitor.hpp"
#include "parser/statement.hpp"
#include "parser/grammar-units.hpp"
#include "llvm/Analysis/StackSafetyAnalysis.h"
#include <llvm-14/llvm/IR/Function.h>

namespace generator {
// TODO(): translate AST nodes to LLVM nodes

llvm::Type* Generator::typenameToType(const std::string& name) {
    return m_type_table.at(name);
}

void Generator::gen_expr_fork(parsing::Math& node, llvm::Value*& left, llvm::Value*& right) {
    current_expression = left;
    node.m_left->accept(*this);

    current_expression = right;
    node.m_right->accept(*this);
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

void Generator::visit(parsing::ASTNode& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::Declaration& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::Type& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::RecordType& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::ArrayType& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::Variable& node) {

}

void Generator::visit(parsing::ArrayVariable& node) {
    
}

void Generator::visit(parsing::PrimitiveVariable& node) {
    
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

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", current_function);
    builder.SetInsertPoint(BB);
    // arguments generation
    int arg_idx = 0;
    for (auto &arg : node.m_params) {
        llvm::AllocaInst *space = builder.CreateAlloca(typenameToType(arg->m_name), nullptr, arg->m_name);
        auto *arg_var = current_function->getArg(arg_idx);
        builder.CreateStore(arg_var, space);

        arg_var->setName(arg->m_name);

        m_var_table[arg->m_name] = arg_var;
    }

    node.m_body->accept(*this);
    if (current_function->getReturnType() == llvm::Type::getVoidTy(context)) {
        builder.CreateRetVoid();
        return;
    }
    
    // verifyFunction(*current_function);
}

void Generator::visit(parsing::RoutineCall& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::RoutineCallResult& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::RoutineParameter& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::Statement& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::Expression& node) {
    // Target llvm::Value*
}

void Generator::visit(parsing::Modifiable& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::ReturnStatement& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::Range& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::For& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::While& node) {
    // Target llvm:: ...
}

void Generator::visit(parsing::Assignment& node) {
    
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

void Generator::visit(parsing::Math& node) {
    // TODO(): consider real numbers (.CreateFAdd)
    // llvm::Value* left = nullptr;
    // llvm::Value* rigth = nullptr;

    // llvm::Value* result = nullptr;

    // current_expression = left;
    // node.m_left->accept(*this);

    // current_expression = rigth;
    // node.m_right->accept(*this);

    // if (leftValue->getType()->isIntegerTy() && rightValue->getType()->isIntegerTy()) {
    //     switch (node.get_grammar()) {
    //         case GrammarUnit::PLUS:
    //             result = builder.CreateAdd(left, rigth, "addtmp");
    //             break;
    //         default:
    //             break;
    //     }
    // } else if (false) {
    //     // real
    // } else if (false) {
    //     // bool
    // } else {
    //     throw std::runtime_error("Both operands of '+' must be of the same type (int or real)");
    // }
    

    // current_expression = result;
}

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
