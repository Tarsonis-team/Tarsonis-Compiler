#pragma once

namespace parsing {

struct ASTNode;
struct Declaration;
struct Type;
struct RecordType;
struct ArrayType;
struct ArrayVariable;
struct PrimitiveVariable;
struct Body;
struct Routine;
struct RoutineCall;
struct RoutineCallResult;
struct RoutineParameter;
struct Statement;
struct Expression;
struct Modifiable;
struct ReturnStatement;
struct Range;
struct For;
struct While;
struct If;
struct Assignment;
struct Program;
struct True;
struct False;
struct Math;
struct Real;
struct Boolean;
struct Integer;
struct RecordAccess;
struct ArrayAccess;
struct TypeAliasing;
struct Variable;

struct Plus;
struct Minus;
struct Multiplication;
struct Division;
struct And;
struct Or;
struct Xor;
struct Mod;
struct Greater;
struct Less;
struct GreaterEqual;
struct LessEqual;
struct Equal;
struct NotEqual;

// Damn, need to use templates in accept function in nodes...
struct ICompleteVisitor {

    virtual void visit(ASTNode& node) = 0;
    virtual void visit(Declaration& node) = 0;
    virtual void visit(Type& node) = 0;
    virtual void visit(RecordType& node) = 0;
    virtual void visit(ArrayType& node) = 0;
    virtual void visit(Variable& node) = 0;
    virtual void visit(ArrayVariable& node) = 0;
    virtual void visit(PrimitiveVariable& node) = 0;
    virtual void visit(Body& node) = 0;
    virtual void visit(Routine& node) = 0;
    virtual void visit(RoutineCall& node) = 0;
    virtual void visit(RoutineCallResult& node) = 0;
    virtual void visit(RoutineParameter& node) = 0;
    virtual void visit(Statement& node) = 0;
    virtual void visit(Expression& node) = 0;
    virtual void visit(Modifiable& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    virtual void visit(Range& node) = 0;
    virtual void visit(For& node) = 0;
    virtual void visit(While& node) = 0;
    virtual void visit(If& node) = 0;
    virtual void visit(Assignment& node) = 0;
    virtual void visit(Program& node) = 0;
    virtual void visit(True& node) = 0;
    virtual void visit(False& node) = 0;
    virtual void visit(Math& node) = 0;
    virtual void visit(Real& node) = 0;
    virtual void visit(Boolean& node) = 0;
    virtual void visit(Integer& node) = 0;
    virtual void visit(ArrayAccess& node) = 0;
    virtual void visit(RecordAccess& node) = 0;
    virtual void visit(TypeAliasing& node) = 0;

    virtual void visit(parsing::Plus& node) = 0;
    virtual void visit(parsing::Minus& node) = 0;
    virtual void visit(parsing::Multiplication& node)= 0;
    virtual void visit(parsing::Division& node) = 0;
    virtual void visit(parsing::And& node) = 0;
    virtual void visit(parsing::Or& node) = 0;
    virtual void visit(parsing::Xor& node) = 0;
    virtual void visit(parsing::Mod& node) = 0;
    virtual void visit(parsing::Greater& node) = 0;
    virtual void visit(parsing::Less& node) = 0;
    virtual void visit(parsing::GreaterEqual& node) = 0;
    virtual void visit(parsing::LessEqual& node) = 0;
    virtual void visit(parsing::Equal& node) = 0;
    virtual void visit(parsing::NotEqual& node) = 0;
};

}  // namespace parsing
