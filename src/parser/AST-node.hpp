#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "grammar-units.hpp"
#include "visitor/abstract-visitor.hpp"

namespace parsing
{

struct IVisitor;

using std::cout;
class Declaration;
class Type;

class ASTNode : public virtual std::enable_shared_from_this<ASTNode>
{
public:
    explicit ASTNode(GrammarUnit gr) : m_grammar(gr)
    {
    }

    virtual ~ASTNode() = default;

    virtual bool isVariableDecl()
    {
        return false;
    }

    virtual void accept(IVisitor& visitor)
    {
        visitor.visit(*this);
    }

    virtual void accept(IVisitor&& visitor)
    {
        visitor.visit(*this);
    }

    virtual void checkReturnCoincides(
        std::shared_ptr<Type> type, std::unordered_map<std::string, std::shared_ptr<Declaration>>& table)
    {
    }

    virtual void removeUnused(std::unordered_map<std::string, int>& table)
    {
    }

    virtual GrammarUnit get_grammar() const
    {
        return this->m_grammar;
    };

    std::string gr_to_str() const
    {
        switch (this->m_grammar)
        {
            case GrammarUnit::PROGRAM:
                return "PROGRAM";
            case GrammarUnit::BODY:
                return "BODY";
            case GrammarUnit::RANGE:
                return "RANGE";
            case GrammarUnit::IDENTIFIER:
                return "IDENTIFIER";
            case GrammarUnit::VARIABLE:
                return "VARIABLE";
            case GrammarUnit::ROUTINE:
                return "ROUTINE";
            case GrammarUnit::PARAMETER:
                return "PARAMETER";
            case GrammarUnit::RECORD:
                return "RECORD";
            case GrammarUnit::TYPE:
                return "TYPE";
            case GrammarUnit::ARRAY:
                return "ARRAY";
            case GrammarUnit::IF:
                return "IF";
            case GrammarUnit::FOR:
                return "FOR";
            case GrammarUnit::WHILE:
                return "WHILE";
            case GrammarUnit::CALL:
                return "CALL";
            case GrammarUnit::ASSIGNMENT:
                return "ASSIGNMENT";
            case GrammarUnit::INTEGER:
                return "INTEGER";
            case GrammarUnit::BOOL:
                return "BOOL";
            case GrammarUnit::REAL:
                return "REAL";
            case GrammarUnit::ACCESS_ARRAY:
                return "ACCESS_ARRAY";
            case GrammarUnit::ACCESS_RECORD:
                return "ACCESS_RECORD";
            case GrammarUnit::PLUS:
                return "PLUS";
            case GrammarUnit::MINUS:
                return "MINUS";
            case GrammarUnit::MULTIPLICATE:
                return "MULTIPLICATE";
            case GrammarUnit::DIVISION:
                return "DIVISION";
            case GrammarUnit::MOD:
                return "MOD";
            case GrammarUnit::GREATER:
                return "GREATER";
            case GrammarUnit::LESS:
                return "LESS";
            case GrammarUnit::GREATER_EQUAL:
                return "GREATER_EQUAL";
            case GrammarUnit::LESS_EQUAL:
                return "LESS_EQUAL";
            case GrammarUnit::EQUAL:
                return "EQUAL";
            case GrammarUnit::NOT_EQUAL:
                return "NOT_EQUAL";
            case GrammarUnit::AND:
                return "AND";
            case GrammarUnit::OR:
                return "OR";
            case GrammarUnit::XOR:
                return "XOR";
            case GrammarUnit::TRUE:
                return "TRUE";
            case GrammarUnit::FALSE:
                return "FALSE";
            default:
                return "UNKNOWN";
        }
    }

    GrammarUnit m_grammar;
    std::weak_ptr<ASTNode> parent;
};

class Expression : public ASTNode
{
public:
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    ~Expression() override = default;

    explicit Expression() : ASTNode(GrammarUnit::DIVISION)
    {
    }

    virtual bool isConst() {
        return false;
    }

    virtual std::shared_ptr<Type> deduceType(std::unordered_map<std::string, std::shared_ptr<Declaration>>& var_table,
    std::unordered_map<std::string, std::shared_ptr<Declaration>>& type_table) = 0;
};

class Statement : public ASTNode
{
public:
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    ~Statement() override = default;

    explicit Statement(GrammarUnit gr) : ASTNode(gr)
    {
    }
};

class Declaration : public ASTNode
{
public:
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    explicit Declaration(GrammarUnit gr, std::string name) : ASTNode(gr), m_name(std::move(name))
    {
    }

    std::string m_name;
};

class Program : public ASTNode
{
public:
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void checkTypes()
    {
    }

    void removeUnused(std::unordered_map<std::string, int>& table) override
    {
        for (auto& entity : m_declarations)
        {
            entity->removeUnused(table);
        }
    }

    explicit Program() : ASTNode(GrammarUnit::PROGRAM)
    {
    }

    std::vector<std::shared_ptr<Declaration>> m_declarations;
};

class Range : public ASTNode
{
public:
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    explicit Range() : ASTNode(GrammarUnit::RANGE), m_reverse(false)
    {
    }

    bool m_reverse;
    std::shared_ptr<Expression> m_begin;
    std::shared_ptr<Expression> m_end;
};

} // namespace parsing
