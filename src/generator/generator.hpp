#include "parser/AST-node.hpp"
#include "parser/parser.hpp"
#include "parser/visitor/abstract-visitor.hpp"
#include "parser/expression.hpp"
#include "parser/visitor/complete-visitor.hpp"

#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

namespace generator {
struct Generator : public parsing::ICompleteVisitor {
    llvm::LLVMContext context{};
    std::shared_ptr<llvm::Module> module;
    std::shared_ptr<parsing::Program> m_tree;

    // Something like Control Flow Graph
    llvm::IRBuilder<> builder;

    std::unordered_map<std::string, llvm::Type*> m_type_table;
    // we may consider storing string -> llvm::AllocaInst*
    std::unordered_map<std::string, llvm::AllocaInst*> m_var_table;
    std::unordered_map<std::string, llvm::Function*> m_routine_table;
    std::unordered_map<std::string, std::vector<std::string>> m_records_table;
    std::unordered_map<std::string, std::string> m_recordnames_table;

    llvm::Value* current_expression;
    llvm::Function* current_function;
    llvm::Value* current_lvalue;
    llvm::Type* current_access_type;

    bool is_lvalue = false;

    explicit Generator(std::shared_ptr<parsing::Program> program)
        : m_tree(program),
        module(std::make_shared<llvm::Module>("I_module", context)),
        builder(context) {}

    void apply();

    llvm::Type* typenameToType(const std::string& name);
    void gen_expr_fork(parsing::Math& node, llvm::Value*& left, llvm::Value*& right);

    void visit(parsing::ASTNode& node) override;
    void visit(parsing::Declaration& node) override;
    void visit(parsing::Type& node) override;
    void visit(parsing::RecordType& node) override;
    void visit(parsing::ArrayType& node) override;
    void visit(parsing::Variable& node) override;
    void visit(parsing::ArrayVariable& node) override;
    void visit(parsing::PrimitiveVariable& node) override;
    void visit(parsing::Body& node) override;
    void visit(parsing::Routine& node) override;
    void visit(parsing::RoutineCall& node) override;
    void visit(parsing::RoutineCallResult& node) override;
    void visit(parsing::RoutineParameter& node) override;
    void visit(parsing::Statement& node) override;
    void visit(parsing::Expression& node) override;
    void visit(parsing::Modifiable& node) override;
    void visit(parsing::ReturnStatement& node) override;
    void visit(parsing::Range& node) override;
    void visit(parsing::For& node) override;
    void visit(parsing::While& node) override;
    void visit(parsing::If& node) override;
    void visit(parsing::Assignment& node) override;
    void visit(parsing::Program& node) override;
    void visit(parsing::True& node) override;
    void visit(parsing::False& node) override;
    void visit(parsing::Math& node) override;
    void visit(parsing::Real& node) override;
    void visit(parsing::Boolean& node) override;
    void visit(parsing::Integer& node) override;
    void visit(parsing::ArrayAccess& node) override;
    void visit(parsing::RecordAccess& node) override;
    void visit(parsing::TypeAliasing& node) override;

    void visit(parsing::Plus& node) override;
    void visit(parsing::Minus& node) override;
    void visit(parsing::Multiplication& node) override;
    void visit(parsing::Division& node) override;
    void visit(parsing::And& node) override;
    void visit(parsing::Or& node) override;
    void visit(parsing::Xor& node) override;
    void visit(parsing::Mod& node) override;
    void visit(parsing::Greater& node) override;
    void visit(parsing::Less& node) override;
    void visit(parsing::GreaterEqual& node) override;
    void visit(parsing::LessEqual& node) override;
    void visit(parsing::Equal& node) override;
    void visit(parsing::NotEqual& node) override;

    void visit(parsing::StdFunction& node) override;
};
} // namespace generator
